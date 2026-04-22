#include "ExperimentWindow.h"
#include "MainWindow.h" // Для возврата назад
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QGraphicsView>

ExperimentWindow::ExperimentWindow(MainWindow *parentWindow, QWidget *parent)
    : QMainWindow(parent), mainWindowPtr(parentWindow), rowCount(0), currentTime(0)
{
    setupUI();
    setWindowTitle("Лабораторная работа №3: Эксперимент");
    //setMinimumSize(1500, 790);
    showMaximized();
}

ExperimentWindow::~ExperimentWindow()
{
}

void ExperimentWindow::setupUI()
{
    // 1. Центральный виджет и Основной вертикальный Layout
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // 2. ВЕРХНЯЯ ЧАСТЬ: Схема установки (CalorimeterScene)
    scene = new CalorimeterScene(this);
    QGraphicsView *view = new QGraphicsView(scene, this);
    view->setFrameStyle(QFrame::NoFrame); // Убираем рамку
    view->setStyleSheet("background-color: transparent;");
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setAlignment(Qt::AlignCenter);
    //view->setMinimumHeight(300); // Фиксируем высоту схемы
    //view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    view->setMinimumSize(600, 600);

    mainLayout->addWidget(view);
    //---------------------------------------------------------------------------

    // 3. НИЖНЯЯ ЧАСТЬ: Разделена на две колонки (Таблица и Управление)
    QHBoxLayout *bottomLayout = new QHBoxLayout();

    // --- ЛЕВАЯ КОЛОНКА: Таблица измерений ---
    QGroupBox *tableGroup = new QGroupBox("Протокол измерений", this);
    QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);

    tableWidget = new QTableWidget(0, 6, this); // 6 колонок: №, Время, T1..T4
    tableWidget->setHorizontalHeaderLabels({"№", "Время (с)", "T1 (°C)", "T2 (°C)", "T3 (°C)", "T4 (°C)"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // Только для чтения
    tableWidget->setStyleSheet("background-color: white;");

    tableLayout->addWidget(tableWidget);
    bottomLayout->addWidget(tableGroup, 2); // Занимает 2/3 ширины
    //--------------------------------------------------------------------

    // --- ПРАВАЯ КОЛОНКА: Управление ---
    QGroupBox *controlGroup = new QGroupBox("Управление", this);
    QVBoxLayout *controlLayout = new QVBoxLayout(controlGroup);

    // Кнопки действий
    btnStart = new QPushButton("▶ Начать нагрев", this);
    btnStart->setStyleSheet("background-color: #27ae60; color: white; padding: 10px; border-radius: 5px;");

    btnRecord = new QPushButton("📝 Записать показания", this);
    btnRecord->setEnabled(false); // Сначала недоступна
    btnRecord->setStyleSheet("background-color: #2980b9; color: white; padding: 10px; border-radius: 5px;");

    btnStop = new QPushButton("⏹ Остановить", this);
    btnStop->setEnabled(false);
    btnStop->setStyleSheet("background-color: #c0392b; color: white; padding: 10px; border-radius: 5px;");

    controlLayout->addWidget(btnStart);
    controlLayout->addWidget(btnRecord);
    controlLayout->addWidget(btnStop);
    controlLayout->addStretch();
    //----------------------------------------------------------------------

    // Кнопка НАЗАД
    btnBack = new QPushButton("⬅ Назад в меню", this);
    btnBack->setStyleSheet("background-color: #7f8c8d; color: white; padding: 10px; border-radius: 5px;");
    controlLayout->addWidget(btnBack);

    bottomLayout->addWidget(controlGroup, 1); // Занимает 1/3 ширины
    mainLayout->addLayout(bottomLayout);
    //------------------------------------------------------------

    // 4. ПОДКЛЮЧЕНИЕ СИГНАЛОВ
    connect(btnStart, &QPushButton::clicked, this, [this](){
        QMessageBox::information(this, "Инфо", "Логика нагрева будет добавлена на следующем шаге.");
    });

    connect(btnRecord, &QPushButton::clicked, this, [this](){
        // Временная запись тестовых данных
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
        tableWidget->setItem(row, 1, new QTableWidgetItem("300"));
        for(int i=1; i<5; ++i) tableWidget->setItem(row, i+1, new QTableWidgetItem("59.5"));
    });

    connect(btnBack, &QPushButton::clicked, this, [this](){
        this->close();
        if(mainWindowPtr) mainWindowPtr->show();
    });
    //-----------------------------------------------------------------
}
