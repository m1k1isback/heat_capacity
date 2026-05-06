#include "physicsengine.h"
#include "controlpanelwidget.h"
#include <cmath>

PhysicsEngine::PhysicsEngine(QObject *parent)
    : QObject(parent)
{
    m_elapsedSec = 0;
    m_state = ExperimentState::Idle;
    m_pointCount = 0;

    m_timer = new QTimer(this);
    m_envTemp = 20.0;
    connect(m_timer, &QTimer::timeout, this, &PhysicsEngine::onTimerTick);
}

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

void PhysicsEngine::recordPoint()
{
    // === Сценарий А: Переход от термостатирования к остыванию ===
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

        // Отправляем сигнал ОДИН раз, после всех изменений
        emit stateChanged(m_state);
    }

    // === Сценарий Б: Запись точки во время остывания ===
    if(m_state == ExperimentState::Cooling){
        QVector<double> temps;

        // Собираем температуры всех образцов
        for(int i = 0; i < m_samples.size(); i++){
            temps.append(m_samples[i].currentTemp);
        }

        // Считаем время остывания
        double coolingTimeSec = m_elapsedSec - m_coolingStartSec;

        // Увеличиваем счётчик и отправляем данные
        m_pointCount++;
        emit pointsCountUpdated(m_pointCount);
        emit pointRecorded(m_pointCount, coolingTimeSec, temps);
    }
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
    emit timeUpdated(0);
    emit pointsCountUpdated(0);

    for(int i = 0; i < m_samples.size(); i++){
        m_samples[i].currentTemp = m_envTemp;
    }
    QVector<double> temps;
    for (int i = 0; i < m_samples.size(); i++){
        temps.append(m_samples[i].currentTemp);
    }
    emit temperaturesUpdated(temps, 0);
}

void PhysicsEngine::onTimerTick()
{
    m_elapsedSec += 0.001; // Увеличиваем время

    bool tempsChanged = false;

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

    case ExperimentState::Thermostate: {
        tempsChanged = true;
        break;
    }

    case ExperimentState::Cooling: {
        // === Остывание ===
        double t_min = (m_elapsedSec - m_coolingStartSec) / 60.0; // Время в минутах

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
        emit temperaturesUpdated(temps, m_elapsedSec);

        emit timeUpdated(static_cast<int>(m_elapsedSec));
    }
}
