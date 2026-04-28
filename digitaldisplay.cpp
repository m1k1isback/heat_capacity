#include "DigitalDisplay.h"
#include <QPen>
#include <QBrush>

DigitalDisplay::DigitalDisplay(const QString &label, QGraphicsItem *parent)
    : QGraphicsItemGroup(parent), m_label(label), m_prefix("T")
{
    // Фон дисплея
    bgRect = new QGraphicsRectItem(0, 0, 80, 40, this);
    bgRect->setBrush(QBrush(QColor("#1a1a2e"))); // Тёмно-синий фон
    bgRect->setPen(QPen(QColor("#444444"), 1));
    bgRect->setZValue(1);

    // Подписи
    labelText = new QGraphicsTextItem(label, this);
    labelText->setDefaultTextColor(QColor("#aaa"));
    labelText->setFont(QFont("Arial", 11, QFont::Bold));
    labelText->setPos(5, 0);
    labelText->setZValue(2);
    labelText->setPlainText(m_prefix + m_label);

    // Значение температуры
    valueText = new QGraphicsTextItem("0.0 °C", this);
    valueText->setDefaultTextColor(QColor("#00ff00"));
    valueText->setFont(QFont("Courier New", 13, QFont::Bold));
    valueText->setPos(-3, 16);
    valueText->setZValue(2);

}

void DigitalDisplay::setValue(double value)
{
    valueText->setPlainText(QString("%1 °C").arg(value, 5, 'f', 1));
}
// Замена префикса в дисплее
void DigitalDisplay::setPrefix(const QString &prefix)
{
    m_prefix = prefix;
    labelText->setPlainText(m_prefix + m_label);
}

// setTextColor: меняет цвет цифр температуры
void DigitalDisplay::setTextColor(const QColor &color)
{
    valueText->setDefaultTextColor(color);
}
