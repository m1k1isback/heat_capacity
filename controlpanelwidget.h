#pragma once
#include <QWidget>

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

    // --- Статус ---
    QLabel* phase;
    QLabel* time;

    // Основной компоновщик
    QVBoxLayout* m_mainLayout;

    // Внутренние методы сборки
    void setupUI();
private slots:
    void setupConnections();
};
