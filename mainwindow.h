#pragma once

#include <QMainWindow>

class QPushButton;
class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class MethodologyDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void startExperiment();
    void showMethodology();

private:
    void setupUI();

    QWidget *centralWidget = nullptr;
    QVBoxLayout *mainLayout = nullptr;
    QLabel *titleLabel = nullptr;
    QLabel *numberLabel = nullptr;
    QHBoxLayout *buttonLayout = nullptr;

    QPushButton *startButton = nullptr;
    QPushButton *methodologyButton = nullptr;
    QPushButton *guideBtn = nullptr;

    MethodologyDialog *methodologyDialog = nullptr;
};
