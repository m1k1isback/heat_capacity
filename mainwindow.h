#pragma once

// Forward declarations (используются только как указатели — ускоряет компиляцию)
class QPushButton;
class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QMessageBox;
class QApplication;
class QScreen;
class MethodologyDialog;

// Необходимые заголовки (базовый класс + типы по значению в сигнатурах)
#include <QMainWindow>

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

    // Виджеты и компоновщики (инициализация = nullptr)
    QWidget *centralWidget = nullptr;
    QVBoxLayout *mainLayout = nullptr;
    QLabel *titleLabel = nullptr;
    QLabel *numberLabel = nullptr;
    QHBoxLayout *buttonLayout = nullptr;

    // Кнопки
    QPushButton *startButton = nullptr;
    QPushButton *methodologyButton = nullptr;
    QPushButton *guideBtn = nullptr;

    // Диалог методички
    MethodologyDialog *methodologyDialog = nullptr;
};
