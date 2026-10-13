#pragma once

#include <QMainWindow>
#include <QVector>

class MainWindow;
class CalorimeterScene;
class QGraphicsView;
class QTableWidget;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QResizeEvent;
class ControlPanelWidget;

class ExperimentWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ExperimentWindow(MainWindow *parentWindow = nullptr, QWidget *parent = nullptr);
    ~ExperimentWindow() override;

protected:
    void resizeEvent(QResizeEvent *event) override;

signals:
    void returnToMainMenuRequested();

public slots:
    void onPointRecorded(int id, double time, const QVector<double>& temps);
    void onExportRequested();
    void onSamplesStatusUpdated(const QVector<bool>& statuses);
    void onTableReset();
    void onTableHeaderChanged(int sampleIndex, bool isDifferential);
    void onMaterialSymbolChanged(int sampleIndex, const QString& symbol);

private:
    void setupUI();
    void fitSceneToView();
    void updateColumnHeader(int sampleIndex);

    MainWindow *mainWindowPtr = nullptr;
    CalorimeterScene *scene = nullptr;
    QGraphicsView *graphicsView = nullptr;
    QTableWidget *tableWidget = nullptr;
    ControlPanelWidget *controlPanel = nullptr;

    QString m_materialSymbols[4] = {"", "", "", ""};
    bool m_isDifferentialMode[4] = {false, false, false, false};

    int rowCount = 0;
    int currentTime = 0;
};
