#pragma once
#include <QObject>
#include <QTimer>
#include <QVector>
#include "sample.h"

// УПРОЩЕНО: Оставляем только два состояния, так как нагрев и термостатирование убраны
enum class ExperimentState { Idle, Cooling };

class PhysicsEngine : public QObject
{
    Q_OBJECT

public:
    explicit PhysicsEngine(QObject *parent = nullptr);
    void configure(double envTemp, const QVector<Sample>& samples);

    // НОВОЕ: Запуск сразу фазы остывания с заданной начальной температурой
    void startCooling(double initialTemp);

    // НОВОЕ: Установка режима времени (true = ускоренный, false = реальный)
    void setTimeMode(bool isAccelerated);

    void recordPoint();
    void reset();

public slots:
    void setSampleActive(int index, bool active);
    void setDifferentialMode(int index, bool enabled);

signals:
    void displayTemperaturesUpdated(const QVector<double>& noisyTemps, double elapsedSec);
    void temperaturesUpdated(QVector<double> newTemps, double elapsedSec);
    void stateChanged(ExperimentState newState);
    void pointRecorded(int pointIndex, double currentTimeSec, QVector<double> temps);
    void coolingTimeUpdated(double timeSec);
    void timeUpdated(int totalSeconds);
    void pointsCountUpdated(int count);
    void tableReset();

private slots:
    void onTimerTick();
    void onDisplayUpdateTick();
    double generateNoise(double trueTemp);

private:
    QTimer* m_timer;
    double m_elapsedSec;
    double m_coolingStartSec;
    ExperimentState m_state;
    double m_envTemp;
    QVector<Sample> m_samples;
    int m_pointCount;
    double m_coolingTimeSec;
    bool m_isDifferentialMode[4];

    // НОВОЕ: Флаг режима времени
    bool m_isAcceleratedMode;

    static constexpr double K_CONST = 5.20;  // Дж/(мин·К)
    static constexpr double C_K = 210.0;     // Дж/К
    double m_T1_CoolingStart = 60.0;

    QTimer *m_displayTimer;
    QVector<double> m_lastNoisyTemps;
};
