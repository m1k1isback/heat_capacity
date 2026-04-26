#ifndef DIGITALDISPLAY_H
#define DIGITALDISPLAY_H

#include <QGraphicsItemGroup>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QFont>

class DigitalDisplay : public QGraphicsItemGroup
{
public:
    // label — подпись (например, "T1"), parent — родительский элемент
    explicit DigitalDisplay(const QString &label, QGraphicsItem *parent = nullptr);

    // Метод для обновления значения
    void setValue(double value);

    void setPrefix(const QString &prefix);    // для установки "T" или "ΔT"
    void setTextColor(const QColor &color);

private:
    QGraphicsRectItem *bgRect;      // Фон табло
    QGraphicsTextItem *labelText;   // Подпись (T1, T2...)
    QGraphicsTextItem *valueText;   // Цифры

    QString m_label;
    QString m_prefix;      // префикс ("T" или "ΔT")
    QColor m_textColor;    // цвет текста
    QGraphicsTextItem* m_textItem;  // или как у тебя называется текстовый элемент
};

#endif // DIGITALDISPLAY_H
