#pragma once
#include <QWidget>
#include "physicsengine.h"

class QVBoxLayout;
class QGroupBox;
class QFormLayout;
class QComboBox;
class QDoubleSpinBox;
class QSpinBox;
class QPushButton;
class QLabel;

class ControlPanelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ControlPanelWidget(QWidget* parent = nullptr);
    ~ControlPanelWidget() override = default;

    PhysicsEngine* getEngine() const { return m_engine; }
signals:
    void environmentTemperatureSet(double t0);

private:
    // --- Контейнеры (группы) ---
    QGroupBox* m_sampleGroup;
    QGroupBox* m_envGroup;
    QGroupBox* m_controlGroup;
    QGroupBox* m_statusGroup;

    // --- Виджеты образцов ---
    QComboBox* first_mat_combo;
    QComboBox* second_mat_combo;
    QComboBox* third_mat_combo;
    QComboBox* fourth_mat_combo;

    QDoubleSpinBox* first_mat_spin;
    QDoubleSpinBox* second_mat_spin;
    QDoubleSpinBox* third_mat_spin;
    QDoubleSpinBox* fourth_mat_spin;

    QLabel* first_mat_label;
    QLabel* second_mat_label;
    QLabel* third_mat_label;
    QLabel* fourth_mat_label;

    // --- Виджеты среды ---
    QDoubleSpinBox* T0;
    QSpinBox* humidity;
    QLabel* consts;

    // --- Кнопки ---
    QPushButton* nagrev;
    QPushButton* termostat;
    QPushButton* start;
    QPushButton* write_point;
    QPushButton* stop;
    QPushButton* export_btn;
    QPushButton* reset_btn;

    // --- Статус ---
    QLabel* phase;
    QLabel* time;
    QLabel* pointsLabel;

    // Основной компоновщик
    QVBoxLayout* m_mainLayout;

    // Внутренние методы сборки
    void setupUI();

    // Для материалов
    QVector<QComboBox*> m_sampleCombos;
    QVector<QLabel*> m_sampleLabels;

    PhysicsEngine* m_engine;
private slots:
    void setupConnections();

    void onMaterialChanged(int sampleIndex, const QString& materialName);
    void clearSampleLabel(int sampleIndex);

    void onStartHeatingClicked();
    QVector<Sample> gatherSamples() const;

    void onStateChanged(ExperimentState state);

    void onTimeUpdated(int seconds);
    void onPointsCountUpdated(int count);
};
