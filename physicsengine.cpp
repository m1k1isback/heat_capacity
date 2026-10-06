#include "physicsengine.h"
#include "controlpanelwidget.h"
#include <cmath>
#include <QRandomGenerator> // <--- ДОБАВЛЕНО: Для генерации случайных чисел (шум)

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

    // <--- ДОБАВЛЕНО: Таймер для обновления дисплеев с шумом (раз в 0.5 сек)
    m_displayTimer = new QTimer(this);
    m_displayTimer->setInterval(500);
    connect(m_displayTimer, &QTimer::timeout, this, &PhysicsEngine::onDisplayUpdateTick);

    for(int i = 0; i < 4; i++){ // изначально режим основной у всех
        m_isDifferentialMode[i] = false;
        m_lastNoisyTemps.append(m_envTemp); // <--- ДОБАВЛЕНО: Инициализация массива шума
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
        m_lastNoisyTemps[i] = envTemp; // <--- ДОБАВЛЕНО: Сброс шума при конфигурации
    }

    m_pointCount = 0;
    m_elapsedSec = 0;
    m_coolingStartSec = 0;
    m_T1_CoolingStart = envTemp;

    // Сразу отправляем сигнал, чтобы дисплеи обновились на актуальную T0
    QVector<double> temps;
    for(const auto& s : m_samples) temps.append(s.currentTemp);
    emit temperaturesUpdated(temps, 0);
    emit displayTemperaturesUpdated(temps, 0); // <--- ДОБАВЛЕНО: Инициализация дисплеев
}

// Запуск нагрева
void PhysicsEngine::startHeating()
{
    if(m_state != ExperimentState::Idle) return;
    m_state = ExperimentState::Heating;
    emit stateChanged(m_state);
    m_timer->start(1);
    qDebug() << "=== ЗАПУСКАЕМ ТАЙМЕР ДИСПЛЕЯ ===";
    m_displayTimer->start(); // <--- ДОБАВЛЕНО: Запуск таймера дисплеев
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
    // 1. Переход в фазу остывания
    if(m_state == ExperimentState::Thermostate){
        m_state = ExperimentState::Cooling;
        m_coolingStartSec = m_elapsedSec;

        for(const auto& s : m_samples){
            if(s.isActive){
                m_T1_CoolingStart = s.currentTemp;
                break;
            }
        }
        m_coolingTimeSec = 0;
        emit stateChanged(m_state);
    }

    // 2. Запись точки
    if(m_state == ExperimentState::Cooling){
        // ОСТАНАВЛИВАЕМ таймер шума, чтобы он не перезаписал значение во время клика
        m_displayTimer->stop();

        QVector<double> tempsToRecord;
        QVector<double> tempsForDisplay;

        for(int i = 0; i < m_samples.size(); i++){
            // Генерируем шум ПРЯМО В МОМЕНТ КЛИКА
            double noise = QRandomGenerator::global()->bounded(-50, 50) / 1000.0; // ±0.05
            double noisyVal = m_samples[i].currentTemp + noise;

            // Сохраняем в кэш
            m_lastNoisyTemps[i] = noisyVal;

            // Формируем массивы
            tempsForDisplay.append(noisyVal);
            tempsToRecord.append(m_isDifferentialMode[i] ? (noisyVal - m_envTemp) : noisyVal);
        }

        double coolingTimeSec = m_elapsedSec - m_coolingStartSec;
        m_pointCount++;
        emit pointsCountUpdated(m_pointCount);

        // 3. ПРИНУДИТЕЛЬНО обновляем дисплей этими значениями
        emit displayTemperaturesUpdated(tempsForDisplay, m_elapsedSec);

        // 4. Отправляем в таблицу ТЕ ЖЕ САМЫЕ значения
        emit pointRecorded(m_pointCount, coolingTimeSec, tempsToRecord);

        // 5. Перезапускаем таймер шума (он снова начнет "фонить" через 0.5 сек)
        m_displayTimer->start();
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
    m_displayTimer->stop(); // <--- ДОБАВЛЕНО: Остановка таймера дисплеев
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
        m_lastNoisyTemps[i] = m_envTemp; // <--- ДОБАВЛЕНО: Сброс шума
    }

    QVector<double> temps;
    for (int i = 0; i < m_samples.size(); i++){
        temps.append(m_samples[i].currentTemp);
    }
    emit temperaturesUpdated(temps, 0);
    emit displayTemperaturesUpdated(temps, 0); // <--- ДОБАВЛЕНО: Сброс дисплеев
}

// Ключевая функция расчета температуры
void PhysicsEngine::onTimerTick()
{
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
    // Этот сигнал идет на ГРАФИК (он остается идеально плавным, без шума)
    if(tempsChanged == true || m_state == ExperimentState::Thermostate){
        QVector<double> temps;
        for(int i = 0; i < m_samples.size(); i++){
            temps.append(m_samples[i].currentTemp);
        }
        emit temperaturesUpdated(temps, m_elapsedSec); // обновляем дисплеи (график)
        emit timeUpdated(static_cast<int>(m_elapsedSec)); // обновляем таймер
    }
}

void PhysicsEngine::setSampleActive(int index, bool active){
    if (index < 0 || index >= 4) return;
    if (index >= m_samples.size()) return;

    m_samples[index].isActive = active;
}

// =====================================================================
// <--- ДОБАВЛЕНО: Новые методы для реализации шума дисплеев
// =====================================================================

// Генерация простого белого шума в диапазоне ±0.05 градуса
double PhysicsEngine::generateNoise(double trueTemp) {
    // bounded(-50, 50) дает число от -50 до 49. Делим на 1000.0, получаем от -0.05 до +0.049
    double noise = QRandomGenerator::global()->bounded(-50, 50) / 1000.0;
    return trueTemp + noise;
}

void PhysicsEngine::onDisplayUpdateTick() {
    QVector<double> noisyTemps;
    for(int i = 0; i < m_samples.size(); i++) {
        if (m_samples[i].isActive) {
            noisyTemps.append(generateNoise(m_samples[i].currentTemp));
        } else {
            // Если образец выключен, показываем температуру среды без сильного шума
            noisyTemps.append(m_envTemp);
        }
    }

    // Сохраняем эти значения, чтобы метод recordPoint() взял именно их для протокола
    m_lastNoisyTemps = noisyTemps;

    qDebug() << "Отправляем сигнал: T1=" << noisyTemps[0]
             << "T2=" << noisyTemps[1]
             << "T3=" << noisyTemps[2]
             << "T4=" << noisyTemps[3];

    // Отправляем сигнал ТОЛЬКО на дисплеи (UI)
    emit displayTemperaturesUpdated(noisyTemps, m_elapsedSec);
}
