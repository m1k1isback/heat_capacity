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
             const QString& symbol,
             double debyeTempK,      // Θ, К
             double molarMassKgMol,  // M, кг/моль
             double refSpecificHeatJKgK, // c_ref, Дж/(кг·К) при ~300 К
             double densityGcm3);    // ρ, г/см³

    // === НОВОЕ: Расчёт теплоёмкости при конкретной температуре ===
    double getSpecificHeatAt(double tempK) const;

    // === Геттеры (константные) ===
    QString name() const;
    QString symbol() const;
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
    // === НОВОЕ: Вспомогательная функция для интеграла Дебая ===
    double calculateDebyeFunction(double x) const;

    QString m_name;
    QString m_symbol;
    double m_debyeTemperature;   // Θ, К
    double m_molarMass;          // M, кг/моль
    double m_refSpecificHeat;    // c_ref, Дж/(кг·К)
    double m_density;            // ρ, г/см³

    // === Фабричные методы для каждого материала (приватные или публичные — на ваш выбор) ===
private:
    static Material copper();
    static Material aluminum();
    static Material steel();
    static Material stainlessSteel();
    static Material lead();
    static Material polyethylene();
    static Material ptfe();
    static Material oak1();
    static Material beech1();
    static Material beech2();
    static Material beech3();
    static Material larch1();
    static Material larch2();
    static Material larch3();
    static Material ash1();
    static Material ash2();
    static Material porcelain1();
    static Material porcelain2();
    static Material rubber();
    static Material brickRed();
    static Material brickYellow();
    static Material brickSil1();
    static Material brickSil2();
    static Material glass();
    static Material ebonite();
    static Material carbolite();
    static Material textolite();
    static Material polycarbonate();
    static Material brass();
    static Material bronze();
    static Material zinc();
    static Material tin();
    static Material duralumin();
    static Material silumin();
    static Material castIron();
    static Material titanium();
    static Material tungsten();
    static Material molybdenum();
    static Material gold();
    static Material platinum();
    static Material silver();
    static Material nickel();
    static Material beryllium();
    static Material zirconium();
    static Material paraffin();

    // TODO: позже добавите nickel(), beryllium() и ещё 54 материала
};
