#include "physicsengine.h"
#include <cmath>
#include <QRandomGenerator>
#include <QtGlobal>

const double K_CALORIMETER_PER_MIN = 5.20;
const double K_CALORIMETER_PER_SEC = K_CALORIMETER_PER_MIN / 60.0;
const double C_K = 210.0;

PhysicsEngine::PhysicsEngine(QObject *parent)
    : QObject(parent),
    m_state(ExperimentState::Idle),
    m_elapsedSec(0),
    m_T0(20.0),
    m_isAcceleratedMode(false),
    m_timer(new QTimer(this)),
    m_autoRecordTimer(new QTimer(this)),
    m_pointsCount(0),
    m_startTemp(0.0),
    m_autoRecordIntervalSec(60)
{
    m_previousTemps.resize(4, 0.0);
    m_coolingRates.resize(4, 0.0);
    m_lastRecordedTemps.resize(4, 0.0);
    m_recordedTemps.resize(4);

    connect(m_timer, &QTimer::timeout, this, &PhysicsEngine::onTimerTick);
    connect(m_autoRecordTimer, &QTimer::timeout, this, &PhysicsEngine::onAutoRecordTick);

    m_autoRecordTimer->setInterval(m_autoRecordIntervalSec * 1000);
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

    m_recordedTimes.clear();
    for (int i = 0; i < 4; ++i) {
        m_recordedTemps[i].clear();
    }

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
    emit heatCapacityUpdated(QVector<double>(4, 0.0));

    m_timer->start(1000);
}

void PhysicsEngine::reset()
{
    m_timer->stop();
    m_autoRecordTimer->stop();
    m_state = ExperimentState::Idle;
    m_elapsedSec = 0;
    m_pointsCount = 0;
    m_coolingRates.fill(0.0, 4);

    m_recordedTimes.clear();
    for (int i = 0; i < 4; ++i) {
        m_recordedTemps[i].clear();
    }

    emit stateChanged(m_state);
    emit timeUpdated(0);
    emit pointsCountUpdated(0);
    emit coolingRatesUpdated(m_coolingRates);
    emit progressUpdated(QVector<double>(4, 0.0));
    emit heatCapacityUpdated(QVector<double>(4, 0.0));
    emit autoRecordToggled(false);
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

    m_recordedTimes.append(m_elapsedSec);
    for (int i = 0; i < 4; ++i) {
        m_recordedTemps[i].append(currentTemps[i]);
    }

    calculateHeatCapacities();

    emit pointRecorded(m_pointsCount, m_elapsedSec, currentTemps);
    emit pointsCountUpdated(m_pointsCount);
}

void PhysicsEngine::setAutoRecordEnabled(bool enabled)
{
    if (enabled && m_state == ExperimentState::Cooling) {
        m_autoRecordTimer->start(m_autoRecordIntervalSec * 1000);
    } else {
        m_autoRecordTimer->stop();
    }
    emit autoRecordToggled(enabled);
}

void PhysicsEngine::setAutoRecordInterval(int seconds)
{
    if (seconds < 1) seconds = 1;
    m_autoRecordIntervalSec = seconds;
    int intervalMs = seconds * 1000;
    m_autoRecordTimer->setInterval(intervalMs);

    if (m_autoRecordTimer->isActive()) {
        m_autoRecordTimer->stop();
        m_autoRecordTimer->start(intervalMs);
    }
}

void PhysicsEngine::onAutoRecordTick()
{
    recordPoint();
}

void PhysicsEngine::onTimerTick()
{
    if (m_state != ExperimentState::Cooling) return;

    m_elapsedSec++;
    emit timeUpdated(m_elapsedSec);

    QVector<double> newTemps(4);
    QVector<double> progress(4, 0.0);
    double dt = 1.0;

    for (int i = 0; i < 4; ++i) {
        if (!m_samples[i].isActive) {
            newTemps[i] = m_T0;
            m_coolingRates[i] = 0.0;
            progress[i] = 0.0;
            continue;
        }

        double currentT = m_samples[i].currentTemp;

        double C_sample = m_samples[i].calculateHeatCapacity();
        double C_total = C_K + C_sample;

        double dT_dt = -(K_CALORIMETER_PER_SEC / C_total) * (currentT - m_T0);
        newTemps[i] = currentT + dT_dt * dt;

        m_samples[i].currentTemp = newTemps[i];

        m_coolingRates[i] = (newTemps[i] - m_previousTemps[i]) * 60.0;
        m_previousTemps[i] = newTemps[i];

        if (m_startTemp > m_T0) {
            double totalRange = m_startTemp - m_T0;
            double cooledRange = m_startTemp - newTemps[i];
            progress[i] = qBound(0.0, (cooledRange / totalRange) * 100.0, 100.0);
        }
    }

    emit coolingRatesUpdated(m_coolingRates);
    emit progressUpdated(progress);
    emit temperaturesUpdated(newTemps, m_elapsedSec);

    QVector<double> noisyTemps = addMeasurementNoise(newTemps);
    emit displayTemperaturesUpdated(noisyTemps, m_elapsedSec);
}

void PhysicsEngine::calculateHeatCapacities()
{
    if (m_recordedTimes.size() < 3) {
        emit heatCapacityUpdated(QVector<double>(4, 0.0));
        return;
    }

    QVector<double> heatCapacities(4, 0.0);

    for (int i = 0; i < 4; ++i) {
        if (!m_samples[i].isActive) continue;

        QVector<double> x = m_recordedTimes;
        QVector<double> y;

        for (int j = 0; j < m_recordedTemps[i].size(); ++j) {
            double deltaT = m_recordedTemps[i][j] - m_T0;
            if (deltaT > 0.1) {
                y.append(std::log(deltaT));
            } else {
                y.append(0.0);
            }
        }

        double slope = linearRegressionSlope(x, y);

        if (slope < -0.0001) {
            double C_total = -K_CALORIMETER_PER_SEC / slope;
            double C_sample = C_total - C_K;
            double mass_kg = m_samples[i].massGrams / 1000.0;
            if (mass_kg > 0.001) {
                heatCapacities[i] = C_sample / mass_kg;
            }
        }
    }

    emit heatCapacityUpdated(heatCapacities);
}

double PhysicsEngine::linearRegressionSlope(const QVector<double>& x, const QVector<double>& y)
{
    int n = x.size();
    if (n < 2) return 0.0;

    double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;

    for (int i = 0; i < n; ++i) {
        sumX += x[i];
        sumY += y[i];
        sumXY += x[i] * y[i];
        sumX2 += x[i] * x[i];
    }

    double denominator = n * sumX2 - sumX * sumX;
    if (std::abs(denominator) < 1e-10) return 0.0;

    double slope = (n * sumXY - sumX * sumY) / denominator;
    return slope;
}

QVector<double> PhysicsEngine::addMeasurementNoise(const QVector<double>& temps) const
{
    QVector<double> noisy = temps;
    for (int i = 0; i < 4; ++i) {
        if (m_samples[i].isActive) {
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
}
