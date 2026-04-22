#include "CalorimeterScene.h"
#include <QPen>
#include <QBrush>
#include <QGraphicsTextItem>
#include <QFont>

CalorimeterScene::CalorimeterScene(QObject *parent)
    : QGraphicsScene(parent)
{
    buildLayout();
}

void CalorimeterScene::buildLayout()
{
    // 1. Увеличиваем сцену, чтобы элементы не прилипали к краям
    setSceneRect(0, 0, 600, 600);

    // Делаем фон прозрачным, чтобы он сливался с окном программы
    setBackgroundBrush(Qt::transparent);

    // --- 1. Рисуем корпус калориметра (Термос) ---
    QGraphicsRectItem *body = new QGraphicsRectItem(100, 100, 400, 400);
    body->setBrush(QBrush(QColor("#f5f5f5"))); // Очень светлый серый
    body->setPen(QPen(QColor("#a0a0a0"), 2));  // Тонкая серая рамка
    body->setZValue(0);
    addItem(body);

    // Текст "ВАКУУМНЫЙ КАЛОРИМЕТР" (сверху)
    QGraphicsTextItem *title = new QGraphicsTextItem("ВАКУУМНЫЙ КАЛОРИМЕТР");
    title->setFont(QFont("Arial", 14, QFont::Bold));
    title->setDefaultTextColor(QColor("#333"));
    // Центрируем текст по ширине сцены
    title->setPos(300 - title->boundingRect().width() / 2, 40);
    addItem(title);

    // --- 2. Рисуем теплоизоляцию (Внутренний пунктир) ---
    QGraphicsRectItem *insulation = new QGraphicsRectItem(130, 130, 340, 340);
    insulation->setBrush(Qt::NoBrush);
    QPen dashPen(QColor("#bdc3c7"), 1, Qt::DashLine);
    insulation->setPen(dashPen);
    insulation->setZValue(0);
    addItem(insulation);

    // --- 3. Рисуем 4 образца (центруем их внутри изоляции) ---
    // Центры образцов относительно всей сцены
    QPointF positions[] = {
        {230, 230}, {370, 230}, // Верхний ряд
        {230, 370}, {370, 370}  // Нижний ряд
    };

    for (int i = 0; i < 4; ++i) {
        int row = i / 2;
        int col = i % 2;

        // Тело образца (Круг)
        QGraphicsEllipseItem *sample = new QGraphicsEllipseItem(
            positions[i].x() - 40, positions[i].y() - 40, 80, 80
            );
        sample->setBrush(QBrush(QColor("#bdc3c7"))); // Серебристый
        sample->setPen(QPen(QColor("#7f8c8d"), 2));
        sample->setZValue(1);
        addItem(sample);

       /* // Текст "Обр. N"
        QGraphicsTextItem *lbl = new QGraphicsTextItem(QString::number(i + 1), sample);
        lbl->setFont(QFont("Arial", 14, QFont::Bold));
        lbl->setDefaultTextColor(QColor("#2c3e50"));

        qreal centerX = positions[i].x();
        qreal textWidth = lbl->boundingRect().width();
        qreal textHeight = lbl->boundingRect().height();
        lbl->setPos(-textWidth / 2, -textHeight / 2); */
    }

    // --- 4. Размещаем цифровые табло (DigitalDisplay) ---
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

    // T0 (Среда) - Снизу по центру
    displays[4] = new DigitalDisplay("T0 (Среда)");
    displays[4]->setPos(250, 520);
    addItem(displays[4]);
}

void CalorimeterScene::updateTemperatures(double t1, double t2, double t3, double t4, double t0)
{
    displays[0]->setValue(t1);
    displays[1]->setValue(t2);
    displays[2]->setValue(t3);
    displays[3]->setValue(t4);
    displays[4]->setValue(t0);
}
