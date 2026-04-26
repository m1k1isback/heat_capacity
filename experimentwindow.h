#ifndef EXPERIMENTWINDOW_H
#define EXPERIMENTWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QGraphicsView>
#include <QResizeEvent> // Добавлено для resizeEvent
#include "CalorimeterScene.h"

class MainWindow;

class ExperimentWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ExperimentWindow(MainWindow *parentWindow = nullptr, QWidget *parent = nullptr);
    ~ExperimentWindow();

protected:
    // Переопределяем событие изменения размера для динамического масштабирования
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupUI();

    MainWindow *mainWindowPtr;
    CalorimeterScene *scene;
    QGraphicsView *graphicsView; // Теперь это член класса
    QTableWidget *tableWidget;
    QPushButton *btnStart;
    QPushButton *btnRecord;
    QPushButton *btnStop;
    QPushButton *btnBack;
    void fitSceneToView();

    int rowCount;
    int currentTime;
};

#endif // EXPERIMENTWINDOW_H
