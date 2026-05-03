#ifndef SAMPLE_H
#define SAMPLE_H

#include "material.h"

struct Sample {
    int id;                  // Номер образца (0-3)
    Material material;       // Материал (копия объекта)
    double massGrams;        // Масса в граммах (введенная юзером)
    double currentTemp;      // Текущая температура (меняется в процессе опыта)
    bool isActive;           // Флаг: выбран ли материал вообще

    // Конструктор (инициализация полей)
    Sample();
    Sample(int idx, const Material& mat, double massG);

    // Метод расчета теплоемкости C_об (Дж/К)
    // Формула 3.7: C_ob = c_уд * m
    // ВНИМАНИЕ: c_уд в Дж/(кг*К), а масса в граммах. Нужно перевести!
    double calculateHeatCapacity() const;
};
#endif // SAMPLE_H
