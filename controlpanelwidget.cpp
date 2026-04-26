#include "controlpanelwidget.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>

ControlPanelWidget::ControlPanelWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
    setupConnections();
}

void ControlPanelWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);

    // === Группа "Настройка образцов" ===
    m_sampleGroup = new QGroupBox(tr("Настройка образцов"), this);
    QFormLayout* sampleLayout = new QFormLayout(m_sampleGroup);

    // --- Образец 1 ---
    first_mat_combo = new QComboBox(this);
    first_mat_combo->addItem("Выберите материал");
    first_mat_combo->setFixedHeight(20);
    first_mat_combo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    first_mat_spin = new QDoubleSpinBox(this);
    first_mat_spin->setRange(0.1, 10000.0);  // Расширил диапазон до 10000
    first_mat_spin->setValue(100.0);
    first_mat_spin->setDecimals(2);
    first_mat_spin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    first_mat_spin->setFixedWidth(120);  // Ширины хватит для "10000.00"
    first_mat_spin->setFixedHeight(20);

    first_mat_label = new QLabel("Θ=___ К, ρ=___ г/см³", this);
    first_mat_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    first_mat_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    sampleLayout->addRow(tr("Обр. 1:"), first_mat_combo);
    sampleLayout->addRow(tr("Масса, г:"), first_mat_spin);
    sampleLayout->addRow("", first_mat_label);

    // --- Образец 2 ---
    second_mat_combo = new QComboBox(this);
    second_mat_combo->addItem("Выберите материал");
    second_mat_combo->setFixedHeight(20);
    second_mat_combo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    second_mat_spin = new QDoubleSpinBox(this);
    second_mat_spin->setRange(0.1, 10000.0);
    second_mat_spin->setValue(100.0);
    second_mat_spin->setDecimals(2);
    second_mat_spin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    second_mat_spin->setFixedWidth(120);
    second_mat_spin->setFixedHeight(20);

    second_mat_label = new QLabel("Θ=___ К, ρ=___ г/см³", this);
    second_mat_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    second_mat_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    sampleLayout->addRow(tr("Обр. 2:"), second_mat_combo);
    sampleLayout->addRow(tr("Масса, г:"), second_mat_spin);
    sampleLayout->addRow("", second_mat_label);

    // --- Образец 3 ---
    third_mat_combo = new QComboBox(this);
    third_mat_combo->addItem("Выберите материал");
    third_mat_combo->setFixedHeight(20);
    third_mat_combo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    third_mat_spin = new QDoubleSpinBox(this);
    third_mat_spin->setRange(0.1, 10000.0);
    third_mat_spin->setValue(100.0);
    third_mat_spin->setDecimals(2);
    third_mat_spin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    third_mat_spin->setFixedWidth(120);
    third_mat_spin->setFixedHeight(20);

    third_mat_label = new QLabel("Θ=___ К, ρ=___ г/см³", this);
    third_mat_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    third_mat_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    sampleLayout->addRow(tr("Обр. 3:"), third_mat_combo);
    sampleLayout->addRow(tr("Масса, г:"), third_mat_spin);
    sampleLayout->addRow("", third_mat_label);

    // --- Образец 4 ---
    fourth_mat_combo = new QComboBox(this);
    fourth_mat_combo->addItem("Выберите материал");
    fourth_mat_combo->setFixedHeight(20);
    fourth_mat_combo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    fourth_mat_spin = new QDoubleSpinBox(this);
    fourth_mat_spin->setRange(0.1, 10000.0);
    fourth_mat_spin->setValue(100.0);
    fourth_mat_spin->setDecimals(2);
    fourth_mat_spin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    fourth_mat_spin->setFixedWidth(120);
    fourth_mat_spin->setFixedHeight(20);

    fourth_mat_label = new QLabel("Θ=___ К, ρ=___ г/см³", this);
    fourth_mat_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    fourth_mat_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    sampleLayout->addRow(tr("Обр. 4:"), fourth_mat_combo);
    sampleLayout->addRow(tr("Масса, г:"), fourth_mat_spin);
    sampleLayout->addRow("", fourth_mat_label);

    m_mainLayout->addWidget(m_sampleGroup);


    // === Группа "Параметры среды" ===
    m_envGroup = new QGroupBox(tr("Параметры среды"), this);

    // ИСПРАВЛЕНИЕ: Используем QFormLayout, чтобы лейбл и поле были в одну строку
    QFormLayout* envLayout = new QFormLayout(m_envGroup);

    T0 = new QDoubleSpinBox(this);
    T0->setRange(-50.0, 200.0);
    T0->setSingleStep(1.0);
    T0->setDecimals(1);
    T0->setValue(20.0);
    // Фиксируем ширину, чтобы спинбокс не растягивался на всю панель
    T0->setFixedWidth(100);

    // Добавляем в layout: [Метка] [Поле]
    envLayout->addRow(tr("Температура среды, °C:"), T0);

    humidity = new QSpinBox(this);
    humidity->setRange(0, 100);
    humidity->setSingleStep(5);
    humidity->setValue(70);
    humidity->setFixedWidth(100); // Фиксируем ширину

    envLayout->addRow(tr("Влажность, %:"), humidity);

    QLabel* calibHeader = new QLabel(tr("Параметры калориметра"), this);
    QFont headerFont = calibHeader->font();
    headerFont.setBold(true);
    calibHeader->setFont(headerFont);

    envLayout->addRow("", calibHeader);

    consts = new QLabel(this);
    consts->setText(tr("k = 5.20 Дж/(мин·К)\nCk = 210 Дж/К"));
    consts->setFont(QFont("Consolas", 9));

    envLayout->addRow("", consts);

    m_mainLayout->addWidget(m_envGroup);


    // === Группа "Управление" ===
    m_controlGroup = new QGroupBox(tr("Управление"), this);
    QVBoxLayout* manageLayout = new QVBoxLayout(m_controlGroup);

    nagrev = new QPushButton(tr("Нагрев"), this);
    termostat = new QPushButton(tr("Термостатирование"), this);
    stop = new QPushButton(tr("Стоп"), this);
    write_point = new QPushButton(tr("Записать точку"), this);
    export_btn = new QPushButton(tr("Экспортировать в XLS"), this);
    start = new QPushButton(tr("Старт"), this);

    manageLayout->addWidget(start);
    manageLayout->addWidget(stop);
    manageLayout->addWidget(nagrev);
    manageLayout->addWidget(termostat);
    manageLayout->addWidget(write_point);
    manageLayout->addWidget(export_btn);

    m_mainLayout->addWidget(m_controlGroup);


    // === Группа "Статус" ===
    m_statusGroup = new QGroupBox(tr("Статус"), this);
    QFormLayout* statusLayout = new QFormLayout(m_statusGroup);

    phase = new QLabel(tr("Ожидание"), this);
    statusLayout->addRow(tr("Фаза:"), phase);

    time = new QLabel(tr("00:00"), this);
    statusLayout->addRow(tr("Время:"), time);

    auto* pointsLabel = new QLabel("0", this);
    statusLayout->addRow(tr("Точек:"), pointsLabel);

    m_mainLayout->addWidget(m_statusGroup);

    m_mainLayout->addStretch();

    setMinimumWidth(280);
}

void ControlPanelWidget::setupConnections()
{
    // Этот метод пока оставьте пустым.
    // Позже здесь будут связи кнопок со слотами через connect()
    // Пример (не копируйте, разберём позже):
    // connect(m_btnHeat, &QPushButton::clicked, this, &ControlPanelWidget::onHeatClicked);
}
