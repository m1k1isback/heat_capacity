#include "DigitalDisplay.h"
#include <QPen>
#include <QBrush>

DigitalDisplay::DigitalDisplay(const QString &label, QGraphicsItem *parent)
    : QGraphicsItemGroup(parent)
{
    // Фон дисплея
    bgRect = new QGraphicsRectItem(0, 0, 80, 40, this);
    bgRect->setBrush(QBrush(QColor("#1a1a2e"))); // Тёмно-синий фон
    bgRect->setPen(QPen(QColor("#444444"), 1));
    bgRect->setZValue(1);

    // Подписи
    labelText = new QGraphicsTextItem(label, this);
    labelText->setDefaultTextColor(QColor("#aaa"));
    labelText->setFont(QFont("Arial", 9, QFont::Bold));
    labelText->setPos(5, 2);
    labelText->setZValue(2);

    // Значение температуры
    valueText = new QGraphicsTextItem("---.-", this);
    valueText->setDefaultTextColor(QColor("#00ff00")); // Ярко-зелёный цвет
    valueText->setFont(QFont("Courier New", 14, QFont::Bold));
    valueText->setPos(5, 16);
    valueText->setZValue(2);
}

void DigitalDisplay::setValue(double value)
{
    valueText->setPlainText(QString("%1 °C").arg(value, 5, 'f', 1));
}
