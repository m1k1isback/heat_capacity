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

private:
    QGraphicsRectItem *bgRect;      // Фон табло
    QGraphicsTextItem *labelText;   // Подпись (T1, T2...)
    QGraphicsTextItem *valueText;   // Цифры
};

#endif // DIGITALDISPLAY_H
