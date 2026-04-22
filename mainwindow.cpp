#include "mainwindow.h"
#include "methodologydialog.h"
#include "experimentwindow.h"
#include <QMessageBox>
#include <QApplication>
#include <QScreen>
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), methodologyDialog(nullptr)
{
    setupUI();
}

MainWindow::~MainWindow()
{
    if (methodologyDialog) {
        delete methodologyDialog;
    }
}

void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(20); //?
    mainLayout->addStretch(2); //?

    numberLabel = new QLabel("Лабораторная работа №3", this);
    numberLabel->setAlignment(Qt::AlignCenter);
    QFont numberFont("Arial", 14, QFont::Bold);
    numberLabel->setFont(numberFont);
    mainLayout->addWidget(numberLabel);

    titleLabel = new QLabel("ТЕПЛОЁМКОСТЬ КРИСТАЛЛИЧЕСКИХ ТЕЛ", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setWordWrap(true);
    QFont titleFont("Arial", 18, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("QLabel { color: #2c3e50; padding: 20px; }");
    mainLayout->addWidget(titleLabel);

    mainLayout->addStretch(3);

    buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(30);

    startButton = new QPushButton("Приступить к выполнению", this);
    startButton->setMinimumSize(250, 50);
    QFont buttonFont("Arial", 12, QFont::Bold);
    startButton->setFont(buttonFont);
    startButton->setStyleSheet(
        "QPushButton { background-color: #3498db; color: white; border: none; border-radius: 8px; padding: 10px 20px; }"
        "QPushButton:hover { background-color: #2980b9; }"
        "QPushButton:pressed { background-color: #1f6aa5; }"
        );
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startExperiment);
    buttonLayout->addWidget(startButton);

    methodologyButton = new QPushButton("Методические указания", this);
    methodologyButton->setMinimumSize(250, 50);
    methodologyButton->setFont(buttonFont);
    methodologyButton->setStyleSheet(
        "QPushButton { background-color: #95a5a6; color: white; border: none; border-radius: 8px; padding: 10px 20px; }"
        "QPushButton:hover { background-color: #7f8c8d; }"
        "QPushButton:pressed { background-color: #6c7a7b; }"
        );
    connect(methodologyButton, &QPushButton::clicked, this, &MainWindow::showMethodology);
    buttonLayout->addWidget(methodologyButton);

    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch(1);

    setWindowTitle("Виртуальная лабораторная работа");
    setMinimumSize(800, 600);

    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    move((screenGeometry.width() - 800) / 2, (screenGeometry.height() - 600) / 2);
}

void MainWindow::startExperiment()
{
    // Создаем окно эксперимента, передавая указатель на себя (this)
    ExperimentWindow *expWindow = new ExperimentWindow(this);

    // Позволяем удалить это окно из памяти при закрытии
    expWindow->setAttribute(Qt::WA_DeleteOnClose);

    // Скрываем главное окно (не закрываем, чтобы можно было вернуться)
    this->hide();

    // Показываем окно эксперимента
    expWindow->show();
}

void MainWindow::showMethodology()
{
    MethodologyDialog dlg(this);
    dlg.exec();
}
