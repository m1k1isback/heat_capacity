#include "physicsengine.h"
#include "controlpanelwidget.h"
#include <cmath>

// Задание всех начальных величин, соединение таймера с onTimerTick
PhysicsEngine::PhysicsEngine(QObject *parent)
    : QObject(parent)
{
    m_elapsedSec = 0;
    m_state = ExperimentState::Idle;
    m_pointCount = 0;

    m_timer = new QTimer(this);
    m_envTemp = 20.0;
    connect(m_timer, &QTimer::timeout, this, &PhysicsEngine::onTimerTick);

    for(int i = 0; i < 4; i++){ // изначально режим основной у всех
        m_isDifferentialMode[i] = false;
    }
}

// Собираем все необходимые величины, отправляем температуры на дисплеи
void PhysicsEngine::configure(double envTemp, const QVector<Sample>& samples)
{
    m_envTemp = envTemp;
    m_samples = samples;

    // Перед стартом все образцы должны быть в тепловом равновесии со средой
    for(int i = 0; i < m_samples.size(); i++){
        m_samples[i].currentTemp = envTemp;
    }

    m_pointCount = 0;
    m_elapsedSec = 0;
    m_coolingStartSec = 0;
    m_T1_CoolingStart = envTemp;

    // Сразу отправляем сигнал, чтобы дисплеи обновились на актуальную T0
    QVector<double> temps;
    for(const auto& s : m_samples) temps.append(s.currentTemp);
    emit temperaturesUpdated(temps, 0);
}

// Запуск нагрева
void PhysicsEngine::startHeating()
{
    if(m_state != ExperimentState::Idle) return;
    m_state = ExperimentState::Heating;
    emit stateChanged(m_state);
    m_timer->start(1);
}

// Запуск термостатирования
void PhysicsEngine::startThermostatting()
{
    if(m_state != ExperimentState::Heating) return;
    m_state = ExperimentState::Thermostate;
    emit stateChanged(m_state);
}

// Функция записи точек
void PhysicsEngine::recordPoint()
{
    // Запись первой точки, которая находилась в фазе термостатирования
    if(m_state == ExperimentState::Thermostate){
        m_state = ExperimentState::Cooling;
        m_coolingStartSec = m_elapsedSec;

        // Запоминаем реальную температуру старта остывания
        for(const auto& s : m_samples){
            if(s.isActive){
                m_T1_CoolingStart = s.currentTemp;
                break;  // Достаточно первого активного образца
            }
        }

        m_coolingTimeSec = 0; // началось остывание - обнуляем таймер

        emit stateChanged(m_state); // Изменяем фазу
    }

    // Запись точек уже в процессе фазы остывания
    if(m_state == ExperimentState::Cooling){
        QVector<double> temps;

        // Собираем температуры всех образцов
        for(int i = 0; i < m_samples.size(); i++){
            if (m_isDifferentialMode[i]) {
                // Если режим ΔT: отнимаем температуру среды
                temps.append(m_samples[i].currentTemp - m_envTemp);
            } else {
                // Если обычный режим: просто температура
                temps.append(m_samples[i].currentTemp);
            }
        }

        // Считаем время остывания
        double coolingTimeSec = m_elapsedSec - m_coolingStartSec;

        // Увеличиваем счётчик и отправляем данные
        m_pointCount++;
        emit pointsCountUpdated(m_pointCount);
        emit pointRecorded(m_pointCount, coolingTimeSec, temps);
    }
}

void PhysicsEngine::setDifferentialMode(int index, bool enabled){
    if (index < 0 || index >= m_samples.size()) return;
    m_isDifferentialMode[index] = enabled;
}

// Полный сброс
void PhysicsEngine::reset()
{
    m_timer->stop();
    m_state = ExperimentState::Idle;
    emit stateChanged(m_state);

    m_elapsedSec = 0;
    m_coolingStartSec = 0;
    m_pointCount = 0;
    m_coolingTimeSec = 0;
    emit timeUpdated(0);
    emit pointsCountUpdated(0);
    emit coolingTimeUpdated(0);
    emit tableReset();

    for(int i = 0; i < m_samples.size(); i++){
        m_samples[i].currentTemp = m_envTemp;
    }
    QVector<double> temps;
    for (int i = 0; i < m_samples.size(); i++){
        temps.append(m_samples[i].currentTemp);
    }
    emit temperaturesUpdated(temps, 0);
}

// Ключевая функция расчета температуры
void PhysicsEngine::onTimerTick()
{
    qDebug() << "⚙️ Адрес работающего движка:" << this;
    m_elapsedSec += 0.001; // Увеличиваем время

    bool tempsChanged = false;

    if(m_state == ExperimentState::Cooling){
        m_coolingTimeSec += 0.001;
    }

    // Линейное возрастание температуры
    switch (m_state) {
    case ExperimentState::Heating: {
        // === Нагрев ===
        for(int i = 0; i < m_samples.size(); i++){
            if(m_samples[i].isActive){
                m_samples[i].currentTemp += 0.001; // Шаг 0.01
                tempsChanged = true;
            }
        }
        break;
    }

    // Просто сохраняем текущую температуру
    case ExperimentState::Thermostate: {
        tempsChanged = true;
        break;
    }

    // Остывание
    case ExperimentState::Cooling: {
        double t_min = m_coolingTimeSec / 60.0; // Время в минутах
        emit coolingTimeUpdated(m_coolingTimeSec);

        for(int i = 0; i < m_samples.size(); i++){
            if(m_samples[i].isActive){
                double c_ob = m_samples[i].calculateHeatCapacity();
                double denominator = c_ob + C_K;
                if(denominator <= 0) continue;

                double exponent = -(K_CONST * t_min) / denominator;
                double newTemp = m_envTemp + (m_T1_CoolingStart - m_envTemp) * std::exp(exponent);

                m_samples[i].currentTemp = newTemp;
                tempsChanged = true;
            }
        }
        break;
    }
    default:
        break;
    }

    // === Отправка сигнала ===
    if(tempsChanged == true || m_state == ExperimentState::Thermostate){
        QVector<double> temps;
        for(int i = 0; i < m_samples.size(); i++){
            temps.append(m_samples[i].currentTemp);
        }
        emit temperaturesUpdated(temps, m_elapsedSec); // обновляем дисплеи
        emit timeUpdated(static_cast<int>(m_elapsedSec)); // обновляем таймер
    }
}

void PhysicsEngine::setSampleActive(int index, bool active){
    if (index < 0 || index >= 4) return;
    if (index >= m_samples.size()) return;

    m_samples[index].isActive = active;
}
