#include "CalorimeterScene.h"
#include <QPen>
#include <QBrush>
#include <QGraphicsTextItem>
#include <QFont>
#include <QCheckBox>
#include <QGraphicsProxyWidget>
#include <QTimer>

CalorimeterScene::CalorimeterScene(QObject *parent)
    : QGraphicsScene(parent)
{
    buildLayout();
}

void CalorimeterScene::buildLayout()
{
    setSceneRect(0, 0, 600, 600);

    // Прозрачный фон
    setBackgroundBrush(Qt::transparent);

    // Корпус калориметра
    QGraphicsRectItem *body = new QGraphicsRectItem(100, 100, 400, 400);
    body->setBrush(QBrush(QColor("#f5f5f5")));
    body->setPen(QPen(QColor("#a0a0a0"), 2));
    body->setZValue(0);
    addItem(body);

    // Текст "ВАКУУМНЫЙ КАЛОРИМЕТР"
   /* QGraphicsTextItem *title = new QGraphicsTextItem("ВАКУУМНЫЙ КАЛОРИМЕТР");
    title->setFont(QFont("Arial", 14, QFont::Bold));
    title->setDefaultTextColor(QColor("#333"));
    // Центрируем текст по ширине сцены
    title->setPos(300 - title->boundingRect().width() / 2, 40);
    addItem(title); */

    // Пунктир теплоизоляции
    QGraphicsRectItem *insulation = new QGraphicsRectItem(130, 130, 340, 340);
    insulation->setBrush(Qt::NoBrush);
    QPen dashPen(QColor("#bdc3c7"), 1, Qt::DashLine);
    insulation->setPen(dashPen);
    insulation->setZValue(0);
    addItem(insulation);

    // 4 образца
    // Центры образцов относительно всей сцены
    QPointF positions[] = {
        {230, 230}, {370, 230}, // Верхний ряд
        {230, 370}, {370, 370}  // Нижний ряд
    };

    for (int i = 0; i < 4; ++i) {
        int row = i / 2;
        int col = i % 2;

        // Тело образца
        QGraphicsEllipseItem *sample = new QGraphicsEllipseItem(
            positions[i].x() - 40, positions[i].y() - 40, 80, 80
            );
        sample->setBrush(QBrush(QColor("#bdc3c7"))); // Серебристый
        sample->setPen(QPen(QColor("#7f8c8d"), 2));
        sample->setZValue(1);
        addItem(sample);

        m_sampleCircles[i] = sample;

        // Крест если образец выключен
        QGraphicsTextItem* cross = new QGraphicsTextItem("✕");

        cross->setDefaultTextColor(QColor("#4B0082"));

        cross->setFont(QFont("Arial", 30, QFont::Bold));

        qreal textWidth = cross->boundingRect().width();
        qreal textHeight = cross->boundingRect().height();
        qreal centerX = positions[i].x() - textWidth / 2;
        qreal centerY = positions[i].y() - textHeight / 2;
        cross->setPos(centerX, centerY);

        cross->setVisible(false);

        addItem(cross);

        m_sampleCrosses[i] = cross;

        cross->setZValue(2);

    }

    for(int i = 0; i < 4; i++){
        QCheckBox *cb = new QCheckBox();
        cb->setText("Обр. " + QString::number(i + 1));
        checkBoxes[i] = cb;
        cb->setChecked(true);
        QGraphicsProxyWidget* proxy = new QGraphicsProxyWidget();
        proxy->setWidget(cb);
        addItem(proxy);
        if(i % 2 == 0){
            proxy->setPos(positions[i].x() - 90, positions[i].y() - 50);
        } else {
            proxy->setPos(positions[i].x() + 40, positions[i].y() - 50);
        }
        connect(checkBoxes[i], &QCheckBox::toggled, this, [this, i]() {
            this->checkSample(i, checkBoxes[i]->isChecked());
        });
    }

    // 4 дисплея
    // T1 (Левый Верхний) - Чуть левее образца
    displays[0] = new DigitalDisplay("T1");
    displays[0]->setPos(110, 210);
    addItem(displays[0]);

    // T2 (Правый Верхний) - Чуть правее образца
    displays[1] = new DigitalDisplay("T2");
    displays[1]->setPos(410, 210);
    addItem(displays[1]);

    // T3 (Левый Нижний)
    displays[2] = new DigitalDisplay("T3");
    displays[2]->setPos(110, 350);
    addItem(displays[2]);

    // T4 (Правый Нижний)
    displays[3] = new DigitalDisplay("T4");
    displays[3]->setPos(410, 350);
    addItem(displays[3]);

    //---------------------------------------------
    // T0 (Среда) - Снизу по центру
    displays[4] = new DigitalDisplay("T0 (Среда)");
    displays[4]->setPos(250, 520);
    addItem(displays[4]);
    //------------------------------------------

    QPointF displayCenters[] = {
        QPointF(150, 250),  // T1: pos(110,210) + половина размера
        QPointF(450, 250),  // T2: pos(410,210) + половина размера
        QPointF(150, 390),  // T3: pos(110,350) + половина размера
        QPointF(450, 390)   // T4: pos(410,350) + половина размера
    };

    for(int i = 0; i < 4; i++) {
        //Провода к дисплею
        QPointF start = positions[i];
        QPointF end = displayCenters[i];

        QPainterPath path;
        path.moveTo(start.x(), start.y());

        //контрольная точка
        qreal midX = ((start.x() + end.x()) / 2);
        qreal midY = ((start.y() + end.y()) / 2);

        path.quadTo(midX + 15, midY + 40, end.x(), end.y());

        QGraphicsPathItem* wire = new QGraphicsPathItem(path);

        // Настройка пера
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

void CalorimeterScene::updateTemperatures(double t1, double t2, double t3, double t4, double t0)
{
    double  temps[] = {t1, t2, t3, t4, t0};

    for(int i = 0; i < 4; i++){
        if(isActive[i]){
            displays[i]->setValue(temps[i]);
        } else {
            displays[i]->setValue(0.0);
        }
    }

    displays[4]->setValue(t0);
}

void CalorimeterScene::checkSample(int index, bool  checked){
    if(index < 0 || index >= 4) return;
    isActive[index] = checked;
    m_sampleCrosses[index]->setVisible(!checked);
}
