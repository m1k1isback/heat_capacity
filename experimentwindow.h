#ifndef EXPERIMENTWINDOW_H
#define EXPERIMENTWINDOW_H

#include <QMainWindow>
#include <QPushButton>

// Объявляем класс заранее, чтобы не было циклических зависимостей
class MainWindow;

class ExperimentWindow : public QMainWindow
{
    Q_OBJECT
public:
    // Принимаем указатель на главное окно
    explicit ExperimentWindow(MainWindow *parentWindow, QWidget *parent = nullptr);
    ~ExperimentWindow();

private slots:
    void goBack();

private:
    MainWindow *mainWindowPtr; // Указатель на главное окно для возврата
    QPushButton *backButton;
};

#endif // EXPERIMENTWINDOW_H
