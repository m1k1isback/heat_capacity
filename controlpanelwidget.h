#pragma once

// Forward declarations для ускорения компиляции (используются только как указатели)
class QVBoxLayout;
class QGroupBox;
class QFormLayout;
class QComboBox;
class QDoubleSpinBox;
class QSpinBox;
class QPushButton;
class QLabel;

// Необходимые заголовки (требуются для типов в сигнатурах слотов)
#include <QWidget>
#include <QVector>
#include "physicsengine.h" // Оставлен, т.к. в заголовке используется ExperimentState и Sample

class ControlPanelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ControlPanelWidget(QWidget* parent = nullptr);
    ~ControlPanelWidget() override = default;

    PhysicsEngine* getEngine() const { return m_engine; }

signals:
    void environmentTemperatureSet(double t0);
    void exportRequested();
    void samplesStatusUpdated(const QVector<bool>& statuses);
    void sampleActivationRequested(int sampleIndex, bool active);
    void backToMenuRequested();
    void materialSymbolChanged(int SampleIndex, QString& symbol);

public slots:
    void onSampleCheckboxChanged(int sampleIndex, bool checked);

private slots:
    void setupConnections();
    void onMaterialChanged(int sampleIndex, const QString& materialName);
    void clearSampleLabel(int sampleIndex);
    void onStartHeatingClicked();
    QVector<Sample> gatherSamples() const;
    void onStateChanged(ExperimentState state);
    void onTimeUpdated(int seconds);
    void onPointsCountUpdated(int count);
    void onCoolingTimeUpdated(double timeSec);

private:
    void setupUI();

    // --- Контейнеры (группы) ---
    QGroupBox* m_sampleGroup = nullptr;
    QGroupBox* m_envGroup = nullptr;
    QGroupBox* m_controlGroup = nullptr;
    QGroupBox* m_statusGroup = nullptr;

    // --- Виджеты образцов ---
    QComboBox* first_mat_combo = nullptr;
    QComboBox* second_mat_combo = nullptr;
    QComboBox* third_mat_combo = nullptr;
    QComboBox* fourth_mat_combo = nullptr;

    QDoubleSpinBox* first_mat_spin = nullptr;
    QDoubleSpinBox* second_mat_spin = nullptr;
    QDoubleSpinBox* third_mat_spin = nullptr;
    QDoubleSpinBox* fourth_mat_spin = nullptr;

    QLabel* first_mat_label = nullptr;
    QLabel* second_mat_label = nullptr;
    QLabel* third_mat_label = nullptr;
    QLabel* fourth_mat_label = nullptr;

    // --- Виджеты среды ---
    QDoubleSpinBox* T0 = nullptr;
    QSpinBox* humidity = nullptr;
    QLabel* consts = nullptr;

    // --- Кнопки ---
    QPushButton* nagrev = nullptr;
    QPushButton* termostat = nullptr;
    QPushButton* start = nullptr;
    QPushButton* write_point = nullptr;
    QPushButton* stop = nullptr;
    QPushButton* export_btn = nullptr;
    QPushButton* reset_btn = nullptr;
    QPushButton* back_btn = nullptr;

    // --- Статус ---
    QLabel* phase = nullptr;
    QLabel* time = nullptr;
    QLabel* pointsLabel = nullptr;
    QLabel* m_coolingTimeLabel = nullptr;

    // --- Основной компоновщик ---
    QVBoxLayout* m_mainLayout = nullptr;

    // --- Для материалов ---
    QVector<QComboBox*> m_sampleCombos;
    QVector<QLabel*> m_sampleLabels;

    // --- Движок ---
    PhysicsEngine* m_engine = nullptr;

    // --- Состояния чекбоксов ---
    bool m_checkboxStates[4] = {false, false, false, false};
};
