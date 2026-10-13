#include "physicsengine.h"
#include <cmath>
#include <QRandomGenerator>
#include <QDebug>

PhysicsEngine::PhysicsEngine(QObject *parent)
    : QObject(parent), m_isAcceleratedMode(true) // По умолчанию ускоренный режим для удобства
{
    m_elapsedSec = 0;
    m_state = ExperimentState::Idle;
    m_pointCount = 0;

    // Таймер физики: тикает каждые 100 мс (0.1 сек) для плавности
    m_timer = new QTimer(this);
    m_timer->setInterval(100);
    m_envTemp = 20.0;
    connect(m_timer, &QTimer::timeout, this, &PhysicsEngine::onTimerTick);

    // Таймер дисплеев: тикает каждые 500 мс для эффекта "шума"
    m_displayTimer = new QTimer(this);
    m_displayTimer->setInterval(500);
    connect(m_displayTimer, &QTimer::timeout, this, &PhysicsEngine::onDisplayUpdateTick);

    for(int i = 0; i < 4; i++){
        m_isDifferentialMode[i] = false;
        m_lastNoisyTemps.append(m_envTemp);
    }
}

void PhysicsEngine::configure(double envTemp, const QVector<Sample>& samples)
{
    m_envTemp = envTemp;
    m_samples = samples;

    for(int i = 0; i < m_samples.size(); i++){
        m_samples[i].currentTemp = envTemp;
        m_lastNoisyTemps[i] = envTemp;
    }

    m_pointCount = 0;
    m_elapsedSec = 0;
    m_coolingStartSec = 0;
    m_T1_CoolingStart = envTemp;

    QVector<double> temps;
    for(const auto& s : m_samples) temps.append(s.currentTemp);
    emit temperaturesUpdated(temps, 0);
    emit displayTemperaturesUpdated(temps, 0);
}

// НОВОЕ: Мгновенный старт остывания с заданной температуры
void PhysicsEngine::startCooling(double initialTemp)
{
    if(m_state != ExperimentState::Idle) return;

    m_state = ExperimentState::Cooling;
    m_T1_CoolingStart = initialTemp;
    m_coolingStartSec = 0;
    m_coolingTimeSec = 0;

    // Мгновенно устанавливаем начальную температуру всем активным образцам
    for(int i = 0; i < m_samples.size(); i++){
        if(m_samples[i].isActive){
            m_samples[i].currentTemp = initialTemp;
        }
    }

    emit stateChanged(m_state);
    m_timer->start();
    m_displayTimer->start();
}

void PhysicsEngine::setTimeMode(bool isAccelerated)
{
    m_isAcceleratedMode = isAccelerated;
}

void PhysicsEngine::recordPoint()
{
    // Теперь запись возможна только в фазе Cooling
    if(m_state == ExperimentState::Cooling){
        m_displayTimer->stop(); // Останавливаем шум, чтобы зафиксировать значение

        QVector<double> tempsToRecord;
        QVector<double> tempsForDisplay;

        for(int i = 0; i < m_samples.size(); i++){
            double noise = QRandomGenerator::global()->bounded(-50, 50) / 1000.0; // ±0.05
            double noisyVal = m_samples[i].currentTemp + noise;

            m_lastNoisyTemps[i] = noisyVal;
            tempsForDisplay.append(noisyVal);
            tempsToRecord.append(m_isDifferentialMode[i] ? (noisyVal - m_envTemp) : noisyVal);
        }

        double coolingTimeSec = m_elapsedSec - m_coolingStartSec;
        m_pointCount++;
        emit pointsCountUpdated(m_pointCount);

        emit displayTemperaturesUpdated(tempsForDisplay, m_elapsedSec);
        emit pointRecorded(m_pointCount, coolingTimeSec, tempsToRecord);

        m_displayTimer->start(); // Возобновляем шум
    }
}

void PhysicsEngine::setDifferentialMode(int index, bool enabled){
    if (index < 0 || index >= m_samples.size()) return;
    m_isDifferentialMode[index] = enabled;
}

void PhysicsEngine::reset()
{
    m_timer->stop();
    m_displayTimer->stop();
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
        m_lastNoisyTemps[i] = m_envTemp;
    }

    QVector<double> temps;
    for (int i = 0; i < m_samples.size(); i++){
        temps.append(m_samples[i].currentTemp);
    }
    emit temperaturesUpdated(temps, 0);
    emit displayTemperaturesUpdated(temps, 0);
}

void PhysicsEngine::onTimerTick()
{
    // НОВОЕ: Расчет шага времени в зависимости от режима
    // Если ускоренный: 1 тик (0.1 сек реального времени) = 5 минут (300 сек) симуляции
    // Если реальный: 1 тик = 0.1 сек симуляции
    double timeStep = m_isAcceleratedMode ? 300.0 : 0.1;

    m_elapsedSec += timeStep;

    if(m_state == ExperimentState::Cooling){
        m_coolingTimeSec += timeStep;
        emit coolingTimeUpdated(m_coolingTimeSec);

        double t_min = m_coolingTimeSec / 60.0; // Время в минутах для формулы

        for(int i = 0; i < m_samples.size(); i++){
            if(m_samples[i].isActive){
                double c_ob = m_samples[i].calculateHeatCapacity();
                double denominator = c_ob + C_K;
                if(denominator <= 0) continue;

                double exponent = -(K_CONST * t_min) / denominator;
                double newTemp = m_envTemp + (m_T1_CoolingStart - m_envTemp) * std::exp(exponent);

                m_samples[i].currentTemp = newTemp;
            }
        }
    }

    // Отправка гладких температур (для графиков, если они есть)
    QVector<double> temps;
    for(int i = 0; i < m_samples.size(); i++){
        temps.append(m_samples[i].currentTemp);
    }
    emit temperaturesUpdated(temps, m_elapsedSec);
    emit timeUpdated(static_cast<int>(m_elapsedSec));
}

void PhysicsEngine::setSampleActive(int index, bool active){
    if (index < 0 || index >= 4) return;
    if (index >= m_samples.size()) return;
    m_samples[index].isActive = active;
}

double PhysicsEngine::generateNoise(double trueTemp) {
    double noise = QRandomGenerator::global()->bounded(-50, 50) / 1000.0;
    return trueTemp + noise;
}

void PhysicsEngine::onDisplayUpdateTick() {
    QVector<double> noisyTemps;
    for(int i = 0; i < m_samples.size(); i++) {
        if (m_samples[i].isActive) {
            noisyTemps.append(generateNoise(m_samples[i].currentTemp));
        } else {
            noisyTemps.append(m_envTemp);
        }
    }
    m_lastNoisyTemps = noisyTemps;
    emit displayTemperaturesUpdated(noisyTemps, m_elapsedSec);
}
