#pragma once

#include <QWidget>
#include <QVector>
#include "physicsengine.h"

class QVBoxLayout;
class QFormLayout;
class QGridLayout;
class QHBoxLayout;
class QGroupBox;
class QComboBox;
class QDoubleSpinBox;
class QSpinBox;
class QPushButton;
class QLabel;
class QSlider;
class QRadioButton;
class QButtonGroup;

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
    void materialSymbolChanged(int sampleIndex, const QString& symbol);
    void coolingStarted(double initialTemp, bool isAccelerated);
    void materialNameChanged(int sampleIndex, const QString& materialName);

public slots:
    void onSampleCheckboxChanged(int sampleIndex, bool checked);

private slots:
    void setupConnections();
    void onMaterialChanged(int sampleIndex, const QString& materialName);
    void clearSampleLabel(int sampleIndex);
    void onStartCoolingClicked();
    void onStateChanged(ExperimentState state);
    void onTimeUpdated(int seconds);
    void onPointsCountUpdated(int count);
    void onCoolingTimeUpdated(double timeSec);

private:
    void setupUI();
    QVector<Sample> gatherSamples() const;

    QGroupBox* m_sampleGroup = nullptr;
    QGroupBox* m_envAndStartGroup = nullptr;
    QGroupBox* m_controlGroup = nullptr;
    QGroupBox* m_statusGroup = nullptr;

    QComboBox* first_mat_combo = nullptr;
    QDoubleSpinBox* first_mat_spin = nullptr;
    QLabel* first_mat_label = nullptr;

    QComboBox* second_mat_combo = nullptr;
    QDoubleSpinBox* second_mat_spin = nullptr;
    QLabel* second_mat_label = nullptr;

    QComboBox* third_mat_combo = nullptr;
    QDoubleSpinBox* third_mat_spin = nullptr;
    QLabel* third_mat_label = nullptr;

    QComboBox* fourth_mat_combo = nullptr;
    QDoubleSpinBox* fourth_mat_spin = nullptr;
    QLabel* fourth_mat_label = nullptr;

    QVector<QComboBox*> m_sampleCombos;
    QVector<QLabel*> m_sampleLabels;

    QDoubleSpinBox* T0 = nullptr;
    QSpinBox* humidity = nullptr;

    QSlider* initialTempSlider = nullptr;
    QDoubleSpinBox* initialTempSpinBox = nullptr;
    QRadioButton* radioAccelerated = nullptr;
    QRadioButton* radioReal = nullptr;

    QPushButton* startCoolingBtn = nullptr;
    QPushButton* write_point = nullptr;
    QPushButton* reset_btn = nullptr;
    QPushButton* export_btn = nullptr;
    QPushButton* back_btn = nullptr;

    QLabel* phase = nullptr;
    QLabel* time = nullptr;
    QLabel* pointsLabel = nullptr;
    QLabel* m_coolingTimeLabel = nullptr;

    QVBoxLayout* m_mainLayout = nullptr;
    PhysicsEngine* m_engine = nullptr;
    bool m_checkboxStates[4] = {true, true, true, true};
};
