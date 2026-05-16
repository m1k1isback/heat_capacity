#pragma once

// Forward declarations (используются только как указатели — ускоряет компиляцию)
class QGraphicsRectItem;
class QGraphicsTextItem;

// Необходимые заголовки (типы, используемые по значению в сигнатурах методов)
#include <QGraphicsItemGroup>
#include <QString>
#include <QColor>

class DigitalDisplay : public QGraphicsItemGroup
{
public:
    // Конструктор с explicit (защита от неявных преобразований)
    explicit DigitalDisplay(const QString &label, QGraphicsItem *parent = nullptr);
    ~DigitalDisplay() override = default;  // Явный виртуальный деструктор

    // Публичные методы (интерфейс класса)
    void setValue(double value);
    void setPrefix(const QString &prefix);
    void setTextColor(const QColor &color);

private:
    // Графические элементы (инициализация = nullptr)
    QGraphicsRectItem *bgRect = nullptr;
    QGraphicsTextItem *labelText = nullptr;
    QGraphicsTextItem *valueText = nullptr;

    // Данные (строки и цвета)
    QString m_label;
    QString m_prefix;
    QColor m_textColor;

    // Текстовый элемент (инициализация = nullptr)
    QGraphicsTextItem* m_textItem = nullptr;
};
