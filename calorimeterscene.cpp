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
#include <QPainterPath>

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

    // =========================================================================
    // ЛЕВАЯ ЧАСТЬ: ИЗОМЕТРИЧЕСКИЙ РАЗРЕЗ КАЛОРИМЕТРА
    // =========================================================================

    const double isoCX = 220;
    const double isoCY = 440;

    // ИСПРАВЛЕНИЕ 4: надпись над установкой
    QGraphicsTextItem *calorimeterTitle = new QGraphicsTextItem("Вакуумный калориметр");
    calorimeterTitle->setPos(isoCX - 90, 120);
    calorimeterTitle->setFont(QFont("Arial", 14, QFont::Bold));
    calorimeterTitle->setDefaultTextColor(QColor("#2c3e50"));
    calorimeterTitle->setZValue(11);
    addItem(calorimeterTitle);

    // Параметры внешнего цилиндра (корпус)
    const double outerRX = 140;
    const double outerRY = 70;
    const double outerH  = 300;

    // Параметры внутреннего цилиндра (вакуумная камера)
    const double innerRX = 105;
    const double innerRY = 52;
    const double innerH  = 250;

    const double topY = isoCY - outerH / 2;
    const double botY = isoCY + outerH / 2;
    const double innerTopY = isoCY - innerH / 2;
    const double innerBotY = isoCY + innerH / 2;

    // --- Внешний корпус (задняя половина) ---
    QPainterPath outerTopArc;
    outerTopArc.moveTo(isoCX - outerRX, topY);
    outerTopArc.arcTo(isoCX - outerRX, topY - outerRY, outerRX * 2, outerRY * 2, 180, -180);
    QGraphicsPathItem *outerTop = new QGraphicsPathItem(outerTopArc);
    outerTop->setPen(QPen(QColor("#7f8c8d"), 2));
    outerTop->setZValue(1);
    outerTop->setToolTip("Корпус калориметра (сталь)");
    addItem(outerTop);

    QGraphicsLineItem *outerLeft = new QGraphicsLineItem(isoCX - outerRX, topY, isoCX - outerRX, botY);
    outerLeft->setPen(QPen(QColor("#7f8c8d"), 2));
    outerLeft->setZValue(1);
    addItem(outerLeft);

    QGraphicsLineItem *outerRight = new QGraphicsLineItem(isoCX + outerRX, topY, isoCX + outerRX, botY);
    outerRight->setPen(QPen(QColor("#7f8c8d"), 2));
    outerRight->setZValue(1);
    addItem(outerRight);

    QPainterPath outerBotArc;
    outerBotArc.moveTo(isoCX - outerRX, botY);
    outerBotArc.arcTo(isoCX - outerRX, botY - outerRY, outerRX * 2, outerRY * 2, 180, -180);
    QGraphicsPathItem *outerBot = new QGraphicsPathItem(outerBotArc);
    outerBot->setPen(QPen(QColor("#7f8c8d"), 2));
    outerBot->setZValue(1);
    addItem(outerBot);

    QPainterPath outerBotFrontArc;
    outerBotFrontArc.moveTo(isoCX - outerRX, botY);
    outerBotFrontArc.arcTo(isoCX - outerRX, botY - outerRY, outerRX * 2, outerRY * 2, 180, 180);
    QGraphicsPathItem *outerBotFront = new QGraphicsPathItem(outerBotFrontArc);
    outerBotFront->setPen(QPen(QColor("#7f8c8d"), 2));
    outerBotFront->setZValue(1);
    addItem(outerBotFront);

    // --- Теплоизоляция ---
    QPainterPath isoTopArc;
    isoTopArc.moveTo(isoCX - outerRX, topY);
    isoTopArc.arcTo(isoCX - outerRX, topY - outerRY, outerRX * 2, outerRY * 2, 180, -180);
    isoTopArc.lineTo(isoCX - innerRX, innerTopY);
    isoTopArc.arcTo(isoCX - innerRX, innerTopY - innerRY, innerRX * 2, innerRY * 2, 0, 180);
    isoTopArc.closeSubpath();
    QGraphicsPathItem *isoTop = new QGraphicsPathItem(isoTopArc);
    isoTop->setBrush(QBrush(Qt::lightGray, Qt::BDiagPattern));
    isoTop->setPen(QPen(QColor("#95a5a6"), 1));
    isoTop->setZValue(2);
    isoTop->setToolTip("Теплоизоляция (минеральная вата)");
    addItem(isoTop);

    QGraphicsRectItem *isoLeft = new QGraphicsRectItem(isoCX - outerRX, topY, outerRX - innerRX, innerH);
    isoLeft->setBrush(QBrush(Qt::lightGray, Qt::BDiagPattern));
    isoLeft->setPen(QPen(QColor("#95a5a6"), 1));
    isoLeft->setZValue(2);
    isoLeft->setToolTip("Теплоизоляция (минеральная вата)");
    addItem(isoLeft);

    QGraphicsRectItem *isoRight = new QGraphicsRectItem(isoCX + innerRX, topY, outerRX - innerRX, innerH);
    isoRight->setBrush(QBrush(Qt::lightGray, Qt::BDiagPattern));
    isoRight->setPen(QPen(QColor("#95a5a6"), 1));
    isoRight->setZValue(2);
    isoRight->setToolTip("Теплоизоляция (минеральная вата)");
    addItem(isoRight);

    // --- Внутренняя вакуумная камера ---
    QPainterPath innerTopArc;
    innerTopArc.moveTo(isoCX - innerRX, innerTopY);
    innerTopArc.arcTo(isoCX - innerRX, innerTopY - innerRY, innerRX * 2, innerRY * 2, 180, -180);
    QGraphicsPathItem *innerTop = new QGraphicsPathItem(innerTopArc);
    innerTop->setPen(QPen(QColor("#34495e"), 2));
    innerTop->setZValue(3);
    addItem(innerTop);

    QGraphicsLineItem *innerLeft = new QGraphicsLineItem(isoCX - innerRX, innerTopY, isoCX - innerRX, innerBotY);
    innerLeft->setPen(QPen(QColor("#34495e"), 2));
    innerLeft->setZValue(3);
    addItem(innerLeft);

    QGraphicsLineItem *innerRight = new QGraphicsLineItem(isoCX + innerRX, innerTopY, isoCX + innerRX, innerBotY);
    innerRight->setPen(QPen(QColor("#34495e"), 2));
    innerRight->setZValue(3);
    addItem(innerRight);

    QGraphicsEllipseItem *innerBot = new QGraphicsEllipseItem(
        isoCX - innerRX, innerBotY - innerRY, innerRX * 2, innerRY * 2);
    innerBot->setBrush(QBrush(QColor("#ecf0f1")));
    innerBot->setPen(QPen(QColor("#34495e"), 2));
    innerBot->setZValue(3);
    addItem(innerBot);

    // --- Нагревательный элемент ---
    const double heaterRX = 90;
    const double heaterRY = 45;
    const double heaterY = innerBotY - 30;

    QGraphicsEllipseItem *heater = new QGraphicsEllipseItem(
        isoCX - heaterRX, heaterY - heaterRY, heaterRX * 2, heaterRY * 2);
    heater->setBrush(QBrush(QColor("#e74c3c")));
    heater->setPen(QPen(QColor("#c0392b"), 2));
    heater->setZValue(4);
    heater->setToolTip("Нагревательный элемент (нихром)");
    addItem(heater);

    QPainterPath heaterBody;
    heaterBody.moveTo(isoCX - heaterRX, heaterY);
    heaterBody.lineTo(isoCX - heaterRX, heaterY + 15);
    heaterBody.arcTo(isoCX - heaterRX, heaterY + 15 - heaterRY, heaterRX * 2, heaterRY * 2, 180, 180);
    heaterBody.lineTo(isoCX + heaterRX, heaterY);
    heaterBody.arcTo(isoCX - heaterRX, heaterY - heaterRY, heaterRX * 2, heaterRY * 2, 0, -180);
    heaterBody.closeSubpath();
    QGraphicsPathItem *heaterBodyItem = new QGraphicsPathItem(heaterBody);
    heaterBodyItem->setBrush(QBrush(QColor("#c0392b")));
    heaterBodyItem->setPen(QPen(QColor("#922b21"), 1));
    heaterBodyItem->setZValue(4);
    heaterBodyItem->setToolTip("Нагревательный элемент (нихром)");
    addItem(heaterBodyItem);

    // --- Держатель образцов (крестовина) ---
    const double holderY = innerBotY - 25;  // ОПУЩЕН ближе к дну

    QGraphicsRectItem *holderH = new QGraphicsRectItem(
        isoCX - innerRX + 10, holderY - 3, (innerRX - 10) * 2, 6);
    holderH->setBrush(QBrush(QColor("#95a5a6")));
    holderH->setPen(QPen(QColor("#7f8c8d"), 1));
    holderH->setZValue(5);
    addItem(holderH);

    QGraphicsRectItem *holderV = new QGraphicsRectItem(
        isoCX - 3, holderY - innerRY + 10, 6, (innerRY - 10) * 2);
    holderV->setBrush(QBrush(QColor("#95a5a6")));
    holderV->setPen(QPen(QColor("#7f8c8d"), 1));
    holderV->setZValue(5);
    addItem(holderV);

    // =========================================================================
    // 4 ИЗОМЕТРИЧЕСКИХ ОБРАЗЦА — УВЕЛИЧЕННЫЕ И ОПУЩЕННЫЕ
    // =========================================================================
    const double sampleOffsets[4][2] = {
        {-55, -20},  // образец 1 (задний-левый)
        { 55, -20},  // образец 2 (задний-правый)
        {-55,  20},  // образец 3 (передний-левый)
        { 55,  20}   // образец 4 (передний-правый)
    };
    const double sRX = 35;
    const double sRY = 17;
    const double sH  = 50;

    for (int i = 0; i < 4; ++i) {
        double sx = isoCX + sampleOffsets[i][0];
        double sy = holderY + sampleOffsets[i][1];

        // Боковая поверхность
        QPainterPath bodyPath;
        bodyPath.moveTo(sx - sRX, sy - sH);
        bodyPath.lineTo(sx - sRX, sy);
        bodyPath.arcTo(sx - sRX, sy - sRY, sRX * 2, sRY * 2, 180, 180);
        bodyPath.lineTo(sx + sRX, sy - sH);
        bodyPath.arcTo(sx - sRX, sy - sH - sRY, sRX * 2, sRY * 2, 0, -180);
        bodyPath.closeSubpath();

        QGraphicsPathItem *body = new QGraphicsPathItem(bodyPath);
        body->setBrush(QBrush(QColor("#bdc3c7")));
        body->setPen(QPen(QColor("#7f8c8d"), 1));
        body->setZValue(6);
        body->setToolTip(QString("Образец %1 (цилиндр)").arg(i + 1));
        addItem(body);
        m_isoSampleBodies[i] = body;

        // Верхняя грань
        QGraphicsEllipseItem *top = new QGraphicsEllipseItem(
            sx - sRX, sy - sH - sRY, sRX * 2, sRY * 2);
        top->setBrush(QBrush(QColor("#bdc3c7")));
        top->setPen(QPen(QColor("#7f8c8d"), 1));
        top->setZValue(7);
        top->setToolTip(QString("Образец %1 (верхняя грань)").arg(i + 1));
        addItem(top);
        m_isoSampleTops[i] = top;
    }

    // --- Крышка калориметра ---
    QPainterPath lidFrontArc;
    lidFrontArc.moveTo(isoCX - outerRX, topY);
    lidFrontArc.arcTo(isoCX - outerRX, topY - outerRY, outerRX * 2, outerRY * 2, 180, 180);
    QGraphicsPathItem *lidFront = new QGraphicsPathItem(lidFrontArc);
    lidFront->setBrush(QBrush(QColor("#bdc3c7")));
    lidFront->setPen(QPen(QColor("#7f8c8d"), 2));
    lidFront->setZValue(8);
    lidFront->setToolTip("Крышка калориметра");
    addItem(lidFront);

    QPainterPath lidBody;
    lidBody.moveTo(isoCX - outerRX, topY - 15);
    lidBody.lineTo(isoCX - outerRX, topY);
    lidBody.arcTo(isoCX - outerRX, topY - outerRY, outerRX * 2, outerRY * 2, 180, 180);
    lidBody.lineTo(isoCX + outerRX, topY - 15);
    lidBody.arcTo(isoCX - outerRX, topY - 15 - outerRY, outerRX * 2, outerRY * 2, 0, -180);
    lidBody.closeSubpath();
    QGraphicsPathItem *lidBodyItem = new QGraphicsPathItem(lidBody);
    lidBodyItem->setBrush(QBrush(QColor("#95a5a6")));
    lidBodyItem->setPen(QPen(QColor("#7f8c8d"), 1));
    lidBodyItem->setZValue(8);
    lidBodyItem->setToolTip("Крышка калориметра");
    addItem(lidBodyItem);

    // =========================================================================
    // 4 ТЕРМОПАРЫ (сходятся в одну точку на крышке)
    // =========================================================================

    // Координаты центров образцов (середина высоты)
    const double thermoSampleCenters[4][2] = {
        {isoCX - 55, holderY - 20 - sH/2},
        {isoCX + 55, holderY - 20 - sH/2},
        {isoCX - 55, holderY + 20 - sH/2},
        {isoCX + 55, holderY + 20 - sH/2}
    };

    // Точка схождения на крышке (центр)
    const double junctionX = isoCX;
    const double junctionY = topY;

    // Точка "выхода" провода из крышки (чуть выше крышки)
    const double exitY = topY - 5;

    // 4 термопары внутри калориметра
    for (int i = 0; i < 4; ++i) {
        double targetX = thermoSampleCenters[i][0];
        double targetY = thermoSampleCenters[i][1];

        // === НИЖНЯЯ ЧАСТЬ ПРОВОДА (под крышкой, zValue < крышки) ===
        QPainterPath lowerPath;
        lowerPath.moveTo(targetX, targetY);
        lowerPath.lineTo(targetX, targetY - 30);  // сначала вверх
        lowerPath.lineTo(junctionX, exitY);       // к центру крышки

        QGraphicsPathItem *lowerWire = new QGraphicsPathItem(lowerPath);
        lowerWire->setPen(QPen(QColor("#34495e"), 2));
        lowerWire->setZValue(7);  // НИЖЕ крышки (у крышки zValue=8)
        lowerWire->setToolTip(QString("Термопара %1 (внутри калориметра)").arg(i + 1));
        addItem(lowerWire);

        // Наконечник термопары (тоже под крышкой)
        QGraphicsEllipseItem *thermoTip = new QGraphicsEllipseItem(
            targetX - 4, targetY - 4, 8, 8);
        thermoTip->setBrush(QBrush(Qt::black));
        thermoTip->setZValue(7);  // НИЖЕ крышки
        thermoTip->setToolTip(QString("Наконечник термопары %1").arg(i + 1));
        addItem(thermoTip);

        // === ВЕРХНЯЯ ЧАСТЬ ПРОВОДА (над крышкой, zValue > крышки) ===
        QPainterPath upperPath;
        upperPath.moveTo(junctionX, exitY);
        upperPath.lineTo(junctionX, junctionY - 5);  // короткий вертикальный участок над крышкой

        QGraphicsPathItem *upperWire = new QGraphicsPathItem(upperPath);
        upperWire->setPen(QPen(QColor("#34495e"), 2));
        upperWire->setZValue(9);  // ВЫШЕ крышки
        upperWire->setToolTip(QString("Термопара %1 (над крышкой)").arg(i + 1));
        addItem(upperWire);
    }

    // === СТВОЛ КАБЕЛЯ (только над крышкой) ===
    const double branchX = junctionX;
    const double branchY = junctionY - 40;

    QGraphicsLineItem *cableTrunk = new QGraphicsLineItem(
        junctionX, junctionY - 5, branchX, branchY);
    cableTrunk->setPen(QPen(QColor("#2c3e50"), 3));
    cableTrunk->setZValue(9);  // ВЫШЕ крышки
    cableTrunk->setToolTip("Кабель термопар (выход из калориметра)");
    addItem(cableTrunk);

    // =========================================================================
    // 4 ДИСПЛЕЯ СЛЕВА ОТ ИЗОМЕТРИЧЕСКОГО РАЗРЕЗА
    // =========================================================================

    const double isoDisplayX = 5;
    const double isoDisplayY[4] = {150, 220, 290, 360};

    for (int i = 0; i < 4; ++i) {
        m_isoDisplays[i] = new DigitalDisplay(QString::number(i + 1));
        m_isoDisplays[i]->setPos(isoDisplayX, isoDisplayY[i]);
        m_isoDisplays[i]->setZValue(10);
        addItem(m_isoDisplays[i]);
    }

    // =========================================================================
    // ПРОВОДА ОТ КАБЕЛЯ К ДИСПЛЕЯМ СЛЕВА
    // =========================================================================

    for (int i = 0; i < 4; ++i) {
        double dispX = isoDisplayX + 60;
        double dispY = isoDisplayY[i] + 25;

        QPainterPath wirePath;
        wirePath.moveTo(branchX, branchY);

        // Более плавный изгиб
        double midX = (branchX + dispX) / 2;
        wirePath.cubicTo(midX - 30, branchY,   // первая контрольная точка левее
                         midX - 30, dispY,      // вторая контрольная точка левее
                         dispX, dispY);

        QGraphicsPathItem *wire = new QGraphicsPathItem(wirePath);
        wire->setPen(QPen(QColor("#7f8c8d"), 1.5));
        wire->setZValue(8);
        wire->setToolTip(QString("Провод к дисплею T%1").arg(i + 1));
        addItem(wire);
    }
    // =========================================================================
    // ЦЕНТРАЛЬНАЯ ЧАСТЬ: ВИД СВЕРХУ (4 образца в квадрате)
    // =========================================================================

    QGraphicsRectItem *body = new QGraphicsRectItem(150+250, 125, 550, 550);
    body->setBrush(QBrush(QColor("#f5f5f5")));
    body->setPen(QPen(QColor("#a0a0a0"), 2));
    body->setZValue(0);
    addItem(body);

    QGraphicsRectItem *insulation = new QGraphicsRectItem(170+250, 145, 510, 510);
    insulation->setBrush(Qt::NoBrush);
    QPen dashPen(QColor("#bdc3c7"), 1, Qt::DashLine);
    insulation->setPen(dashPen);
    insulation->setZValue(0);
    addItem(insulation);

    QPointF positions[] = {
        {320+250+15, 250}, {520+250, 250},
        {320+250+15, 500}, {520+250, 500}
    };

    for (int i = 0; i < 4; ++i) {
        QGraphicsEllipseItem *sample = new QGraphicsEllipseItem(
            positions[i].x() - 50, positions[i].y() - 50, 100, 100);
        sample->setBrush(QBrush(QColor("#bdc3c7"), Qt::NoBrush));
        sample->setPen(QPen(QColor("#7f8c8d"), 2));
        sample->setZValue(1);
        addItem(sample);
        m_sampleCircles[i] = sample;

        QGraphicsTextItem* cross = new QGraphicsTextItem("✕");
        cross->setDefaultTextColor(QColor("#FF0000"));
        cross->setFont(QFont("Arial", 50, QFont::Bold));
        qreal textWidth = cross->boundingRect().width();
        qreal textHeight = cross->boundingRect().height();
        cross->setPos(positions[i].x() - textWidth / 2, positions[i].y() - textHeight / 2);
        cross->setVisible(false);
        cross->setZValue(2);
        addItem(cross);
        m_sampleCrosses[i] = cross;
    }

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
        if(i % 2 == 0)
            proxy->setPos(positions[i].x() - 110, positions[i].y() - 50);
        else
            proxy->setPos(positions[i].x() + 40, positions[i].y() - 50);
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
        if(i % 2 == 0)
            diffProxy->setPos(positions[i].x() - 155, positions[i].y() - 50);
        else
            diffProxy->setPos(positions[i].x() + 110, positions[i].y() - 50);
        addItem(diffProxy);
        m_differentialModeCheckBox[i] = diffCheckBox;
        connect(diffCheckBox, &QCheckBox::toggled,
                this, [this, i](bool checked) {
                    this->onDifferentialModeToggled(i, checked);
                });
    }

    // 4 дисплея T1-T4 (вид сверху)
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
}

// =====================================================================
// ВИЗУАЛИЗАЦИЯ (Цвет + Штриховка)
// =====================================================================

void CalorimeterScene::setCoolingStartTemperature(double startTemp)
{
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
    updateSampleVisuals(index, m_lastSampleTemps[index]);
    updateIsoSampleVisuals(index, m_lastSampleTemps[index]);
}

void CalorimeterScene::updateSampleVisuals(int index, double currentTemp)
{
    if (!isActive[index] || !m_sampleCircles[index]) return;

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

    QPixmap texture(32, 32);
    texture.fill(baseColor);

    if (!m_currentMaterials[index].isEmpty() && m_currentMaterials[index] != "Выберите") {
        uint hash = qHash(m_currentMaterials[index]);
        int patternType = hash % 8;

        QPainter painter(&texture);
        painter.setPen(QPen(QColor(60, 60, 60), 2));

        if (patternType == 0) {
            for (int y = 0; y < 32; y += 8) painter.drawLine(0, y, 32, y);
        } else if (patternType == 1) {
            for (int x = 0; x < 32; x += 8) painter.drawLine(x, 0, x, 32);
        } else if (patternType == 2) {
            for (int i = -31; i < 32; i += 8)
                painter.drawLine(qMax(0, i), qMax(0, -i), qMin(32, 32+i), qMin(32, 32-i));
        } else if (patternType == 3) {
            for (int i = 0; i < 64; i += 8)
                painter.drawLine(qMax(0, i-31), qMin(31, i), qMin(31, i), qMax(0, i-31));
        } else if (patternType == 4) {
            for (int y = 0; y < 32; y += 16) painter.drawLine(0, y, 32, y);
            for (int x = 0; x < 32; x += 16) painter.drawLine(x, 0, x, 32);
        } else if (patternType == 5) {
            for (int i = -31; i < 32; i += 16)
                painter.drawLine(qMax(0, i), qMax(0, -i), qMin(32, 32+i), qMin(32, 32-i));
            for (int i = 0; i < 64; i += 16)
                painter.drawLine(qMax(0, i-31), qMin(31, i), qMin(31, i), qMax(0, i-31));
        } else if (patternType == 6) {
            for (int y = 0; y < 32; y += 8)
                for (int x = 0; x < 32; x += 8)
                    painter.drawPoint(x, y);
        } else {
            for (int y = 0; y < 32; y += 8) painter.drawLine(0, y, 32, y);
            for (int x = 0; x < 32; x += 16) painter.drawLine(x, 0, x, 32);
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

void CalorimeterScene::updateIsoSampleVisuals(int index, double currentTemp)
{
    if (!isActive[index]) return;
    if (!m_isoSampleTops[index] || !m_isoSampleBodies[index]) return;

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

    QPixmap texture(32, 32);
    texture.fill(baseColor);

    if (!m_currentMaterials[index].isEmpty() && m_currentMaterials[index] != "Выберите") {
        uint hash = qHash(m_currentMaterials[index]);
        int patternType = hash % 8;

        QPainter painter(&texture);
        painter.setPen(QPen(QColor(60, 60, 60), 2));

        if (patternType == 0) {
            for (int y = 0; y < 32; y += 8) painter.drawLine(0, y, 32, y);
        } else if (patternType == 1) {
            for (int x = 0; x < 32; x += 8) painter.drawLine(x, 0, x, 32);
        } else if (patternType == 2) {
            for (int i = -31; i < 32; i += 8)
                painter.drawLine(qMax(0, i), qMax(0, -i), qMin(32, 32+i), qMin(32, 32-i));
        } else if (patternType == 3) {
            for (int i = 0; i < 64; i += 8)
                painter.drawLine(qMax(0, i-31), qMin(31, i), qMin(31, i), qMax(0, i-31));
        } else if (patternType == 4) {
            for (int y = 0; y < 32; y += 16) painter.drawLine(0, y, 32, y);
            for (int x = 0; x < 32; x += 16) painter.drawLine(x, 0, x, 32);
        } else if (patternType == 5) {
            for (int i = -31; i < 32; i += 16)
                painter.drawLine(qMax(0, i), qMax(0, -i), qMin(32, 32+i), qMin(32, 32-i));
            for (int i = 0; i < 64; i += 16)
                painter.drawLine(qMax(0, i-31), qMin(31, i), qMin(31, i), qMax(0, i-31));
        } else if (patternType == 6) {
            for (int y = 0; y < 32; y += 8)
                for (int x = 0; x < 32; x += 8)
                    painter.drawPoint(x, y);
        } else {
            for (int y = 0; y < 32; y += 8) painter.drawLine(0, y, 32, y);
            for (int x = 0; x < 32; x += 16) painter.drawLine(x, 0, x, 32);
        }
        painter.end();
    }

    QBrush textureBrush(texture);
    m_isoSampleTops[index]->setBrush(textureBrush);

    QColor sideColor = baseColor.darker(115);
    m_isoSampleBodies[index]->setBrush(QBrush(sideColor));
    m_isoSampleBodies[index]->setPen(QPen(QColor("#7f8c8d"), 1));
}

// =====================================================================
// СТАРАЯ ЛОГИКА
// =====================================================================

void CalorimeterScene::updateTemperatures(double t1, double t2, double t3, double t4, double t0)
{
    m_lastSampleTemps[0] = t1;
    m_lastSampleTemps[1] = t2;
    m_lastSampleTemps[2] = t3;
    m_lastSampleTemps[3] = t4;
    m_lastT0 = t0;

    double temps[] = {t1, t2, t3, t4, t0};

    // Обновляем основные дисплеи T1-T4 (вид сверху)
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

    // Обновляем изометрические дисплеи (дублируем значения)
    for (int i = 0; i < 4; ++i) {
        if (m_isoDisplays[i]) {
            if (!isActive[i]) {
                m_isoDisplays[i]->setValue(0.0);
                continue;
            }
            if (m_isDifferentialMode[i]) {
                double deltaT = temps[i] - t0;
                m_isoDisplays[i]->setValue(deltaT);
                m_isoDisplays[i]->setPrefix("ΔT");
                m_isoDisplays[i]->setTextColor(Qt::red);
            } else {
                m_isoDisplays[i]->setValue(temps[i]);
                m_isoDisplays[i]->setPrefix("T");
                m_isoDisplays[i]->setTextColor(QColor("#00ff00"));
            }
        }
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
        if (m_isoDisplays[sampleIndex]) m_isoDisplays[sampleIndex]->setValue(0.0);
        return;
    }
    if (m_isDifferentialMode[sampleIndex]) {
        double deltaT = T_sample - T0;
        displays[sampleIndex]->setValue(deltaT);
        displays[sampleIndex]->setPrefix("ΔT");
        displays[sampleIndex]->setTextColor(Qt::red);
        if (m_isoDisplays[sampleIndex]) {
            m_isoDisplays[sampleIndex]->setValue(deltaT);
            m_isoDisplays[sampleIndex]->setPrefix("ΔT");
            m_isoDisplays[sampleIndex]->setTextColor(Qt::red);
        }
    } else {
        displays[sampleIndex]->setValue(T_sample);
        displays[sampleIndex]->setPrefix("T");
        displays[sampleIndex]->setTextColor(Qt::green);
        if (m_isoDisplays[sampleIndex]) {
            m_isoDisplays[sampleIndex]->setValue(T_sample);
            m_isoDisplays[sampleIndex]->setPrefix("T");
            m_isoDisplays[sampleIndex]->setTextColor(Qt::green);
        }
    }
}

void CalorimeterScene::setEnvironmentTemperature(double t0)
{
    m_lastT0 = t0;
}

void CalorimeterScene::onPhysicsTemperaturesUpdated(const QVector<double>& temps, int elapsedSec)
{
    Q_UNUSED(elapsedSec);
    if (temps.size() >= 4) {
        for(int i = 0; i < 4; i++) {
            if(isActive[i]) {
                updateSampleVisuals(i, temps[i]);
                updateIsoSampleVisuals(i, temps[i]);
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

void CalorimeterScene::updateThemeColors(bool isDark)
{
    if (isDark) {
        setBackgroundBrush(QColor("#1e1e1e"));
    } else {
        setBackgroundBrush(QColor("#f0f0f0"));
    }
}

void CalorimeterScene::resetSampleVisuals()
{
    // Сбрасываем начальные температуры
    for (int i = 0; i < 4; ++i) {
        m_startTemps[i] = 0.0;

        // Сбрасываем цвет образцов до серого (температура T0)
        updateSampleVisuals(i, m_lastT0);
        updateIsoSampleVisuals(i, m_lastT0);

        // Сбрасываем показания дисплеев до T0
        updateDisplayForSample(i, m_lastT0, m_lastT0);
    }
}
