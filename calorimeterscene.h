#ifndef CALORIMETERSCENE_H
#define CALORIMETERSCENE_H

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include "DigitalDisplay.h"
#include <QCheckBox>

class CalorimeterScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit CalorimeterScene(QObject *parent = nullptr);
    void buildLayout();
    void setEnvironmentTemperature(double t0);

public slots:
    // Этот слот будет принимать данные от физического движка
    void updateTemperatures(double t1, double t2, double t3, double t4, double t0);
    void checkSample(int a, bool checked);
    void onPhysicsTemperaturesUpdated(const QVector<double>& temps, int elapsedSec);

private:
    // Массив указателей на дисплеи: [0]-T1, [1]-T2, [2]-T3, [3]-T4, [4]-T0
    DigitalDisplay *displays[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};

    bool isActive[4];
    QCheckBox* checkBoxes[4];
    QGraphicsEllipseItem* m_sampleCircles[4];
    QGraphicsTextItem* m_sampleCrosses[4];
    QGraphicsPathItem* m_sampleWires[4];

    QCheckBox* m_differentialModeCheckBox[4];
    bool m_isDifferentialMode[4];
    double m_T0;
    double m_lastSampleTemps[4];  // T1, T2, T3, T4
    double m_lastT0;
public slots:
    void onDifferentialModeToggled(int sampleIndex, bool enabled);
    void updateDisplayForSample(int sampleIndex, double T_sample, double T0);
    void setSampleActive(int index, bool active);
signals:
    void sampleVisibilityChanged(int index, bool visible);
    void sampleActiveChanged(int index, bool active);
    void sampleIsChanged(int index, bool active);
    void differentialModeChanged(int index, bool enabled);
    void tableHeaderChanged(int index, bool isDifferential);
};

#endif // CALORIMETERSCENE_H
