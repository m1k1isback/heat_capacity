#include "CalorimeterScene.h"
#include <QPen>
#include <QBrush>
#include <QGraphicsTextItem>
#include <QFont>
#include <QCheckBox>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <QDebug>
#include <QColor>
#include <QPainter>

CalorimeterScene::CalorimeterScene(QObject *parent)
    : QGraphicsScene(parent)
{
    m_lastT0 = 20.0;
    for(int i = 0; i < 4; ++i) {
        m_lastSampleTemps[i] = 0.0;
        m_startTemps[i] = 0.0;
    }

    buildLayout();
    updateTemperatures(20.0, 20.0, 20.0, 20.0, 20.0);
}

void CalorimeterScene::buildLayout()
{
    setSceneRect(0, 0, 1000, 800);
    setBackgroundBrush(Qt::transparent);

    // Корпус калориметра
    QGraphicsRectItem *body = new QGraphicsRectItem(150+250, 125, 550, 550);
    body->setBrush(QBrush(QColor("#f5f5f5")));
    body->setPen(QPen(QColor("#a0a0a0"), 2));
    body->setZValue(0);
    addItem(body);

    // Пунктир теплоизоляции
    QGraphicsRectItem *insulation = new QGraphicsRectItem(170+250, 145, 510, 510);
    insulation->setBrush(Qt::NoBrush);
    QPen dashPen(QColor("#bdc3c7"), 1, Qt::DashLine);
    insulation->setPen(dashPen);
    insulation->setZValue(0);
    addItem(insulation);

    // 4 образца
    QPointF positions[] = {
        {320+250+15, 250}, {520+250, 250},  // Верхний ряд
        {320+250+15, 500}, {520+250, 500}   // Нижний ряд
    };

    for (int i = 0; i < 4; ++i) {
        // Тело образца
        QGraphicsEllipseItem *sample = new QGraphicsEllipseItem(
            positions[i].x() - 50, positions[i].y() - 50, 100, 100
            );
        // Изначально серый цвет без штриховки
        sample->setBrush(QBrush(QColor("#bdc3c7"), Qt::NoBrush));
        sample->setPen(QPen(QColor("#7f8c8d"), 2));
        sample->setZValue(1);
        addItem(sample);

        m_sampleCircles[i] = sample;

        // Крест если образец выключен
        QGraphicsTextItem* cross = new QGraphicsTextItem("✕");
        cross->setDefaultTextColor(QColor("#FF0000"));
        cross->setFont(QFont("Arial", 50, QFont::Bold));

        qreal textWidth = cross->boundingRect().width();
        qreal textHeight = cross->boundingRect().height();
        qreal centerX = positions[i].x() - textWidth / 2;
        qreal centerY = positions[i].y() - textHeight / 2;
        cross->setPos(centerX, centerY);
        cross->setVisible(false);
        cross->setZValue(2);
        addItem(cross);

        m_sampleCrosses[i] = cross;
    }

    // Чекбоксы
    for(int i = 0; i < 4; i++){
        QCheckBox *cb = new QCheckBox();
        cb->setText("Обр. " + QString::number(i + 1));
        cb->setChecked(false);
        cb->setEnabled(false);
        checkBoxes[i] = cb;
        cb->setChecked(true);

        QGraphicsProxyWidget* proxy = new QGraphicsProxyWidget();
        proxy->setWidget(cb);
        addItem(proxy);
        if(i % 2 == 0){
            proxy->setPos(positions[i].x() - 110, positions[i].y() - 50);
        } else {
            proxy->setPos(positions[i].x() + 40, positions[i].y() - 50);
        }
        connect(checkBoxes[i], &QCheckBox::toggled, this, [this, i]() {
            this->checkSample(i, checkBoxes[i]->isChecked());
        });
    }

    for(int i = 0; i < 4; i++){
        isActive[i] = false;
        m_sampleCrosses[i]->setVisible(true);
    }

    for(int i = 0; i < 4; i++) {
        m_isDifferentialMode[i] = false;
        QCheckBox* diffCheckBox = new QCheckBox("ΔT");
        diffCheckBox->setToolTip("Дифференциальный тип (показывать ΔT = T - T0)");

        QGraphicsProxyWidget* diffProxy = new QGraphicsProxyWidget();
        diffProxy->setWidget(diffCheckBox);

        if(i % 2 == 0){
            diffProxy->setPos(positions[i].x() - 155, positions[i].y() - 50);
        } else {
            diffProxy->setPos(positions[i].x() + 110, positions[i].y() - 50);
        }
        addItem(diffProxy);
        m_differentialModeCheckBox[i] = diffCheckBox;

        connect(diffCheckBox, &QCheckBox::toggled,
                this, [this, i](bool checked) {
                    this->onDifferentialModeToggled(i, checked);
                });
    }

    // 4 дисплея
    displays[0] = new DigitalDisplay("1"); displays[0]->setPos(180+250, 230); addItem(displays[0]);
    displays[1] = new DigitalDisplay("2"); displays[1]->setPos(580+250, 230); addItem(displays[1]);
    displays[2] = new DigitalDisplay("3"); displays[2]->setPos(180+250, 480); addItem(displays[2]);
    displays[3] = new DigitalDisplay("4"); displays[3]->setPos(580+250, 480); addItem(displays[3]);

    QPointF displayCenters[] = {
        QPointF(220+250+10, 280), QPointF(630+250, 280),
        QPointF(220+250+10, 530), QPointF(630+250, 530)
    };

    for(int i = 0; i < 4; i++) {
        QPointF start = positions[i];
        QPointF end = displayCenters[i];

        QPainterPath path;
        path.moveTo(start.x(), start.y());
        qreal midX = ((start.x() + end.x()) / 2);
        qreal midY = ((start.y() + end.y()) / 2);
        path.quadTo(midX + 15, midY + 40, end.x(), end.y());

        QGraphicsPathItem* wire = new QGraphicsPathItem(path);
        QPen wirePen;
        wirePen.setColor(Qt::black);
        wirePen.setWidth(2);
        wirePen.setStyle(Qt::SolidLine);
        wire->setPen(wirePen);
        wire->setBrush(Qt::NoBrush);
        wire->setZValue(10);
        addItem(wire);
        m_sampleWires[i] = wire;
    }

    for(int i = 0; i < 4; i++){
        isActive[i] = true;
    }

    // === Разрез калориметра (без изменений, как было) ===
    QGraphicsItemGroup *crossSectionGroup = new QGraphicsItemGroup();

    QGraphicsRectItem *sampleSide = new QGraphicsRectItem(100, 300, 150, 200);
    sampleSide->setBrush(QBrush(QColor("#bdc3c7")));
    sampleSide->setPen(QPen(QColor("#7f8c8d"), 2));
    crossSectionGroup->addToGroup(sampleSide);

    QGraphicsRectItem *heater = new QGraphicsRectItem(100, 500, 150, 25);
    heater->setBrush(QBrush(QColor("#e74c3c")));
    heater->setPen(QPen(QColor("#c0392b"), 2));
    crossSectionGroup->addToGroup(heater);

    QGraphicsRectItem *wall_left = new QGraphicsRectItem(60, 300, 40, 225);
    QGraphicsRectItem *wall_right = new QGraphicsRectItem(250, 300, 40, 225);
    QGraphicsRectItem *wall_top = new QGraphicsRectItem(60, 270, 230, 30);
    QGraphicsRectItem *wall_bot = new QGraphicsRectItem(60, 525, 230, 30);

    wall_left->setBrush(QBrush(Qt::lightGray, Qt::BDiagPattern));
    wall_right->setBrush(QBrush(Qt::lightGray, Qt::BDiagPattern));
    wall_top->setBrush(QBrush(Qt::lightGray, Qt::BDiagPattern));
    wall_bot->setBrush(QBrush(Qt::lightGray, Qt::BDiagPattern));

    crossSectionGroup->addToGroup(wall_left);
    crossSectionGroup->addToGroup(wall_right);
    crossSectionGroup->addToGroup(wall_top);
    crossSectionGroup->addToGroup(wall_bot);

    QGraphicsRectItem *port = new QGraphicsRectItem(170, 270, 11, 130);
    port->setBrush(QBrush(Qt::white, Qt::Dense6Pattern));
    crossSectionGroup->addToGroup(port);

    QGraphicsRectItem *thermopara = new QGraphicsRectItem(172, 370, 7, 20);
    thermopara->setBrush(QBrush(Qt::black));
    crossSectionGroup->addToGroup(thermopara);

    QPainterPath wirePath;
    wirePath.moveTo(175, 370);
    wirePath.lineTo(175, 240);
    wirePath.cubicTo(175, 210, 120, 180, 60, 215);

    QGraphicsPathItem *wire = new QGraphicsPathItem(wirePath);
    wire->setPen(QPen(Qt::black, 2));
    crossSectionGroup->addToGroup(wire);

    DigitalDisplay *crossDisplay = new DigitalDisplay("");
    crossDisplay->setPos(10, 180);
    crossSectionGroup->addToGroup(crossDisplay);

    // Обозначения
    QGraphicsTextItem *msg_1 = new QGraphicsTextItem("1"); msg_1->setPos(100, 150); msg_1->setFont(QFont("Arial", 12, QFont::Bold)); msg_1->setDefaultTextColor(Qt::black); crossSectionGroup->addToGroup(msg_1);
    QGraphicsTextItem *msg_2 = new QGraphicsTextItem("2"); msg_2->setPos(330, 310); msg_2->setFont(QFont("Arial", 12, QFont::Bold)); msg_2->setDefaultTextColor(Qt::black); crossSectionGroup->addToGroup(msg_2);
    QGraphicsTextItem *msg_3 = new QGraphicsTextItem("3"); msg_3->setPos(330, 400); msg_3->setFont(QFont("Arial", 12, QFont::Bold)); msg_3->setDefaultTextColor(Qt::black); crossSectionGroup->addToGroup(msg_3);
    QGraphicsTextItem *msg_4 = new QGraphicsTextItem("4"); msg_4->setPos(20, 500); msg_4->setFont(QFont("Arial", 12, QFont::Bold)); msg_4->setDefaultTextColor(Qt::black); crossSectionGroup->addToGroup(msg_4);
    QGraphicsTextItem *msg_5 = new QGraphicsTextItem("5"); msg_5->setPos(20, 300); msg_5->setFont(QFont("Arial", 12, QFont::Bold)); msg_5->setDefaultTextColor(Qt::black); crossSectionGroup->addToGroup(msg_5);
    QGraphicsTextItem *msg_6 = new QGraphicsTextItem("6"); msg_6->setPos(20, 250); msg_6->setFont(QFont("Arial", 12, QFont::Bold)); msg_6->setDefaultTextColor(Qt::black); crossSectionGroup->addToGroup(msg_6);

    QGraphicsLineItem *ptr1 = new QGraphicsLineItem(80, 180, 103, 170); ptr1->setPen(QPen(Qt::black, 1)); ptr1->setZValue(10); crossSectionGroup->addToGroup(ptr1);
    QGraphicsLineItem *ptr2 = new QGraphicsLineItem(174, 375, 333, 323); ptr2->setPen(QPen(Qt::black, 1)); ptr2->setZValue(10); crossSectionGroup->addToGroup(ptr2);
    QGraphicsLineItem *ptr3_1 = new QGraphicsLineItem(260, 400, 329, 410); ptr3_1->setPen(QPen(Qt::black, 1)); ptr3_1->setZValue(10); crossSectionGroup->addToGroup(ptr3_1);
    QGraphicsLineItem *ptr3_2 = new QGraphicsLineItem(270, 540, 333, 420); ptr3_2->setPen(QPen(Qt::black, 1)); ptr3_2->setZValue(10); crossSectionGroup->addToGroup(ptr3_2);
    QGraphicsLineItem *ptr4 = new QGraphicsLineItem(35, 513, 130, 515); ptr4->setPen(QPen(Qt::black, 1)); ptr4->setZValue(10); crossSectionGroup->addToGroup(ptr4);
    QGraphicsLineItem *ptr5 = new QGraphicsLineItem(35, 314, 130, 340); ptr5->setPen(QPen(Qt::black, 1)); ptr5->setZValue(10); crossSectionGroup->addToGroup(ptr5);
    QGraphicsLineItem *ptr6 = new QGraphicsLineItem(35, 263, 130, 280); ptr6->setPen(QPen(Qt::black, 1)); ptr6->setZValue(10); crossSectionGroup->addToGroup(ptr6);

    QGraphicsTextItem *lg1 = new QGraphicsTextItem("1 - дисплей термометра"); lg1->setPos(30, 580); lg1->setFont(QFont("Arial", 11, QFont::Bold)); lg1->setDefaultTextColor(Qt::black); crossSectionGroup->addToGroup(lg1);
    QGraphicsTextItem *lg2 = new QGraphicsTextItem("2 - датчик термометра"); lg2->setPos(30, 600); lg2->setFont(QFont("Arial", 11, QFont::Bold)); lg2->setDefaultTextColor(Qt::black); crossSectionGroup->addToGroup(lg2);
    QGraphicsTextItem *lg3 = new QGraphicsTextItem("3 - теплоизоляция"); lg3->setPos(30, 620); lg3->setFont(QFont("Arial", 11, QFont::Bold)); lg3->setDefaultTextColor(Qt::black); crossSectionGroup->addToGroup(lg3);
    QGraphicsTextItem *lg4 = new QGraphicsTextItem("4 - нагревательный элемент"); lg4->setPos(30, 640); lg4->setFont(QFont("Arial", 11, QFont::Bold)); lg4->setDefaultTextColor(Qt::black); crossSectionGroup->addToGroup(lg4);
    QGraphicsTextItem *lg5 = new QGraphicsTextItem("5 - образец"); lg5->setPos(30, 660); lg5->setFont(QFont("Arial", 11, QFont::Bold)); lg5->setDefaultTextColor(Qt::black); crossSectionGroup->addToGroup(lg5);
    QGraphicsTextItem *lg6 = new QGraphicsTextItem("6 - крышка калориметра"); lg6->setPos(30, 680); lg6->setFont(QFont("Arial", 11, QFont::Bold)); lg6->setDefaultTextColor(Qt::black); crossSectionGroup->addToGroup(lg6);

    addItem(crossSectionGroup);
}

// =====================================================================
// НОВАЯ ЛОГИКА: Визуализация (Цвет + Штриховка)
// =====================================================================

void CalorimeterScene::setCoolingStartTemperature(double startTemp)
{
    // Устанавливаем одинаковую начальную температуру для всех активных образцов при старте
    for(int i = 0; i < 4; i++) {
        if(isActive[i]) {
            m_startTemps[i] = startTemp;
        }
    }
}

void CalorimeterScene::setSampleMaterial(int index, const QString& materialName)
{
    if (index < 0 || index >= 4) return;
    m_currentMaterials[index] = materialName;

    // Пересчитываем визуализацию, чтобы сразу применить новую штриховку
    updateSampleVisuals(index, m_lastSampleTemps[index]);
}

void CalorimeterScene::updateSampleVisuals(int index, double currentTemp)
{
    if (!isActive[index] || !m_sampleCircles[index]) return;

    // Определяем базовый цвет
    QColor baseColor;

    if (m_startTemps[index] >= 1.0) {
        double tempRange = m_startTemps[index] - m_lastT0;
        double ratio = 1.0;
        if (tempRange > 0.1) {
            ratio = (currentTemp - m_lastT0) / tempRange;
            ratio = qBound(0.0, ratio, 1.0);
        }

        QColor hotColor(255, 69, 0);
        QColor coldColor(189, 195, 199);
        baseColor.setRedF(coldColor.redF() + ratio * (hotColor.redF() - coldColor.redF()));
        baseColor.setGreenF(coldColor.greenF() + ratio * (hotColor.greenF() - coldColor.greenF()));
        baseColor.setBlueF(coldColor.blueF() + ratio * (hotColor.blueF() - coldColor.blueF()));
    } else {
        baseColor = QColor(189, 195, 199);
    }

    // Создаём текстуру БОЛЬШЕГО размера для лучшей видимости
    QPixmap texture(32, 32);
    texture.fill(baseColor);

    if (!m_currentMaterials[index].isEmpty() && m_currentMaterials[index] != "Выберите") {
        uint hash = qHash(m_currentMaterials[index]);
        int patternType = hash % 8;  // Увеличили до 8 типов паттернов

        QPainter painter(&texture);
        painter.setPen(QPen(QColor(60, 60, 60), 2));  // Тёмные линии толщиной 2

        if (patternType == 0) {
            // Горизонтальные линии (шаг 8)
            for (int y = 0; y < 32; y += 8)
                painter.drawLine(0, y, 32, y);
        } else if (patternType == 1) {
            // Вертикальные линии (шаг 8)
            for (int x = 0; x < 32; x += 8)
                painter.drawLine(x, 0, x, 32);
        } else if (patternType == 2) {
            // Диагональ \ (шаг 8)
            for (int i = -31; i < 32; i += 8)
                painter.drawLine(qMax(0, i), qMax(0, -i), qMin(32, 32+i), qMin(32, 32-i));
        } else if (patternType == 3) {
            // Диагональ / (шаг 8)
            for (int i = 0; i < 64; i += 8)
                painter.drawLine(qMax(0, i-31), qMin(31, i), qMin(31, i), qMax(0, i-31));
        } else if (patternType == 4) {
            // Крест + (горизонтальные + вертикальные)
            for (int y = 0; y < 32; y += 16)
                painter.drawLine(0, y, 32, y);
            for (int x = 0; x < 32; x += 16)
                painter.drawLine(x, 0, x, 32);
        } else if (patternType == 5) {
            // Диагональный крест X
            for (int i = -31; i < 32; i += 16)
                painter.drawLine(qMax(0, i), qMax(0, -i), qMin(32, 32+i), qMin(32, 32-i));
            for (int i = 0; i < 64; i += 16)
                painter.drawLine(qMax(0, i-31), qMin(31, i), qMin(31, i), qMax(0, i-31));
        } else if (patternType == 6) {
            // Точки
            for (int y = 0; y < 32; y += 8)
                for (int x = 0; x < 32; x += 8)
                    painter.drawPoint(x, y);
        } else {
            // Клетка (горизонтальные + вертикальные с разным шагом)
            for (int y = 0; y < 32; y += 8)
                painter.drawLine(0, y, 32, y);
            for (int x = 0; x < 32; x += 16)
                painter.drawLine(x, 0, x, 32);
        }
        painter.end();
    }

    QBrush textureBrush(texture);
    m_sampleCircles[index]->setBrush(textureBrush);

    if (m_startTemps[index] >= 1.0 && currentTemp > 500)
        m_sampleCircles[index]->setPen(QPen(Qt::yellow, 3));
    else
        m_sampleCircles[index]->setPen(QPen(QColor("#7f8c8d"), 2));
}

// =====================================================================
// СТАРАЯ ЛОГИКА (с сохраненными комментариями)
// =====================================================================

void CalorimeterScene::updateTemperatures(double t1, double t2, double t3, double t4, double t0)
{
    m_lastSampleTemps[0] = t1;
    m_lastSampleTemps[1] = t2;
    m_lastSampleTemps[2] = t3;
    m_lastSampleTemps[3] = t4;
    m_lastT0 = t0;

    double temps[] = {t1, t2, t3, t4, t0};

    for(int i = 0; i < 4; i++) {
        if (!displays[i]) continue;

        if(!isActive[i]) {
            displays[i]->setValue(0.0);
            continue;
        }

        if(m_isDifferentialMode[i]) {
            double deltaT = temps[i] - t0;
            displays[i]->setValue(deltaT);
            displays[i]->setPrefix("ΔT");
            displays[i]->setTextColor(Qt::red);
        } else {
            displays[i]->setValue(temps[i]);
            displays[i]->setPrefix("T");
            displays[i]->setTextColor(QColor("#00ff00"));
        }
    }

    if (displays[4]) {
        displays[4]->setValue(t0);
    }
}

void CalorimeterScene::checkSample(int index, bool checked){
    if(index < 0 || index >= 4) return;
    isActive[index] = checked;
    m_sampleCrosses[index]->setVisible(!checked);

    emit sampleVisibilityChanged(index, checked);
    emit sampleActiveChanged(index, checked);
    emit sampleIsChanged(index, checked);
}

void CalorimeterScene::onDifferentialModeToggled(int sampleIndex, bool enabled){
    m_isDifferentialMode[sampleIndex] = enabled;
    if (sampleIndex >= 0 && sampleIndex < 4) {
        updateDisplayForSample(sampleIndex, m_lastSampleTemps[sampleIndex], m_lastT0);
    }
    emit differentialModeChanged(sampleIndex, enabled);
    emit tableHeaderChanged(sampleIndex, enabled);
}

void CalorimeterScene::updateDisplayForSample(int sampleIndex, double T_sample, double T0){
    if (sampleIndex < 0 || sampleIndex >= 4) return;
    if (!isActive[sampleIndex]) {
        displays[sampleIndex]->setValue(0.0);
        return;
    }

    if (m_isDifferentialMode[sampleIndex]) {
        double deltaT = T_sample - T0;
        displays[sampleIndex]->setValue(deltaT);
        displays[sampleIndex]->setPrefix("ΔT");
        displays[sampleIndex]->setTextColor(Qt::red);
    } else {
        displays[sampleIndex]->setValue(T_sample);
        displays[sampleIndex]->setPrefix("T");
        displays[sampleIndex]->setTextColor(Qt::green);
    }
}

void CalorimeterScene::setEnvironmentTemperature(double t0)
{
    m_lastT0 = t0;
}

// НОВОЕ: Используем "гладкие" температуры от движка именно для анимации цвета,
// чтобы цвет не мерцал из-за шума, который предназначен только для дисплеев.
void CalorimeterScene::onPhysicsTemperaturesUpdated(const QVector<double>& temps, int elapsedSec)
{
    Q_UNUSED(elapsedSec);
    if (temps.size() >= 4) {
        for(int i = 0; i < 4; i++) {
            if(isActive[i]) {
                updateSampleVisuals(i, temps[i]);
            }
        }
    }
}

void CalorimeterScene::setSampleActive(int index, bool active)
{
    if (index < 0 || index >= 4) return;
    QSignalBlocker blocker(checkBoxes[index]);
    checkBoxes[index]->setChecked(active);
    checkBoxes[index]->setEnabled(active);
    checkSample(index, active);
}

void CalorimeterScene::onDisplayTemperaturesUpdated(const QVector<double>& noisyTemps, int elapsedSec)
{
    Q_UNUSED(elapsedSec);
    if (noisyTemps.size() >= 4) {
        updateTemperatures(noisyTemps[0], noisyTemps[1], noisyTemps[2], noisyTemps[3], m_lastT0);
    }
}
