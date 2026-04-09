#include "experimentwindow.h"
#include "mainwindow.h" // Подключаем mainwindow.h для доступа к методам
#include <QVBoxLayout>
#include <QLabel>

ExperimentWindow::ExperimentWindow(MainWindow *parentWindow, QWidget *parent)
    : QMainWindow(parent), mainWindowPtr(parentWindow)
{
    setWindowTitle("Лабораторная работа №3: Эксперимент");
    setMinimumSize(1000, 700);

    // Центральный виджет для размещения кнопки
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // Временный текст, пока нет основного функционала
    QLabel *placeholderLabel = new QLabel("Здесь будет экспериментальная установка.", this);
    placeholderLabel->setAlignment(Qt::AlignCenter);
    placeholderLabel->setStyleSheet("QLabel { font-size: 18px; color: gray; }");
    layout->addWidget(placeholderLabel);

    // Кнопка "Назад"
    backButton = new QPushButton("⬅ Назад", this);
    backButton->setMaximumWidth(150);
    backButton->setStyleSheet(
        "QPushButton { "
        "    background-color: #e74c3c; " // Красный цвет для кнопки выхода
        "    color: white; "
        "    border: none; "
        "    border-radius: 5px; "
        "    padding: 8px 16px; "
        "    font-weight: bold; "
        "    font-size: 14px; "
        "} "
        "QPushButton:hover { background-color: #c0392b; }"
        );

    // Размещаем кнопку внизу по центру
    layout->addWidget(backButton, 0, Qt::AlignBottom | Qt::AlignCenter);

    // Подключаем сигнал
    connect(backButton, &QPushButton::clicked, this, &ExperimentWindow::goBack);
}

ExperimentWindow::~ExperimentWindow()
{
}

void ExperimentWindow::goBack()
{
    // 1. Закрываем (скрываем) окно эксперимента
    this->close();

    // 2. Показываем главное окно
    if (mainWindowPtr) {
        mainWindowPtr->show();
    }
}
