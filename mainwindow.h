#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QScreen>

class MethodologyDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startExperiment();
    void showMethodology();

private:
    void setupUI();

    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QLabel *numberLabel;
    QHBoxLayout *buttonLayout;
    QPushButton *startButton;
    QPushButton *methodologyButton;

    MethodologyDialog *methodologyDialog;
};

#endif // MAINWINDOW_H
