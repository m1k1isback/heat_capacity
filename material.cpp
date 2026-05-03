#include "material.h"

// Конструктор
Material::Material(const QString& name,
                   double debyeTempK,
                   double molarMassKgMol,
                   double refSpecificHeatJKgK,
                   double densityGcm3)
    : m_name(name)
    , m_debyeTemperature(debyeTempK)
    , m_molarMass(molarMassKgMol)
    , m_refSpecificHeat(refSpecificHeatJKgK)
    , m_density(densityGcm3)
{
}

Material::Material()
    : m_name("")
    , m_debyeTemperature(0.0)
    , m_molarMass(0.0)
    , m_refSpecificHeat(0.0)
    , m_density(0.0)
{
}

// Геттеры
QString Material::name() const { return m_name; }
double Material::debyeTemperature() const { return m_debyeTemperature; }
double Material::molarMass() const { return m_molarMass; }
double Material::referenceSpecificHeat() const { return m_refSpecificHeat; }


Material Material::copper()
{
    // Медь (Cu): Θ=343 К, M=63.546 г/моль, c≈385 Дж/(кг·К), ρ=8.96 г/см³
    return Material("Медь (Cu)",
                    343.0,
                    0.063546,
                    385.0,
                    8.96);  // ← ДОБАВЬТЕ ЭТОТ ПАРАМЕТР (плотность)
}

Material Material::aluminum()
{
    // Алюминий (Al): Θ=428 К, M=26.982 г/моль, c≈900 Дж/(кг·К), ρ=2.70 г/см³
    return Material("Алюминий (Al)",
                    428.0,
                    0.026982,
                    900.0,
                    2.70);  // ← ДОБАВЬТЕ ЭТОТ ПАРАМЕТР
}

Material Material::iron()
{
    // Железо (Fe): Θ=420 К, M=55.845 г/моль, c≈450 Дж/(кг·К), ρ=7.87 г/см³
    return Material("Железо (Fe)",
                    420.0,
                    0.055845,
                    450.0,
                    7.87);  // ← ДОБАВЬТЕ ЭТОТ ПАРАМЕТР
}

Material Material::lead()
{
    // Свинец (Pb): Θ=105 К, M=207.2 г/моль, c≈130 Дж/(кг·К), ρ=11.34 г/см³
    return Material("Свинец (Pb)",
                    105.0,
                    0.2072,
                    130.0,
                    11.34);
}

double Material::densityGcm3() const
{
    return m_density;
}

QList<Material> Material::registry()
{
    return {
        copper(),
        aluminum(),
        iron(),
        lead()
        // + nickel(), beryllium() и др.
    };
}

std::optional<Material> Material::findByName(const QString& name)
{
    for (const auto& mat : registry()) {
        if (mat.name() == name) {
            return std::make_optional(mat);
        }
    }
    return std::nullopt;
}
