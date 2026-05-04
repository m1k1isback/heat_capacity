#pragma once
#include <QObject>
#include <QTimer>
#include <QVector>
#include "sample.h"

enum class ExperimentState {Idle, Heating, Thermostate, Cooling};

class PhysicsEngine : public QObject
{
    Q_OBJECT

public:
    explicit PhysicsEngine(QObject *parent = nullptr);
    void configure(double envTemp, const QVector<Sample>& samples);
    void startHeating();
    void startThermostatting();
    void recordPoint();
    void reset();
signals:
    void temperaturesUpdated(QVector<double> newTemps, int elapsedSec);
    void stateChanged(ExperimentState newState);
    void pointRecorded(int pointIndex, double currentTimeSec, QVector<double> temps);
private slots:
    void onTimerTick();
private:
    QTimer* m_timer;
    int m_elapsedSec; //время эксперимента
    int m_coolingStartSec; //секунда, когда началось остывание
    ExperimentState m_state; // текущая фаза
    double m_envTemp; // температура среды
    QVector<Sample> m_samples; //копия массива образцов с их текущими температурами.
    int m_pointCount; // сколько записанных точек

    static constexpr double K_CONST = 5.20;  // Дж/(мин·К)
    static constexpr double C_K = 210.0;     // Дж/К
};
