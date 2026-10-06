#include "material.h"
#include <cmath>
#include <algorithm>

// Конструктор
Material::Material(const QString& name,
                   const QString& symbol,
                   double debyeTempK,
                   double molarMassKgMol,
                   double refSpecificHeatJKgK,
                   double densityGcm3)
    : m_name(name)                              // Название материала
    , m_symbol(symbol)                          // Символ
    , m_debyeTemperature(debyeTempK)            // Температура Дебая
    , m_molarMass(molarMassKgMol)               // Молярная масса (кг/моль)
    , m_refSpecificHeat(refSpecificHeatJKgK)    // Удельная теплоёмкость (Дж/(кг·К))
    , m_density(densityGcm3)                    // Плотность (г/см³)
{
}

Material::Material()
    : m_name("")
    , m_symbol("")
    , m_debyeTemperature(0.0)
    , m_molarMass(0.0)
    , m_refSpecificHeat(0.0)
    , m_density(0.0)
{
}

// ============================================================================
// МОДЕЛЬ ДЕБАЯ ДЛЯ ТЕПЛОЁМКОСТИ
// ============================================================================

double Material::calculateDebyeFunction(double x) const {
    // x = Theta_D / T
    if (x < 0.01) return 1.0; // Высокотемпературный предел (плато)
    if (x > 15.0) return 77.92 / (x * x * x); // Низкотемпературный предел (T^3)

    // Численное интегрирование методом Симпсона (20 шагов - достаточно для точности)
    int n = 20;
    double h = x / n;
    double sum = 0.0;

    auto integrand = [](double t) {
        if (t < 1e-5) return t * t; // Предел при t -> 0, чтобы избежать деления на 0
        double exp_t = std::exp(t);
        double denom = exp_t - 1.0;
        return (t * t * t * t * exp_t) / (denom * denom);
    };

    sum += integrand(0.0);
    sum += integrand(x);

    for (int i = 1; i < n; ++i) {
        double t = i * h;
        if (i % 2 == 1) {
            sum += 4.0 * integrand(t);
        } else {
            sum += 2.0 * integrand(t);
        }
    }

    sum *= h / 3.0;
    return (3.0 / (x * x * x)) * sum;
}

double Material::getSpecificHeatAt(double tempK) const {
    if (m_debyeTemperature <= 0.0 || tempK <= 0.0) {
        return m_refSpecificHeat; // Защита от некорректных данных
    }

    // 1. Теоретический максимум (Закон Дюлонга-Пти): 3 * R / M
    // R = 8.31446 Дж/(моль*К). Результат в Дж/(кг*К)
    double c_max = (3.0 * 8.31446) / m_molarMass;

    // 2. Значение функции Дебая при текущей температуре и при 300 К (опорная точка)
    double x_current = m_debyeTemperature / tempK;
    double x_ref = m_debyeTemperature / 300.0;

    double ratio_current = calculateDebyeFunction(x_current);
    double ratio_ref = calculateDebyeFunction(x_ref);

    // 3. Теоретическая теплоёмкость по Дебаю
    double c_theoretical = c_max * ratio_current;

    // 4. Масштабируем кривую так, чтобы при 300 К она точно совпадала со справочным значением
    double scale_factor = m_refSpecificHeat / (c_max * ratio_ref);
    double c_scaled = c_theoretical * scale_factor;

    // 5. ФИЗИЧЕСКОЕ ОГРАНИЧЕНИЕ: Теплоёмкость не может расти бесконечно.
    // Ограничиваем её значением ~1.15 * c_max (учитывая разницу между Cp и Cv при высоких T)
    double c_final = std::min(c_scaled, c_max * 1.15);

    return c_final;
}

// Геттеры
QString Material::name() const { return m_name; }
double Material::debyeTemperature() const { return m_debyeTemperature; }
double Material::molarMass() const { return m_molarMass; }
double Material::referenceSpecificHeat() const { return m_refSpecificHeat; }
QString Material::symbol() const { return m_symbol; }
double Material::densityGcm3() const { return m_density; }

// ============================================================================
// ФАБРИЧНЫЕ МЕТОДЫ ДЛЯ ВСЕХ МАТЕРИАЛОВ
// ============================================================================

Material Material::copper()
{
    return Material("Медь", "Cu", 343.0, 0.0635, 385.0, 8.96);
}

Material Material::aluminum()
{
    return Material("Алюминий", "Al", 428.0, 0.0270, 920.0, 2.70);
}

Material Material::steel()
{
    return Material("Сталь", "St", 470.0, 0.0556, 460.0, 7.85);
}

Material Material::stainlessSteel()
{
    return Material("Нерж. сталь", "SS", 450.0, 0.0555, 480.0, 7.90);
}

Material Material::lead()
{
    return Material("Свинец", "Pb", 105.0, 0.2072, 140.0, 11.34);
}

Material Material::polyethylene()
{
    return Material("Полиэтилен", "PE", 200.0, 0.0280, 2900.0, 0.92);
}

Material Material::ptfe()
{
    return Material("Фторопласт", "PTFE", 150.0, 0.1000, 1000.0, 2.20);
}

Material Material::oak1()
{
    return Material("Дуб №1", "Oak1", 220.0, 0.1450, 2400.0, 0.70);
}

Material Material::beech1()
{
    return Material("Бук №1", "Beech1", 220.0, 0.1450, 1780.0, 0.75);
}

Material Material::beech2()
{
    return Material("Бук №2", "Beech2", 220.0, 0.1450, 1780.0, 0.75);
}

Material Material::beech3()
{
    return Material("Бук №3", "Beech3", 220.0, 0.1450, 1780.0, 0.75);
}

Material Material::larch1()
{
    return Material("Лиственница №1", "Larch1", 220.0, 0.1450, 1400.0, 0.60);
}

Material Material::larch2()
{
    return Material("Лиственница №2", "Larch2", 220.0, 0.1450, 1400.0, 0.60);
}

Material Material::larch3()
{
    return Material("Лиственница №3", "Larch3", 220.0, 0.1450, 1400.0, 0.60);
}

Material Material::ash1()
{
    return Material("Ясень №1", "Ash1", 220.0, 0.1450, 1600.0, 0.70);
}

Material Material::ash2()
{
    return Material("Ясень №2", "Ash2", 220.0, 0.1450, 1600.0, 0.70);
}

Material Material::porcelain1()
{
    return Material("Электрофарфор №1 (бол.)", "Porc1", 500.0, 0.0690, 800.0, 2.30);
}

Material Material::porcelain2()
{
    return Material("Фарфор №2 (мал.)", "Porc2", 500.0, 0.0690, 800.0, 2.30);
}

Material Material::rubber()
{
    return Material("Резина", "Rubber", 150.0, 0.0681, 1420.0, 1.10);
}

Material Material::brickRed()
{
    return Material("Кирпич печной (красный)", "BrickRed", 400.0, 0.0776, 840.0, 1.80);
}

Material Material::brickYellow()
{
    return Material("Кирпич огнеупорный (жёлтый)", "BrickYel", 400.0, 0.0658, 600.0, 2.00);
}

Material Material::brickSil1()
{
    return Material("Кирпич силикатный (белый) №1", "BrickSil1", 400.0, 0.0691, 1000.0, 1.90);
}

Material Material::brickSil2()
{
    return Material("Кирпич силикатный (белый) №2", "BrickSil2", 400.0, 0.0691, 1000.0, 1.90);
}

Material Material::glass()
{
    return Material("Стекло", "Glass", 550.0, 0.0600, 840.0, 2.50);
}

Material Material::ebonite()
{
    return Material("Эбонит", "Ebonite", 150.0, 0.2260, 1400.0, 1.20);
}

Material Material::carbolite()
{
    // C_mol = 1900 * 0.231 ≈ 439
    return Material("Карболит", "Carb", 200.0, 0.2310, 1900.0, 1.30);
}

Material Material::textolite()
{
    // M оценочно ~150 г/моль -> 0.15 кг/моль
    // C_mol = 1490 * 0.15 ≈ 224
    return Material("Текстолит", "Text", 250.0, 0.1500, 1490.0, 1.35);
}

Material Material::polycarbonate()
{
    // M оценочно ~254 г/моль -> 0.254 кг/моль
    // C_mol = 1100 * 0.254 ≈ 279
    return Material("Поликарбонат", "PC", 180.0, 0.2540, 1100.0, 1.20);
}

Material Material::brass()
{
    return Material("Латунь", "Brass", 340.0, 0.0650, 400.0, 8.50);
}

Material Material::bronze()
{
    return Material("Бронза", "Bronze", 330.0, 0.0690, 400.0, 8.80);
}

Material Material::zinc()
{
    return Material("Цинк", "Zn", 235.0, 0.0654, 390.0, 7.14);
}

Material Material::tin()
{
    return Material("Олово", "Sn", 200.0, 0.1187, 230.0, 7.31);
}

Material Material::duralumin()
{
    return Material("Дюраль", "Dural", 420.0, 0.0271, 920.0, 2.80);
}

Material Material::silumin()
{
    return Material("Силумин", "Silumin", 400.0, 0.0266, 840.0, 2.70);
}

Material Material::castIron()
{
    return Material("Чугун", "CastIron", 470.0, 0.0558, 500.0, 7.20);
}

Material Material::titanium()
{
    return Material("Титан", "Ti", 420.0, 0.0479, 523.0, 4.50);
}

Material Material::tungsten()
{
    return Material("Вольфрам", "W", 400.0, 0.1838, 134.0, 19.30);
}

Material Material::molybdenum()
{
    return Material("Молибден", "Mo", 450.0, 0.0960, 261.0, 10.20);
}

Material Material::gold()
{
    return Material("Золото", "Au", 165.0, 0.1970, 129.0, 19.30);
}

Material Material::platinum()
{
    return Material("Платина", "Pt", 240.0, 0.1950, 134.0, 21.40);
}

Material Material::silver()
{
    return Material("Серебро", "Ag", 225.0, 0.1080, 234.0, 10.50);
}

Material Material::nickel()
{
    return Material("Никель", "Ni", 450.0, 0.0587, 440.0, 8.90);
}

Material Material::beryllium()
{
    return Material("Бериллий", "Be", 1440.0, 0.0090, 1824.0, 1.85);
}

Material Material::zirconium()
{
    return Material("Цирконий", "Zr", 290.0, 0.0912, 291.0, 6.50);
}

Material Material::paraffin()
{
    return Material("Парафин", "Paraffin", 100.0, 0.3500, 2500.0, 0.90);
}

// ============================================================================
// РЕЕСТР И ПОИСК
// ============================================================================

QList<Material> Material::registry()
{
    return {
        copper(),
        aluminum(),
        steel(),
        stainlessSteel(),
        lead(),
        polyethylene(),
        ptfe(),
        oak1(),
        beech1(),
        beech2(),
        beech3(),
        larch1(),
        larch2(),
        larch3(),
        ash1(),
        ash2(),
        porcelain1(),
        porcelain2(),
        rubber(),
        brickRed(),
        brickYellow(),
        brickSil1(),
        brickSil2(),
        glass(),
        ebonite(),
        carbolite(),
        textolite(),
        polycarbonate(),
        brass(),
        bronze(),
        zinc(),
        tin(),
        duralumin(),
        silumin(),
        castIron(),
        titanium(),
        tungsten(),
        molybdenum(),
        gold(),
        platinum(),
        silver(),
        nickel(),
        beryllium(),
        zirconium(),
        paraffin()
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
