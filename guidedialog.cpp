#include "guidedialog.h"
#include <QFont>
#include <QScrollBar>

GuideDialog::GuideDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Практическое руководство - Лабораторная работа №3");
    setMinimumSize(1000, 750);
    setupUI();
}

GuideDialog::~GuideDialog()
{
}

void GuideDialog::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);

    // Заголовок
    titleLabel = new QLabel("ПРАКТИЧЕСКОЕ РУКОВОДСТВО К ПРОГРАММЕ", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("Arial", 16, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("QLabel { color: #2c3e50; padding: 10px; background-color: #ecf0f1; border-radius: 5px; }");
    mainLayout->addWidget(titleLabel);

    // Текстовый редактор
    textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);
    textEdit->setHtml(getGuideText());
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

QString GuideDialog::getGuideText()
{
    return R"(
    <div style='font-family: Arial, sans-serif; line-height: 1.8;'>

    <h1 style='color: #2c3e50; text-align: center; border-bottom: 3px solid #3498db; padding-bottom: 10px;'>
        ПРАКТИЧЕСКОЕ РУКОВОДСТВО К ПРОГРАММЕ
    </h1>

    <p style='background-color: #f8f9fa; padding: 10px; border-left: 4px solid #3498db; margin-bottom: 20px;'>
        <b>Назначение:</b> данное руководство описывает порядок работы с программой-симулятором
        лабораторной работы №3 «Теплоёмкость кристаллических тел».
    </p>

    <h2 style='color: #2c3e50; border-bottom: 2px solid #3498db; margin-top: 30px; margin-bottom: 15px;'>1. ЗАПУСК И ГЛАВНОЕ МЕНЮ</h2>

    <p style='margin-bottom: 15px;'>При запуске программы открывается главное окно с тремя кнопками:</p>
    <ul style='margin-bottom: 15px;'>
        <li style='margin-bottom: 8px;'><b>«Методические указания»</b> — открывает теоретическую часть работы.</li>
        <li style='margin-bottom: 8px;'><b>«Практическое руководство»</b> — эта справка.</li>
        <li style='margin-bottom: 8px;'><b>«Приступить к выполнению»</b> — запускает рабочее окно эксперимента.</li>
    </ul>

    <h2 style='color: #2c3e50; border-bottom: 2px solid #3498db; margin-top: 30px; margin-bottom: 15px;'>2. ИНТЕРФЕЙС РАБОЧЕГО ОКНА</h2>

    <p style='margin-bottom: 15px;'>Программа состоит из четырёх основных зон:</p>
    <ol style='margin-bottom: 15px;'>
        <li style='margin-bottom: 8px;'><b>Схема установки</b> (слева по центру) — визуализация калориметра с подписями элементов.</li>
        <li style='margin-bottom: 8px;'><b>Подробная схема калориметра</b> (слева внизу) — справочный рисунок с обозначениями.</li>
        <li style='margin-bottom: 8px;'><b>Панель управления</b> (справа) — настройка образцов, параметров среды и кнопки управления.</li>
        <li style='margin-bottom: 8px;'><b>Протокол измерений</b> (внизу) — таблица для автоматической записи данных.</li>
    </ol>

    <h2 style='color: #2c3e50; border-bottom: 2px solid #3498db; margin-top: 30px; margin-bottom: 15px;'>3. ПОШАГОВОЕ ВЫПОЛНЕНИЕ ЭКСПЕРИМЕНТА</h2>

    <h3 style='color: #34495e; margin-top: 25px; margin-bottom: 12px;'>3.1. Выбор образцов</h3>
    <ol style='margin-bottom: 15px;'>
        <li style='margin-bottom: 8px;'>В группе <b>«Настройка образцов»</b> выберите материал для нужных образцов из выпадающего списка.</li>
        <li style='margin-bottom: 8px;'>При необходимости измените массу (по умолчанию 300 г). Ниже отобразятся справочные данные:
            температура Дебая и плотность.</li>
        <li style='margin-bottom: 8px;'>После выбора материала разблокируется чекбокс <b>«Обр. X»</b>. Отметьте образцы,
            которые будут участвовать в эксперименте.
            <div style='background-color: #fff3cd; padding: 10px; border-left: 4px solid #ffc107; margin: 10px 0;'>
                ⚠️ <b>Внимание:</b> если нажать «Нагрев» без выбранных образцов, программа выдаст предупреждение.
            </div>
        </li>
    </ol>

    <h3 style='color: #34495e; margin-top: 25px; margin-bottom: 12px;'>3.2. Режим отображения температур</h3>
    <p style='margin-bottom: 15px;'>Чекбокс <b>«ΔT»</b> переключает режим показа:</p>
    <ul style='margin-bottom: 15px;'>
        <li style='margin-bottom: 8px;'><b>Выключен:</b> отображается абсолютная температура образца.</li>
        <li style='margin-bottom: 8px;'><b>Включён:</b> отображается разность T − T₀ (удобно для анализа остывания).
            Заголовки в таблице обновляются автоматически.</li>
    </ul>

    <h3 style='color: #34495e; margin-top: 25px; margin-bottom: 12px;'>3.3. Параметры среды</h3>
    <p style='margin-bottom: 15px;'>В группе <b>«Параметры среды»</b> задайте:</p>
    <ul style='margin-bottom: 15px;'>
        <li style='margin-bottom: 8px;'>Температуру окружающей среды (по умолчанию 20 °C). При изменении начальная температура
            образцов обновится автоматически.</li>
        <li style='margin-bottom: 8px;'>Влажность (справочный параметр, не влияет на расчёт).</li>
    </ul>
    <p style='margin-bottom: 15px;'>Ниже указаны константы калориметра, используемые в расчётах:</p>
    <div style='background-color: #ecf0f1; padding: 15px; border-left: 4px solid #3498db; margin: 15px 0;'>
        k = 5.20 Дж/(мин·К)<br>
        C<sub>k</sub> = 210 Дж/К
    </div>

    <h3 style='color: #34495e; margin-top: 25px; margin-bottom: 12px;'>3.4. Управление экспериментом</h3>
    <table style='border-collapse: collapse; width: 100%; margin: 15px 0; background-color: white;'>
        <thead>
            <tr style='background-color: #3498db; color: white;'>
                <th style='border: 1px solid #bdc3c7; padding: 12px; width: 30%; text-align: left;'>Кнопка</th>
                <th style='border: 1px solid #bdc3c7; padding: 12px; width: 70%; text-align: left;'>Действие</th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <td style='border: 1px solid #bdc3c7; padding: 10px;'><b>«Нагрев»</b></td>
                <td style='border: 1px solid #bdc3c7; padding: 10px;'>Запускает линейный нагрев активных образцов (1 °C/с).
                    Чтобы остановить нагрев, перейдите к следующему этапу, нажав кнопку "Термостатирование".</td>
            </tr>
            <tr style='background-color: #f8f9fa;'>
                <td style='border: 1px solid #bdc3c7; padding: 10px;'><b>«Термостатирование»</b></td>
                <td style='border: 1px solid #bdc3c7; padding: 10px;'>Останавливает нагрев. В симуляции символизирует
                    выдержку для равномерного прогрева.</td>
            </tr>
            <tr>
                <td style='border: 1px solid #bdc3c7; padding: 10px;'><b>«Записать точку»</b></td>
                <td style='border: 1px solid #bdc3c7; padding: 10px;'>Фиксирует первую точку остывания в таблицу
                    и запускает таймер фазы охлаждения. Последующие нажатия добавляют новые строки данных.</td>
            </tr>
            <tr style='background-color: #f8f9fa;'>
                <td style='border: 1px solid #bdc3c7; padding: 10px;'><b>«Сброс»</b></td>
                <td style='border: 1px solid #bdc3c7; padding: 10px;'>Очищает таблицу, таймеры и счётчики.
                    Выбранные материалы и массы сохраняются.</td>
            </tr>
            <tr>
                <td style='border: 1px solid #bdc3c7; padding: 10px;'><b>«Экспорт в XLS»</b></td>
                <td style='border: 1px solid #bdc3c7; padding: 10px;'>Сохраняет протокол в табличном файле.
                    Откроется диалог выбора пути и имени файла.</td>
            </tr>
            <tr style='background-color: #f8f9fa;'>
                <td style='border: 1px solid #bdc3c7; padding: 10px;'><b>«В главное меню»</b></td>
                <td style='border: 1px solid #bdc3c7; padding: 10px;'>Возвращает в главное окно программы.
                    Эксперимент будет сброшен.</td>
            </tr>
        </tbody>
    </table>

    <h2 style='color: #2c3e50; border-bottom: 2px solid #3498db; margin-top: 30px; margin-bottom: 15px;'>4. ГРУППА «СТАТУС»</h2>
    <p style='margin-bottom: 15px;'>В нижней части панели управления отображается текущая информация:</p>
    <ul style='margin-bottom: 15px;'>
        <li style='margin-bottom: 8px;'><b>Фаза:</b> текущий этап эксперимента (Ожидание / Нагрев / Термостат / Остывание).</li>
        <li style='margin-bottom: 8px;'><b>Время:</b> общее время работы с начала эксперимента.</li>
        <li style='margin-bottom: 8px;'><b>Время остывания:</b> таймер фазы охлаждения (сбрасывается при каждой записи точки).</li>
        <li style='margin-bottom: 8px;'><b>Точек:</b> количество записанных строк в таблицу.</li>
    </ul>

    <h2 style='color: #2c3e50; border-bottom: 2px solid #3498db; margin-top: 30px; margin-bottom: 15px;'>5. ЗАВЕРШЕНИЕ РАБОТЫ</h2>
    <ol style='margin-bottom: 15px;'>
        <li style='margin-bottom: 8px;'>Убедитесь, что данные сохранены через <b>«Экспорт в XLS»</b>.</li>
        <li style='margin-bottom: 8px;'>Откройте файл в Excel или LibreOffice Calc для:
            <ul style='margin-top: 8px;'>
                <li style='margin-bottom: 5px;'>Построения графика ln(T−T₀) от времени.</li>
                <li style='margin-bottom: 5px;'>Расчёта коэффициента α методом наименьших квадратов.</li>
                <li style='margin-bottom: 5px;'>Вычисления удельной и молярной теплоёмкости по формулам методических указаний.</li>
            </ul>
        </li>
        <li style='margin-bottom: 8px;'>Закройте программу через стандартный крестик окна или кнопку «В главное меню» → «Выход».</li>
    </ol>

    <hr style='margin: 30px 0; border: none; border-top: 2px solid #bdc3c7;'>

    <p style='text-align: center; color: #7f8c8d; font-size: 10px;'>
        Программа-симулятор лабораторной работы №3 • УрФУ • 2026
    </p>

    </div>
    )";
}
