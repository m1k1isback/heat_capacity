#pragma once

#include <QObject>
#include <QVector>
#include <QTimer>
#include "sample.h"

enum class ExperimentState {
    Idle,
    Cooling
};

class PhysicsEngine : public QObject
{
    Q_OBJECT

public:
    explicit PhysicsEngine(QObject *parent = nullptr);
    ~PhysicsEngine() override = default;

    void configure(double t0, const QVector<Sample>& samples);
    void setTimeMode(bool isAccelerated);
    void startCooling(double initialTemp);
    void reset();
    void recordPoint();

    QVector<Sample> getSamples() const { return m_samples; }
    double getT0() const { return m_T0; }

    void setSampleActive(int index, bool active);
    void setDifferentialMode(int index, bool enabled);

signals:
    void stateChanged(ExperimentState state);
    void timeUpdated(int seconds);
    void coolingTimeUpdated(double timeSec);
    void pointsCountUpdated(int count);
    void temperaturesUpdated(const QVector<double>& temps, int elapsedSec);
    void displayTemperaturesUpdated(const QVector<double>& noisyTemps, int elapsedSec);
    void pointRecorded(int id, double time, const QVector<double>& temps);
    void tableReset();

    // Новые сигналы для панели статуса
    void samplesConfigured(const QVector<Sample>& samples);
    void coolingRatesUpdated(const QVector<double>& rates);
    void progressUpdated(const QVector<double>& progress);

private slots:
    void onTimerTick();

private:
    QVector<double> addMeasurementNoise(const QVector<double>& temps) const;

    ExperimentState m_state;
    int m_elapsedSec;
    double m_T0;
    bool m_isAcceleratedMode;
    QVector<Sample> m_samples;
    QTimer* m_timer;

    int m_pointsCount;
    QVector<double> m_lastRecordedTemps;

    // Переменные для расчета скорости и прогресса
    QVector<double> m_previousTemps;
    QVector<double> m_coolingRates;
    double m_startTemp;
};
