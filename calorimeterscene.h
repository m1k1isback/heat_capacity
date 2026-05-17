#pragma once

// Forward declarations (используются только как указатели — ускоряет компиляцию)
class QCheckBox;
class QGraphicsEllipseItem;
class QGraphicsTextItem;
class QGraphicsPathItem;

#include <QGraphicsScene>
#include <QVector>
#include "DigitalDisplay.h"

class CalorimeterScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit CalorimeterScene(QObject *parent = nullptr);
    ~CalorimeterScene() override = default;

    void buildLayout();
    void setEnvironmentTemperature(double t0);

signals:
    void sampleVisibilityChanged(int index, bool visible);
    void sampleActiveChanged(int index, bool active);
    void sampleIsChanged(int index, bool active);
    void differentialModeChanged(int index, bool enabled);
    void tableHeaderChanged(int index, bool isDifferential);

public slots:
    // Слоты для приёма данных от физического движка и UI
    void updateTemperatures(double t1, double t2, double t3, double t4, double t0);
    void checkSample(int a, bool checked);
    void onPhysicsTemperaturesUpdated(const QVector<double>& temps, int elapsedSec);
    void onDifferentialModeToggled(int sampleIndex, bool enabled);
    void updateDisplayForSample(int sampleIndex, double T_sample, double T0);
    void setSampleActive(int index, bool active);

private:
    // Массив указателей на дисплеи: [0]-T1, [1]-T2, [2]-T3, [3]-T4, [4]-T0
    DigitalDisplay *displays[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};

    // Состояния и чекбоксы образцов
    bool isActive[4] = {false, false, false, false};
    QCheckBox* checkBoxes[4] = {nullptr, nullptr, nullptr, nullptr};

    // Графические элементы образцов
    QGraphicsEllipseItem* m_sampleCircles[4] = {nullptr, nullptr, nullptr, nullptr};
    QGraphicsTextItem* m_sampleCrosses[4] = {nullptr, nullptr, nullptr, nullptr};
    QGraphicsPathItem* m_sampleWires[4] = {nullptr, nullptr, nullptr, nullptr};

    // Режим дифференциального показа
    QCheckBox* m_differentialModeCheckBox[4] = {nullptr, nullptr, nullptr, nullptr};
    bool m_isDifferentialMode[4] = {false, false, false, false};

    // Температуры и кэш
    double m_T0 = 20.0;
    double m_lastSampleTemps[4] = {0.0, 0.0, 0.0, 0.0};  // T1, T2, T3, T4
    double m_lastT0 = 20.0;
};
