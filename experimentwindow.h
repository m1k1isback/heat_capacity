#ifndef EXPERIMENTWINDOW_H
#define EXPERIMENTWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QTimer>
#include "CalorimeterScene.h" // Наша сцена

class MainWindow; // Объявление главного окна для возврата

class ExperimentWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ExperimentWindow(MainWindow *parentWindow, QWidget *parent = nullptr);
    ~ExperimentWindow();

private:
    void setupUI(); // Настройка интерфейса

    MainWindow *mainWindowPtr;
    CalorimeterScene *scene;
    QTableWidget *tableWidget;
    QPushButton *btnStart, *btnStop, *btnRecord, *btnBack;
    QTimer *timer;
    int rowCount;
    double currentTime;
};

#endif // EXPERIMENTWINDOW_H
