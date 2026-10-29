#include "physicsengine.h"
#include <cmath>
#include <QRandomGenerator>
#include <QtGlobal> // для qBound

// Константы калориметра (из твоего интерфейса)
const double K_CALORIMETER_PER_MIN = 5.20;  // Дж/(мин·К)
const double K_CALORIMETER_PER_SEC = K_CALORIMETER_PER_MIN / 60.0; // Дж/(с·К)
const double C_K = 210.0;                   // Теплоемкость самого калориметра, Дж/К

PhysicsEngine::PhysicsEngine(QObject *parent)
    : QObject(parent),
    m_state(ExperimentState::Idle),
    m_elapsedSec(0),
    m_T0(20.0),
    m_isAcceleratedMode(false),
    m_timer(new QTimer(this)),
    m_pointsCount(0),
    m_startTemp(0.0)
{
    m_previousTemps.resize(4, 0.0);
    m_coolingRates.resize(4, 0.0);
    m_lastRecordedTemps.resize(4, 0.0);

    connect(m_timer, &QTimer::timeout, this, &PhysicsEngine::onTimerTick);
}

void PhysicsEngine::configure(double t0, const QVector<Sample>& samples)
{
    m_T0 = t0;
    m_samples = samples;
    emit samplesConfigured(m_samples);
}

void PhysicsEngine::setTimeMode(bool isAccelerated)
{
    m_isAcceleratedMode = isAccelerated;
}

void PhysicsEngine::startCooling(double initialTemp)
{
    m_state = ExperimentState::Cooling;
    m_elapsedSec = 0;
    m_pointsCount = 0;
    m_startTemp = initialTemp;

    for (int i = 0; i < 4; ++i) {
        if (m_samples[i].isActive) {
            m_samples[i].currentTemp = initialTemp;
            m_previousTemps[i] = initialTemp;
            m_coolingRates[i] = 0.0;
            m_lastRecordedTemps[i] = initialTemp;
        } else {
            m_samples[i].currentTemp = m_T0;
            m_previousTemps[i] = m_T0;
            m_coolingRates[i] = 0.0;
            m_lastRecordedTemps[i] = m_T0;
        }
    }

    emit stateChanged(m_state);
    emit timeUpdated(0);
    emit pointsCountUpdated(0);

    emit coolingRatesUpdated(m_coolingRates);
    emit progressUpdated(QVector<double>(4, 0.0));

    m_timer->start(1000); // 1 тик = 1 реальная секунда
}

void PhysicsEngine::reset()
{
    m_timer->stop();
    m_state = ExperimentState::Idle;
    m_elapsedSec = 0;
    m_pointsCount = 0;
    emit stateChanged(m_state);
    emit timeUpdated(0);
    emit pointsCountUpdated(0);
    emit tableReset();
}

void PhysicsEngine::recordPoint()
{
    if (m_state != ExperimentState::Cooling) return;

    m_pointsCount++;
    QVector<double> currentTemps(4);
    for (int i = 0; i < 4; ++i) {
        currentTemps[i] = m_samples[i].isActive ? m_samples[i].currentTemp : 0.0;
    }

    emit pointRecorded(m_pointsCount, m_elapsedSec, currentTemps);
    emit pointsCountUpdated(m_pointsCount);
}

void PhysicsEngine::onTimerTick()
{
    if (m_state != ExperimentState::Cooling) return;

    m_elapsedSec++;
    emit timeUpdated(m_elapsedSec);

    QVector<double> newTemps(4);
    QVector<double> progress(4, 0.0);
    double dt = 1.0; // Шаг времени = 1 секунда

    for (int i = 0; i < 4; ++i) {
        if (!m_samples[i].isActive) {
            newTemps[i] = m_T0;
            m_coolingRates[i] = 0.0;
            progress[i] = 0.0;
            continue;
        }

        double currentT = m_samples[i].currentTemp;

        // =====================================================================
        // РЕАЛЬНАЯ ФИЗИКА: Закон охлаждения Ньютона-Рихмана
        // dT/dt = - (k / C_полная) * (T - T0)
        // =====================================================================

        // 1. Масса в килограммах
        double mass_kg = m_samples[i].massGrams / 1000.0;

        // 1. Теплоемкость образца (уже рассчитана в Sample с учетом модели Дебая)
        // Метод calculateHeatCapacity() возвращает C_обр = m * c_уд(T) в Дж/К
        double C_sample = m_samples[i].calculateHeatCapacity();

        // 2. Полная теплоемкость системы: C_полная = C_калориметра + C_образца
        double C_total = C_K + C_sample;

        // 3. Скорость изменения температуры (°C в секунду)
        double dT_dt = -(K_CALORIMETER_PER_SEC / C_total) * (currentT - m_T0);

        // 4. Новая температура
        newTemps[i] = currentT + dT_dt * dt;
        // =====================================================================

        m_samples[i].currentTemp = newTemps[i];

        // Расчет скорости остывания (°C в минуту) для отображения в статусе
        m_coolingRates[i] = (newTemps[i] - m_previousTemps[i]) * 60.0;
        m_previousTemps[i] = newTemps[i];

        // Расчет прогресса остывания (%)
        if (m_startTemp > m_T0) {
            double totalRange = m_startTemp - m_T0;
            double cooledRange = m_startTemp - newTemps[i];
            progress[i] = qBound(0.0, (cooledRange / totalRange) * 100.0, 100.0);
        }
    }

    // Отправляем обновления в UI
    emit coolingRatesUpdated(m_coolingRates);
    emit progressUpdated(progress);
    emit temperaturesUpdated(newTemps, m_elapsedSec);

    // Добавляем измерительный шум для дисплеев
    QVector<double> noisyTemps = addMeasurementNoise(newTemps);
    emit displayTemperaturesUpdated(noisyTemps, m_elapsedSec);
}

QVector<double> PhysicsEngine::addMeasurementNoise(const QVector<double>& temps) const
{
    QVector<double> noisy = temps;
    for (int i = 0; i < 4; ++i) {
        if (m_samples[i].isActive) {
            // Случайный шум +/- 0.15 градуса (реалистичная погрешность термопары)
            double noise = (QRandomGenerator::global()->generateDouble() - 0.5) * 0.3;
            noisy[i] += noise;
        }
    }
    return noisy;
}

void PhysicsEngine::setSampleActive(int index, bool active)
{
    if (index >= 0 && index < m_samples.size()) {
        m_samples[index].isActive = active;
    }
}

void PhysicsEngine::setDifferentialMode(int index, bool enabled)
{
    Q_UNUSED(index);
    Q_UNUSED(enabled);
    // Дифференциальный режим обрабатывается на уровне отображения (в сцене)
}
