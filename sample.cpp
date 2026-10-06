#include "sample.h"

Sample::Sample()
    : id(-1), massGrams(0.0), currentTemp(20.0), isActive(false)
{}

Sample::Sample(int idx, const Material& mat, double massG)
    : id(idx), material(mat), massGrams(massG), currentTemp(20.0), isActive(false)
{}

// Нужно для движка
// Расчет теплоемкости образца с учетом зависимости от температуры (Модель Дебая)
double Sample::calculateHeatCapacity() const
{
    if (!isActive) return 0.0;

    // Получаем удельную теплоёмкость именно для текущей температуры образца
    double specificHeatAtCurrentTemp = material.getSpecificHeatAt(currentTemp);

    // Умножаем на массу в кг, чтобы получить полную теплоёмкость в Дж/К
    return specificHeatAtCurrentTemp * (massGrams / 1000.0);
}
