#ifndef EXPERIMENTWINDOW_H
#define EXPERIMENTWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QGraphicsView>

class CalorimeterScene;
class ControlPanelWidget;
class MainWindow;

class ExperimentWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ExperimentWindow(MainWindow *parentWindow, QWidget *parent = nullptr);
    ~ExperimentWindow();

private:
    void setupUI();
    void fitSceneToView();
    void resizeEvent(QResizeEvent *event) override;
    void updateColumnHeader(int sampleIndex);

private slots:
    void onPointRecorded(int id, double time, const QVector<double>& temps);
    void onExportRequested();
    void onSamplesStatusUpdated(const QVector<bool>& statuses);
    void onTableReset();
    void onTableHeaderChanged(int sampleIndex, bool isDifferential);
    void onMaterialSymbolChanged(int sampleIndex, const QString& symbol);

signals:
    void returnToMainMenuRequested();

private:
    MainWindow *mainWindowPtr;
    CalorimeterScene *scene;
    QGraphicsView *graphicsView;
    ControlPanelWidget *controlPanel;
    QTableWidget *tableWidget;

    int rowCount;
    int currentTime;
    bool m_isDifferentialMode[4] = {false, false, false, false};
    QString m_materialSymbols[4] = {"", "", "", ""};
};

#endif // EXPERIMENTWINDOW_H
