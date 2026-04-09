#include "methodologydialog.h"
#include <QFont>
#include <QScrollBar>

MethodologyDialog::MethodologyDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Методические указания - Лабораторная работа №3");
    setMinimumSize(1000, 750);
    setupUI();
}

MethodologyDialog::~MethodologyDialog()
{
}

void MethodologyDialog::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);

    // Заголовок
    titleLabel = new QLabel("ТЕПЛОЁМКОСТЬ КРИСТАЛЛИЧЕСКИХ ТЕЛ", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("Arial", 16, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("QLabel { color: #2c3e50; padding: 10px; background-color: #ecf0f1; border-radius: 5px; }");
    mainLayout->addWidget(titleLabel);

    // Текстовый редактор с прокруткой
    textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);
    textEdit->setHtml(getMethodologyText());
    textEdit->setFont(QFont("Arial", 11));
    textEdit->setStyleSheet(
        "QTextEdit { "
        "    background-color: white; "
        "    border: 2px solid #bdc3c7; "
        "    border-radius: 5px; "
        "    padding: 15px; "
        "} "
        "QScrollBar:vertical { "
        "    background: #ecf0f1; "
        "    width: 12px; "
        "    border-radius: 6px; "
        "} "
        "QScrollBar::handle:vertical { "
        "    background: #95a5a6; "
        "    border-radius: 6px; "
        "} "
        "QScrollBar::handle:vertical:hover { "
        "    background: #7f8c8d; "
        "}"
        );
    mainLayout->addWidget(textEdit);

    // Кнопка закрытия
    closeButton = new QPushButton("Закрыть", this);
    closeButton->setMaximumWidth(150);
    closeButton->setMinimumHeight(40);
    QFont buttonFont("Arial", 11, QFont::Bold);
    closeButton->setFont(buttonFont);
    closeButton->setStyleSheet(
        "QPushButton { "
        "    background-color: #3498db; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 5px; "
        "    padding: 8px; "
        "} "
        "QPushButton:hover { "
        "    background-color: #2980b9; "
        "} "
        "QPushButton:pressed { "
        "    background-color: #1f6aa5; "
        "}"
        );
    connect(closeButton, &QPushButton::clicked, this, &QDialog::close);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
}

QString MethodologyDialog::getMethodologyText()
{
    return R"(
    <div style='font-family: Arial, sans-serif; line-height: 1.6;'>

    <h1 style='color: #2c3e50; text-align: center; border-bottom: 3px solid #3498db; padding-bottom: 10px;'>
        ЛАБОРАТОРНАЯ РАБОТА №3<br>ТЕПЛОЁМКОСТЬ КРИСТАЛЛИЧЕСКИХ ТЕЛ
    </h1>

    <p style='background-color: #f8f9fa; padding: 10px; border-left: 4px solid #3498db;'>
        <b>Цели работы:</b> получение представления о микроскопической теории теплоёмкости
        кристаллических тел, ознакомление с установкой для измерения теплоёмкости и
        измерение теплоёмкости различных кристаллических тел.
    </p>

    <h2 style='color: #2c3e50; border-bottom: 2px solid #3498db;'>1. ПОНЯТИЕ ТЕПЛОЁМКОСТИ. ВИДЫ ТЕПЛОЁМКОСТЕЙ</h2>

    <p>В соответствии с терминологией теплоёмкость – производная от количества теплоты
    по температуре в каком-либо термодинамическом процессе:</p>

    <div style='background-color: #ecf0f1; padding: 15px; border-left: 4px solid #3498db; margin: 10px 0;'>
        c<sub>α</sub> = (δQ/∂T)<sub>α</sub>
    </div>

    <p>где индексом α обозначен некоторый термодинамический процесс.</p>

    <p><b>Изохорная теплоёмкость</b> (объем V = const):</p>
    <div style='background-color: #ecf0f1; padding: 15px; border-left: 4px solid #3498db; margin: 10px 0;'>
        c<sub>V</sub> = (δQ/∂T)<sub>V</sub>
    </div>

    <p><b>Изобарная теплоёмкость</b> (давление p = const):</p>
    <div style='background-color: #ecf0f1; padding: 15px; border-left: 4px solid #3498db; margin: 10px 0;'>
        c<sub>p</sub> = (δQ/∂T)<sub>p</sub>
    </div>

    <p>На практике применяют следующие виды теплоёмкостей:</p>
    <ul>
        <li><b>Удельная теплоемкость</b> – теплоемкость единицы массы вещества</li>
        <li><b>Мольная теплоемкость</b> – теплоемкость одного моля вещества</li>
        <li><b>Объёмная теплоемкость</b> – теплоемкость единицы объёма вещества</li>
    </ul>

    <h2 style='color: #2c3e50; border-bottom: 2px solid #3498db;'>2. ТЕОРИИ ТЕПЛОЁМКОСТИ КРИСТАЛЛИЧЕСКИХ ТЕЛ</h2>

    <h3 style='color: #34495e;'>2.1. Классическая теория</h3>

    <p>В первом приближении считаем, что атомы в узлах кристаллической решётки
    совершают малые тепловые колебания около положений равновесия с одинаковой частотой ω.
    Каждый атом имеет три колебательных степени свободы, при этом весь кристалл,
    состоящий из N атомов, можно рассматривать как совокупность <b>3N линейных гармонических осцилляторов</b>.</p>

    <p>Энергия кристалла E выражается соотношением:</p>
    <div style='background-color: #ecf0f1; padding: 15px; border-left: 4px solid #3498db; margin: 10px 0;'>
        E = 3Nε̄
    </div>
    <p>где ε̄ – средняя энергия одного осциллятора.</p>

    <p>В классическом приближении полагают ε̄ = kT (k – постоянная Больцмана),
    с учётом этого:</p>
    <div style='background-color: #ecf0f1; padding: 15px; border-left: 4px solid #3498db; margin: 10px 0;'>
        E = 3NkT
    </div>

    <p>Откуда, в расчёте на один моль:</p>
    <div style='background-color: #ecf0f1; padding: 15px; border-left: 4px solid #3498db; margin: 10px 0;'>
        c<sub>V</sub> = 3N<sub>A</sub>k = 3R
    </div>
    <p>где N<sub>A</sub> – число Авогадро, R – универсальная газовая постоянная.</p>

    <p style='background-color: #fff3cd; padding: 10px; border-left: 4px solid #ffc107;'>
        <b>Закон Дюлонга и Пти:</b> молярная теплоемкость почти всех кристаллических тел
        не зависит от температуры и рода вещества и равна <b>3R</b>.
    </p>

    <h3 style='color: #34495e;'>2.2. Модель Эйнштейна</h3>

    <p>Эйнштейн предложил использовать квантовомеханический аналог для средней энергии
    одного осциллятора:</p>
    <div style='background-color: #ecf0f1; padding: 15px; border-left: 4px solid #3498db; margin: 10px 0;'>
        ε̄ = ℏω / [exp(ℏω/kT) - 1]
    </div>
    <p>где ℏ – постоянная Планка.</p>

    <p>Энергия E системы из N линейных осцилляторов:</p>
    <div style='background-color: #ecf0f1; padding: 15px; border-left: 4px solid #3498db; margin: 10px 0;'>
        E = Nℏω / [exp(ℏω/kT) - 1]
    </div>

    <p>Теплоёмкость в расчёте на один моль:</p>
    <div style='background-color: #ecf0f1; padding: 15px; border-left: 4px solid #3498db; margin: 10px 0;'>
        c<sub>V</sub> = 3R(ℏω/kT)² · exp(ℏω/kT) / [exp(ℏω/kT) - 1]²
    </div>

    <p>При температурах T >> ℏω/k теплоёмкость c<sub>V</sub> ≈ 3R (закон Дюлонга и Пти).
    При низких температурах: c<sub>V</sub> ~ exp(-ℏω/kT).</p>

    <h3 style='color: #34495e;'>2.3. Теория Дебая</h3>

    <p>Дебай сопоставил осцилляторам не колебания отдельных атомов, а
    <b>собственные упругие колебания твёрдого тела как целого</b>. Он предложил спектр
    колебаний атомов твёрдого тела трактовать как спектр упругих колебаний
    однородного твёрдого тела.</p>

    <p>Энергия кристалла:</p>
    <div style='background-color: #ecf0f1; padding: 15px; border-left: 4px solid #3498db; margin: 10px 0;'>
        E = ∫₀^ω<sub>D</sub> ε(ω)f(ω)dω
    </div>
    <p>где f(ω) – функция распределения упругих колебаний по спектру частот,
    ω<sub>D</sub> – предельная частота.</p>

    <p>После преобразований:</p>
    <div style='background-color: #ecf0f1; padding: 15px; border-left: 4px solid #3498db; margin: 10px 0;'>
        E = 9RT(T/Θ)³ ∫₀^(Θ/T) x³/(eˣ - 1) dx
    </div>
    <p>где Θ – <b>температура Дебая</b>, x = ℏω/kT.</p>

    <p>Теплоёмкость:</p>
    <div style='background-color: #ecf0f1; padding: 15px; border-left: 4px solid #3498db; margin: 10px 0;'>
        c<sub>V</sub> = 9R(T/Θ)³ ∫₀^(Θ/T) x⁴eˣ/(eˣ - 1)² dx
    </div>

    <h4 style='color: #5d6d7e;'>Предельные случаи:</h4>
    <ul>
        <li><b>При T >> Θ:</b> c<sub>V</sub> → 3R (классический предел)</li>
        <li><b>При T << Θ:</b> закон T³ Дебая:</li>
    </ul>
    <div style='background-color: #ecf0f1; padding: 15px; border-left: 4px solid #3498db; margin: 10px 0;'>
        c<sub>V</sub> ≈ (12π⁴/5)R(T/Θ)³ ≈ 234R(T/Θ)³
    </div>

    <h4 style='color: #5d6d7e;'>Таблица 2.1. Температура Дебая Θ для некоторых веществ</h4>
    <table style='border-collapse: collapse; width: 100%; margin: 10px 0; background-color: white;'>
        <tr style='background-color: #3498db; color: white;'>
            <th style='border: 1px solid #bdc3c7; padding: 10px;'>Вещество</th>
            <th style='border: 1px solid #bdc3c7; padding: 10px;'>Бериллий</th>
            <th style='border: 1px solid #bdc3c7; padding: 10px;'>Никель</th>
            <th style='border: 1px solid #bdc3c7; padding: 10px;'>Свинец</th>
            <th style='border: 1px solid #bdc3c7; padding: 10px;'>Медь</th>
            <th style='border: 1px solid #bdc3c7; padding: 10px;'>Железо</th>
            <th style='border: 1px solid #bdc3c7; padding: 10px;'>Алюминий</th>
        </tr>
        <tr>
            <td style='border: 1px solid #bdc3c7; padding: 8px; text-align: center;'><b>Θ, К</b></td>
            <td style='border: 1px solid #bdc3c7; padding: 8px; text-align: center;'>1440</td>
            <td style='border: 1px solid #bdc3c7; padding: 8px; text-align: center;'>450</td>
            <td style='border: 1px solid #bdc3c7; padding: 8px; text-align: center;'>105</td>
            <td style='border: 1px solid #bdc3c7; padding: 8px; text-align: center;'>343</td>
            <td style='border: 1px solid #bdc3c7; padding: 8px; text-align: center;'>420</td>
            <td style='border: 1px solid #bdc3c7; padding: 8px; text-align: center;'>428</td>
        </tr>
    </table>

    <h2 style='color: #2c3e50; border-bottom: 2px solid #3498db;'>3. ЭКСПЕРИМЕНТАЛЬНАЯ УСТАНОВКА</h2>

    <p>Экспериментальная установка состоит из <b>вакуумного калориметра</b> и
    <b>цифрового термометра</b>. В качестве вакуумного калориметра используется
    колба металлического термоса.</p>

    <p style='background-color: #e8f8f5; padding: 10px; border-left: 4px solid #1abc9c;'>
        <b>Принцип измерения:</b> измерение теплоёмкости производится путём измерения
        температуры образца в процессе его остывания в калориметре.
    </p>

    <h3 style='color: #34495e;'>3.1. Вывод расчётных формул</h3>

    <p>Теплообмен содержимого термоса с окружающей средой описывается соотношением:</p>
    <div style='background-color: #ecf0f1; padding: 15px; border-left: 4px solid #3498db; margin: 10px 0;'>
        C·dT/dt = -k(T - T₀)
    </div>
    <p>где:</p>
    <ul>
        <li>C – теплоёмкость содержимого калориметра</li>
        <li>k – коэффициент теплоотдачи</li>
        <li>T – температура содержимого калориметра</li>
        <li>T₀ – температура окружающей среды</li>
    </ul>

    <p>Теплоёмкость можно представить как:</p>
    <div style='background-color: #ecf0f1; padding: 15px; border-left: 4px solid #3498db; margin: 10px 0;'>
        C = C<sub>об</sub> + C<sub>k</sub>
    </div>
    <p>где C<sub>об</sub> – теплоёмкость образца, C<sub>k</sub> – теплоёмкость калориметра.</p>

    <p>Решение уравнения имеет вид:</p>
    <div style='background-color: #ecf0f1; padding: 15px; border-left: 4px solid #3498db; margin: 10px 0;'>
        ln(T - T₀) = ln(T₁ - T₀) - αt
    </div>
    <p>где α = k/(C<sub>об</sub> + C<sub>k</sub>)</p>

    <p>Теплоёмкость образца:</p>
    <div style='background-color: #ecf0f1; padding: 15px; border-left: 4px solid #3498db; margin: 10px 0;'>
        C<sub>об</sub> = k/α - C<sub>k</sub>
    </div>

    <p>Удельная и молярная теплоёмкости:</p>
    <div style='background-color: #ecf0f1; padding: 15px; border-left: 4px solid #3498db; margin: 10px 0;'>
        c<sub>уд</sub> = C<sub>об</sub>/m<br>
        C<sub>моль</sub> = c<sub>уд</sub>·M
    </div>
    <p>где m – масса образца, M – молярная масса.</p>

    <h3 style='color: #34495e;'>3.2. Параметры калориметра</h3>

    <p>Параметры калориметра k и C<sub>k</sub> определяются из серии экспериментов
    с водой и составляют:</p>
    <div style='background-color: #fff3cd; padding: 15px; border-left: 4px solid #ffc107; margin: 10px 0;'>
        k = (5,20 ± 0,03) Дж/(мин·К)<br>
        C<sub>k</sub> = (210 ± 60) Дж
    </div>

    <h2 style='color: #2c3e50; border-bottom: 2px solid #3498db;'>4. МЕТОДИКА ПРОВЕДЕНИЯ ЭКСПЕРИМЕНТА</h2>

    <h3 style='color: #34495e;'>4.1. Порядок измерений</h3>

    <ol>
        <li>Измерить и записать начальную температуру и влажность с помощью
        термометра-гигрометра T₀.</li>

        <li>Записать показания ничтометра.</li>

        <li>Поместить образцы в калориметр с помощью держателя.</li>

        <li>Наполнить калориметр водой до верхнего уровня образцов,
        установить температуру нагрева <b>100 °С</b>.</li>

        <li>При закипании воды установить температуру термостатирования <b>60 °С</b>.</li>

        <li>После выдержки (2–5 минут) вынуть образцы, вылить воду,
        осушить образцы и калориметр салфеткой.</li>

        <li>Установить в калориметр дополнительную теплоизоляцию.</li>

        <li>Поместить образцы в калориметр, ввести датчики термометров T₁–T₄,
        поместить датчик T₀, закрыть крышкой.</li>

        <li>Записать показания T₀ (температуру T₀₁ и влажность).</li>

        <li>Через <b>5 минут</b> записать текущее время (чч:мм) и температуры
        по термометрам T₁–T₄.</li>

        <li>Далее через каждые <b>5 минут</b> проделать ещё <b>8–9 измерений</b>.</li>

        <li>Записать конечные показания T₀ (температуру T₀₂ и влажность).</li>
    </ol>

    <h3 style='color: #34495e;'>4.2. Обработка результатов</h3>

    <ol>
        <li>Обработать результаты методом <b>наименьших квадратов</b> по зависимости:
        <div style='background-color: #ecf0f1; padding: 10px; margin: 5px 0;'>
            ln(T - T₀) = ln(T₁ - T₀) - αt
        </div>
        Получить значения ln(T₁–T₀) и α.</li>

        <li>Найти теплоёмкость образца:
        <div style='background-color: #ecf0f1; padding: 10px; margin: 5px 0;'>
            C<sub>об</sub> = k/α - C<sub>k</sub>
        </div>
        </li>

        <li>Построить зависимость ln(T–T₀) от времени t.</li>

        <li>Рассчитать удельную и молярную теплоёмкости:
        <div style='background-color: #ecf0f1; padding: 10px; margin: 5px 0;'>
            c<sub>уд</sub> = C<sub>об</sub>/m<br>
            C<sub>моль</sub> = c<sub>уд</sub>·M
        </div>
        </li>

        <li>Оценить погрешности по формулам:
        <div style='background-color: #ecf0f1; padding: 10px; margin: 5px 0;'>
            δC<sub>об</sub> = C<sub>об</sub>√[(δα/α)² + (δk/k)² + (δC<sub>k</sub>/C<sub>k</sub>)²]<br>
            δc<sub>уд</sub> = c<sub>уд</sub>√[(δC<sub>об</sub>/C<sub>об</sub>)² + (δm/m)²]<br>
            δC<sub>моль</sub> = C<sub>моль</sub>√[(δc<sub>уд</sub>/c<sub>уд</sub>)² + (δM/M)²]
        </div>
        </li>

        <li>Сравнить с теорией Дебая и справочными данными.</li>
    </ol>

    <h2 style='color: #2c3e50; border-bottom: 2px solid #3498db;'>5. ТРЕБОВАНИЯ К ОТЧЁТУ</h2>

    <ul>
        <li>Оформить отчёт в соответствии с требованиями <b>ГОСТ 7.32-2001</b></li>
        <li>Включить заполненный протокол измерений</li>
        <li>Представить графики и расчёты</li>
        <li>Сравнить результаты с теоретическими значениями</li>
        <li>Сдать отчёт преподавателю до начала следующей работы</li>
    </ul>

    <div style='background-color: #e8f8f5; padding: 15px; border: 2px solid #1abc9c; border-radius: 5px; margin-top: 20px;'>
        <h3 style='color: #16a085; margin-top: 0;'>📝 Примечание</h3>
        <p>Для обработки эксперимента можно использовать электронную таблицу Excel.
        Измерения проводятся при температурах больше 290 К, где теплоёмкость слабо
        зависит от температуры (согласно теории Дебая).</p>
    </div>

    <hr style='margin: 30px 0; border: none; border-top: 2px solid #bdc3c7;'>

    <h2 style='color: #2c3e50;'>📚 БИБЛИОГРАФИЧЕСКИЙ СПИСОК</h2>
    <ol>
        <li>Киттель Ч. Элементарная физика твёрдого тела. – М.: Наука, 1978. – 792 с.</li>
        <li>Займан Дж. Принципы теории твёрдого тела. – М.: Мир, 1974. – 472 с.</li>
        <li>Лейбфрид Г. Микроскопическая теория механических и тепловых свойств кристаллов. –
        М.: Физматгиз, 1963. – 363 с.</li>
        <li>Таблицы физических величин: справочник / под общ. ред. акад. И.К. Кикоина. –
        М.: Атомиздат, 1976. – 1008 с.</li>
    </ol>

    </div>
    )";
}
