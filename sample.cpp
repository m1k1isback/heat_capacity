#include "sample.h"

Sample::Sample()
    : id(-1), massGrams(0.0), currentTemp(20.0), isActive(false)
{}

Sample::Sample(int idx, const Material& mat, double massG)
    : id(idx), material(mat), massGrams(massG), currentTemp(20.0), isActive(false)
{}

// Расчет теплоемкости образца по формуле масса умножить на удельную теплоемкость
// Нужно для движка
double Sample::calculateHeatCapacity() const
{
    if (!isActive) return 0.0;
    return material.referenceSpecificHeat() * (massGrams / 1000.0);
}
