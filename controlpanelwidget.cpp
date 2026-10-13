#include "controlpanelwidget.h"
#include "material.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QSlider>
#include <QRadioButton>
#include <QButtonGroup>
// QSpacerItem больше не нужен, но заголовок можно оставить, если он используется где-то еще

ControlPanelWidget::ControlPanelWidget(QWidget* parent)
    : QWidget(parent)
{
    // Устанавливаем политику размера: ширина фиксирована, высота растягивается на всё доступное пространство
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    setupUI();
    m_engine = new PhysicsEngine(this);
    setupConnections();
}

void ControlPanelWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(8);
    m_mainLayout->setContentsMargins(5, 5, 5, 5);

    // === 1. Группа "Образцы" ===
    m_sampleGroup = new QGroupBox(tr("Образцы"), this);
    QVBoxLayout* sampleMainLayout = new QVBoxLayout(m_sampleGroup);
    sampleMainLayout->setSpacing(4);

    auto createSampleRow = [this, sampleMainLayout](int index, QComboBox*& combo, QDoubleSpinBox*& spin, QLabel*& label) {
        QHBoxLayout* row = new QHBoxLayout();
        row->setContentsMargins(0, 0, 0, 0);
        row->setSpacing(5);

        QLabel* numLabel = new QLabel(QString("%1:").arg(index + 1), this);
        numLabel->setFixedWidth(20);

        combo = new QComboBox(this);
        combo->addItem(tr("Выберите"));
        for (const auto& mat : Material::registry()) {
            combo->addItem(mat.name());
        }
        combo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
        combo->setMinimumContentsLength(12);

        spin = new QDoubleSpinBox(this);
        spin->setRange(10.0, 10000.0);
        spin->setValue(300.0);
        spin->setSuffix(" г");
        spin->setFixedWidth(80);

        label = new QLabel("Θ=__, ρ=__", this);
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        label->setStyleSheet("color: #333333; font-size: 12px; font-weight: normal;");

        row->addWidget(numLabel);
        row->addWidget(combo, 1);
        row->addWidget(spin);
        row->addWidget(label, 1);

        sampleMainLayout->addLayout(row);

        m_sampleCombos.append(combo);
        m_sampleLabels.append(label);

        connect(combo, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
                this, [this, index](const QString& text) {
                    if (text == tr("Выберите")) clearSampleLabel(index);
                    else onMaterialChanged(index, text);
                });
    };

    createSampleRow(0, first_mat_combo, first_mat_spin, first_mat_label);
    createSampleRow(1, second_mat_combo, second_mat_spin, second_mat_label);
    createSampleRow(2, third_mat_combo, third_mat_spin, third_mat_label);
    createSampleRow(3, fourth_mat_combo, fourth_mat_spin, fourth_mat_label);

    m_mainLayout->addWidget(m_sampleGroup);

    // === 2. Группа "Параметры" ===
    m_envAndStartGroup = new QGroupBox(tr("Параметры"), this);
    QGridLayout* gridLayout = new QGridLayout(m_envAndStartGroup);
    gridLayout->setSpacing(5);
    gridLayout->setContentsMargins(5, 5, 5, 5);

    gridLayout->addWidget(new QLabel("T₀ среды:"), 0, 0);
    T0 = new QDoubleSpinBox(this);
    T0->setRange(-50.0, 200.0);
    T0->setValue(20.0);
    T0->setSuffix(" °C");
    T0->setFixedWidth(85);
    gridLayout->addWidget(T0, 0, 1);

    gridLayout->addWidget(new QLabel("Влажность:"), 0, 2);
    humidity = new QSpinBox(this);
    humidity->setRange(0, 100);
    humidity->setValue(70);
    humidity->setSuffix(" %");
    humidity->setFixedWidth(85);
    gridLayout->addWidget(humidity, 0, 3);

    // Параметры калориметра — делаем крупнее и контрастнее
    QLabel* calibHeader = new QLabel("Параметры калориметра:", this);
    calibHeader->setStyleSheet("font-weight: bold; font-size: 12px; color: #2c3e50;");
    gridLayout->addWidget(calibHeader, 1, 0, 1, 4);

    QLabel* consts = new QLabel("k = 5.20 Дж/(мин·К),  Ck = 210 Дж/К", this);
    consts->setStyleSheet("color: #000000; font-size: 13px; font-weight: normal;");  // Чёрный цвет, крупнее
    consts->setFont(QFont("Segoe UI", 11));  // Явно задаём шрифт покрупнее
    gridLayout->addWidget(consts, 2, 0, 1, 4);

    gridLayout->addWidget(new QLabel("T нач.:"), 3, 0);
    initialTempSpinBox = new QDoubleSpinBox(this);
    initialTempSpinBox->setRange(30.0, 10000.0);
    initialTempSpinBox->setValue(60.0);
    initialTempSpinBox->setSuffix(" °C");
    initialTempSpinBox->setFixedWidth(85);
    gridLayout->addWidget(initialTempSpinBox, 3, 1);

    initialTempSlider = new QSlider(Qt::Horizontal, this);
    initialTempSlider->setRange(30, 10000);
    initialTempSlider->setValue(60);
    gridLayout->addWidget(initialTempSlider, 3, 2, 1, 2);

    connect(initialTempSlider, &QSlider::valueChanged, initialTempSpinBox, [this](int val){
        initialTempSpinBox->setValue(val);
    });
    connect(initialTempSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), initialTempSlider, [this](double val){
        initialTempSlider->setValue(val);
    });

    gridLayout->addWidget(new QLabel("Режим:"), 4, 0);
    QWidget* radioWidget = new QWidget(this);
    QHBoxLayout* radioLayout = new QHBoxLayout(radioWidget);
    radioLayout->setContentsMargins(0, 0, 0, 0);
    radioLayout->setSpacing(10);

    radioAccelerated = new QRadioButton(tr("Ускоренный"), this);
    radioAccelerated->setToolTip("1 сек реального времени = 5 мин симуляции");
    radioReal = new QRadioButton(tr("Реальный"), this);
    radioReal->setToolTip("1 сек реального времени = 1 сек симуляции");

    radioAccelerated->setChecked(true);

    radioLayout->addWidget(radioAccelerated);
    radioLayout->addWidget(radioReal);
    radioLayout->addStretch();
    gridLayout->addWidget(radioWidget, 4, 1, 1, 3);

    m_mainLayout->addWidget(m_envAndStartGroup);

    // === 3. Группа "Действия" ===
    m_controlGroup = new QGroupBox(tr("Действия"), this);
    QVBoxLayout* manageLayout = new QVBoxLayout(m_controlGroup);
    manageLayout->setSpacing(6);

    startCoolingBtn = new QPushButton(tr("▶ Старт остывания"), this);
    startCoolingBtn->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 6px;");

    write_point = new QPushButton(tr(" Записать точку"), this);
    reset_btn = new QPushButton(tr(" Сброс"), this);
    export_btn = new QPushButton(tr("💾 Экспорт в CSV"), this);
    back_btn = new QPushButton(tr("← В главное меню"), this);

    manageLayout->addWidget(startCoolingBtn);
    manageLayout->addWidget(write_point);
    manageLayout->addWidget(reset_btn);
    manageLayout->addWidget(export_btn);
    manageLayout->addWidget(back_btn);

    m_mainLayout->addWidget(m_controlGroup);

    // === 4. Группа "Статус" ===
    m_statusGroup = new QGroupBox(tr("Статус"), this);
    m_statusGroup->setFont(QFont("Segoe UI", 10));  // Увеличиваем шрифт всей группы

    QFormLayout* statusLayout = new QFormLayout(m_statusGroup);
    statusLayout->setSpacing(6);  // Немного больше отступы
    statusLayout->setLabelAlignment(Qt::AlignRight);  // Выравнивание меток вправо

    phase = new QLabel(tr("Ожидание"), this);
    phase->setFont(QFont("Segoe UI", 10, QFont::Bold));  // Жирный шрифт
    statusLayout->addRow(tr("Фаза:"), phase);

    time = new QLabel(tr("00:00"), this);
    time->setFont(QFont("Segoe UI", 10, QFont::Bold));  // Жирный шрифт
    time->setStyleSheet("color: #2c3e50;");  // Тёмно-синий для контраста
    statusLayout->addRow(tr("Время:"), time);

    m_coolingTimeLabel = new QLabel(tr("00:00"), this);
    m_coolingTimeLabel->setFont(QFont("Segoe UI", 10, QFont::Bold));
    m_coolingTimeLabel->setStyleSheet("color: #2c3e50;");
    statusLayout->addRow(tr("Остывание:"), m_coolingTimeLabel);

    pointsLabel = new QLabel("0", this);
    pointsLabel->setFont(QFont("Segoe UI", 10, QFont::Bold));
    pointsLabel->setStyleSheet("color: #2c3e50;");
    statusLayout->addRow(tr("Точек:"), pointsLabel);

    m_mainLayout->addWidget(m_statusGroup);

    // !!! ИСПРАВЛЕНИЕ: Строка с QSpacerItem полностью удалена.
    // Ранее она была здесь: m_mainLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    // Её удаление позволяет виджетам группы "Статус" и другим элементам естественно заполнить всё доступное вертикальное пространство,
    // устраняя пустоту внизу, так как QSizePolicy::Expanding у самого ControlPanelWidget теперь работает корректно.
}

void ControlPanelWidget::setupConnections()
{
    connect(startCoolingBtn, &QPushButton::clicked, this, &ControlPanelWidget::onStartCoolingClicked);
    connect(write_point, &QPushButton::clicked, m_engine, &PhysicsEngine::recordPoint);
    connect(reset_btn, &QPushButton::clicked, m_engine, &PhysicsEngine::reset);
    connect(export_btn, &QPushButton::clicked, this, &ControlPanelWidget::exportRequested);
    connect(back_btn, &QPushButton::clicked, this, &ControlPanelWidget::backToMenuRequested);

    connect(m_engine, &PhysicsEngine::stateChanged, this, &ControlPanelWidget::onStateChanged);
    connect(m_engine, &PhysicsEngine::timeUpdated, this, &ControlPanelWidget::onTimeUpdated);
    connect(m_engine, &PhysicsEngine::coolingTimeUpdated, this, &ControlPanelWidget::onCoolingTimeUpdated);
    connect(m_engine, &PhysicsEngine::pointsCountUpdated, this, &ControlPanelWidget::onPointsCountUpdated);
}

void ControlPanelWidget::onStartCoolingClicked()
{
    bool named = false;
    for(int i = 0; i < 4; i++){
        if(m_sampleCombos[i]->currentText() != tr("Выберите") && m_checkboxStates[i]){
            named = true;
            break;
        }
    }
    if(!named){
        QMessageBox::warning(this, "Ошибка", "Выберите материал хотя бы для 1 образца!");
        return;
    }

    QVector<Sample> samples = gatherSamples();
    QVector<bool> actives;
    for(int i = 0; i < 4; i++) actives.append(samples[i].isActive);
    emit samplesStatusUpdated(actives);

    double t0 = T0->value();
    double initialTemp = initialTempSpinBox->value();
    bool isAccelerated = radioAccelerated->isChecked();

    m_engine->configure(t0, samples);
    m_engine->setTimeMode(isAccelerated);
    emit environmentTemperatureSet(t0);

    m_engine->startCooling(initialTemp);
    emit coolingStarted(initialTemp, isAccelerated);
}

void ControlPanelWidget::onMaterialChanged(int sampleIndex, const QString& materialName)
{
    if (sampleIndex < 0 || sampleIndex >= m_sampleLabels.size()) return;

    auto materialOpt = Material::findByName(materialName);
    if (!materialOpt.has_value()) return;

    const Material& mat = materialOpt.value();
    QLabel* label = m_sampleLabels[sampleIndex];
    if (!label) return;

    QString infoText = QString("Θ=%1, ρ=%2")
                           .arg(mat.debyeTemperature(), 0, 'f', 0)
                           .arg(mat.densityGcm3(), 0, 'f', 1);
    label->setText(infoText);

    emit sampleActivationRequested(sampleIndex, true);
    emit materialSymbolChanged(sampleIndex, mat.symbol());
}

void ControlPanelWidget::clearSampleLabel(int sampleIndex)
{
    if (sampleIndex < 0 || sampleIndex >= m_sampleLabels.size()) return;
    QLabel* label = m_sampleLabels[sampleIndex];
    if (label) label->setText("Θ=__, ρ=__");
    emit sampleActivationRequested(sampleIndex, false);
}

QVector<Sample> ControlPanelWidget::gatherSamples() const
{
    QVector<Sample> samples(4);
    for (int i = 0; i < 4; ++i) {
        if(m_sampleCombos[i]->currentText() != tr("Выберите")){
            QString material = m_sampleCombos[i]->currentText();
            auto matOpt = Material::findByName(material);
            if(matOpt.has_value()){
                double mass = (m_sampleCombos[i] == first_mat_combo) ? first_mat_spin->value() :
                                  (m_sampleCombos[i] == second_mat_combo) ? second_mat_spin->value() :
                                  (m_sampleCombos[i] == third_mat_combo) ? third_mat_spin->value() : fourth_mat_spin->value();
                samples[i] = Sample(i, matOpt.value(), mass);
                if(m_checkboxStates[i]) samples[i].isActive = true;
            }
        }
    }
    return samples;
}

void ControlPanelWidget::onSampleCheckboxChanged(int index, bool checked){
    m_checkboxStates[index] = checked;
}

void ControlPanelWidget::onStateChanged(ExperimentState state)
{
    switch (state) {
    case ExperimentState::Idle: phase->setText("Ожидание"); break;
    case ExperimentState::Cooling: phase->setText("Остывание"); break;
    default: phase->setText("Неизвестно"); break;
    }
}

void ControlPanelWidget::onTimeUpdated(int seconds)
{
    int minutes = seconds / 60;
    int remainingSeconds = seconds % 60;
    time->setText(QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(remainingSeconds, 2, 10, QChar('0')));
}

void ControlPanelWidget::onPointsCountUpdated(int count)
{
    pointsLabel->setText(QString::number(count));
}

void ControlPanelWidget::onCoolingTimeUpdated(double timeSec)
{
    int totalSeconds = static_cast<int>(timeSec);
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    m_coolingTimeLabel->setText(QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
}
