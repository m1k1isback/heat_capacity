#include "sample.h"


Sample::Sample()
    : id(-1), massGrams(0.0), currentTemp(20.0), isActive(false)
{}

Sample::Sample(int idx, const Material& mat, double massG)
    : id(idx), material(mat), massGrams(massG), currentTemp(20.0), isActive(true)
{}

double Sample::calculateHeatCapacity() const
{
    if (!isActive) return 0.0;

    // Перевод массы из граммов в килограммы (делением на 1000.0)
    // c_уд берётся из Material в Дж/(кг·К)
    return material.referenceSpecificHeat() * (massGrams / 1000.0);
}
