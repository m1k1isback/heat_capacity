// === material.h ===
#pragma once
#include <QString>
#include <QList>
#include <optional>

class Material
{
public:
    // Конструктор
    Material(const QString& name,
             double debyeTempK,      // Θ, К
             double molarMassKgMol,  // M, кг/моль
             double refSpecificHeatJKgK, // c_ref, Дж/(кг·К) при ~300 К
             double densityGcm3);    // ρ, г/см³

    // === Геттеры (константные) ===
    QString name() const;
    double debyeTemperature() const;  // Θ, К
    double molarMass() const;         // M, кг/моль
    double referenceSpecificHeat() const; // c_ref, Дж/(кг·К)
    double densityGcm3() const;       // ρ, г/см³

    // === Статический реестр — ЕДИНСТВЕННОЕ место со списком материалов ===
    static QList<Material> registry();

    // === Поиск по имени (возвращает пустой optional, если не найдено) ===
    static std::optional<Material> findByName(const QString& name);
    Material();

private:
    QString m_name;
    double m_debyeTemperature;   // Θ, К
    double m_molarMass;          // M, кг/моль
    double m_refSpecificHeat;    // c_ref, Дж/(кг·К)
    double m_density;            // ρ, г/см³

    // === Фабричные методы для каждого материала (приватные или публичные — на ваш выбор) ===
    static Material copper();
    static Material aluminum();
    static Material iron();
    static Material lead();

    // TODO: позже добавите nickel(), beryllium() и ещё 54 материала
};
