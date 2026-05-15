#include "experimentwindow.h"
#include "mainwindow.h"
#include "controlpanelwidget.h"

#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QGraphicsView>
#include <QResizeEvent>
#include <QTimer>
#include <QFileDialog>

ExperimentWindow::ExperimentWindow(MainWindow *parentWindow, QWidget *parent)
    : QMainWindow(parent), mainWindowPtr(parentWindow), rowCount(0), currentTime(0)
{
    setupUI();
    setWindowTitle("Лабораторная работа №3: Эксперимент");
    showMaximized();

    QTimer::singleShot(200, this, [this]() {
        fitSceneToView();
    });
}

ExperimentWindow::~ExperimentWindow()
{
}

void ExperimentWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    fitSceneToView();
}

void ExperimentWindow::fitSceneToView()
{
    if (graphicsView && scene) {
        QRectF contentRect = scene->itemsBoundingRect();
        contentRect.adjust(-2, -2, 2, 2);
        graphicsView->fitInView(contentRect, Qt::KeepAspectRatio);
        scene->setSceneRect(contentRect);
        graphicsView->centerOn(contentRect.center());
    }
}

void ExperimentWindow::setupUI()
{
    // Создание основного виджета на экране лабораторной работы
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // Основной лэйаут
    QHBoxLayout *mainHLayout = new QHBoxLayout(central);
    mainHLayout->setContentsMargins(0, 0, 0, 0);
    mainHLayout->setSpacing(0);

    // ЛЕВАЯ ЧАСТЬ ЭКРАНА
    QWidget *leftContainer = new QWidget(this);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftContainer);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);

    // УСТАНОВКА
    scene = new CalorimeterScene(this);
    graphicsView = new QGraphicsView(scene, this);
    graphicsView->setFrameStyle(QFrame::NoFrame);
    graphicsView->setStyleSheet("background-color: #f0f0f0;");
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setDragMode(QGraphicsView::NoDrag);
    graphicsView->setInteractive(true);
    graphicsView->setAlignment(Qt::AlignCenter);
    graphicsView->setRenderHint(QPainter::Antialiasing);
    graphicsView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    graphicsView->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    graphicsView->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    leftLayout->addWidget(graphicsView, 1);  // 1 = занимает всё доступное место

    // ТАБЛИЦА ДАННЫХ
    QGroupBox *tableGroup = new QGroupBox("Протокол измерений", this);
    QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);
    tableLayout->setContentsMargins(5, 5, 5, 5);

    tableWidget = new QTableWidget(0, 6, this);
    tableWidget->setHorizontalHeaderLabels({"№", "Время (с)", "T1 (°C)", "T2 (°C)", "T3 (°C)", "T4 (°C)"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setStyleSheet("background-color: white;");
    tableWidget->setMinimumHeight(120);
    tableWidget->setMaximumHeight(200);
    tableWidget->verticalHeader()->setVisible(false);

    tableLayout->addWidget(tableWidget);
    leftLayout->addWidget(tableGroup, 0);  // 0 = фиксированная высота

    // Добавляем левую часть в основной layout
    mainHLayout->addWidget(leftContainer, 4);  // 4 = 80% ширины окна

    // ПАНЕЛЬ УПРАВЛЕНИЯ
    auto* controlPanel = new ControlPanelWidget(this);
    controlPanel->setMinimumWidth(350);
    controlPanel->setMaximumWidth(450);

    mainHLayout->addWidget(controlPanel, 1);  // 1 = 20% ширины окна


    // КОННЕКТЫ БЭКЭНДА И ФРОНТЕНДА

    connect(controlPanel->getEngine(), &PhysicsEngine::temperaturesUpdated,             // Обновление температур на дисплеях
            scene, &CalorimeterScene::onPhysicsTemperaturesUpdated);

    connect(controlPanel, &ControlPanelWidget::environmentTemperatureSet,               // Установка температуры среды
            scene, &CalorimeterScene::setEnvironmentTemperature);

    connect(controlPanel->getEngine(), &PhysicsEngine::pointRecorded,                   // Запись точки
            this, &ExperimentWindow::onPointRecorded);

    connect(controlPanel, &ControlPanelWidget::exportRequested,                         // Экспорт файла при нажатии кнопки
            this, &ExperimentWindow::onExportRequested);

    connect(controlPanel, &ControlPanelWidget::samplesStatusUpdated,                    // Видимость столбцов в таблице данных
            this, &ExperimentWindow::onSamplesStatusUpdated);

    connect(controlPanel, &ControlPanelWidget::sampleActivationRequested,               // Активизация образца на сцене
            scene, &CalorimeterScene::setSampleActive);

    connect(scene, &CalorimeterScene::sampleActiveChanged,                              // Связываем состояние чекбокса с движком
            controlPanel->getEngine(), &PhysicsEngine::setSampleActive);

    connect(scene, &CalorimeterScene::sampleIsChanged,                                  // Связываем состояние чекбокса с панелью
            controlPanel, &ControlPanelWidget::onSampleCheckboxChanged);

    connect(controlPanel->getEngine(), &PhysicsEngine::tableReset,                      // Очищение таблицы
            this, &ExperimentWindow::onTableReset);

    connect(scene, &CalorimeterScene::differentialModeChanged,                          // Дифференциальный режим
            controlPanel->getEngine(), &PhysicsEngine::setDifferentialMode);

    connect(scene, &CalorimeterScene::tableHeaderChanged,                               // Связь дифференциального режима с колонками таблицы
            this, &ExperimentWindow::onTableHeaderChanged);

    connect(controlPanel, &ControlPanelWidget::backToMenuRequested,                     // Эмит закрыть окно эксперимента и вернуться в меню
            this, &ExperimentWindow::returnToMainMenuRequested);
}

// Запись точки в таблицу
void ExperimentWindow::onPointRecorded(int id, double time, const QVector<double>& temps){
    int newRow = tableWidget->rowCount();
    tableWidget->insertRow(newRow);

    tableWidget->setItem(newRow, 0, new QTableWidgetItem(QString::number(id)));
    tableWidget->setItem(newRow, 1, new QTableWidgetItem(QString::number(time, 'f', 2)));

    // Колонки в таблице
    for(int i = 0; i < 4; i++){
        int column = i + 2;
        tableWidget->setItem(newRow, column, new QTableWidgetItem(QString::number(temps[i], 'f', 3)));
    }
}

void ExperimentWindow::onTableHeaderChanged(int sampleIndex, bool isDifferential)
{
    // Столбцы с температурами начинаются с индекса 1 (0-й — время)
    int column = sampleIndex + 2;
    if (column < 2 || column >= tableWidget->columnCount()) {
        return; // Защита от некорректного индекса
    }

    // Формируем новый текст заголовка
    QString newHeader = isDifferential
                            ? QString("T%1-T0 (°C)").arg(sampleIndex + 1)
                            : QString("T%1 (°C)").arg(sampleIndex + 1);

    // Получаем существующий элемент заголовка или создаём новый
    QTableWidgetItem* headerItem = tableWidget->horizontalHeaderItem(column);
    if (!headerItem) {
        headerItem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(column, headerItem);
    }

    // Применяем изменения
    headerItem->setText(newHeader);
}

void ExperimentWindow::onTableReset() {
    tableWidget->setRowCount(0);
    for(int i = 0; i < 4; ++i) {
        int col = i + 2;
        auto* item = tableWidget->horizontalHeaderItem(col);
        if(item) item->setText(QString("T%1 (°C)").arg(i + 1));
    }
}

void ExperimentWindow::onExportRequested(){
    // Открытие диалогового окна
    QString fileName = QFileDialog::getSaveFileName(this, "Экспорт в Excel", "", "CSV Files (*.csv);;Text Files (*.txt)" );
    if (fileName.isEmpty()) return; // если пользователь отменил сохранение

    // Сохраняем путь
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){ // WriteOnly - только запись
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл!");
        return;
    }

    // Настраиваем котировку
    QTextStream stream(&file);
    stream.setGenerateByteOrderMark(true);

    //  Цикл ЗАГОЛОВКОВ
    for (int col = 0; col < tableWidget->columnCount(); ++col) {
        if (tableWidget->isColumnHidden(col)) continue; // пропускаем скрытые столбцы

        QTableWidgetItem *headerItem = tableWidget->horizontalHeaderItem(col); // Добавляем заголовок
        stream << (headerItem ? headerItem->text() : "") << ";"; // Разделитель ";"
    }
    stream << "\n";  // перенос строки

    //  Цикл ДАННЫХ
    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        for (int col = 0; col < tableWidget->columnCount(); ++col) {
            if (tableWidget->isColumnHidden(col)) continue; // пропускаем скрытые столбцы

            QTableWidgetItem* item = tableWidget->item(row, col);
            if (item) {
                QString text = item->text();
                if (col >= 1) { // пропускаем столбец времени
                    text.replace('.', ','); // замена точки на запятую
                }
                stream << text << ";";
            } else {
                stream << ";";
            }
        }
        stream << "\n";  //  Перенос после каждой строки данных
    }

    file.close();
    QMessageBox::information(this, "Успех", "Данные успешно экспортированы!");
}

void ExperimentWindow::onSamplesStatusUpdated(const QVector<bool>& statuses){
    for (int i = 0; i < statuses.size(); ++i) {
        int col = i + 2; // Сдвиг на 2, т.к. первые два столбца — время и номер

        if (statuses[i]) {
            // Образец активен - показываем столбец
            tableWidget->showColumn(col);
        } else {
            // Образец не выбран - скрываем столбец
            tableWidget->hideColumn(col);
        }
    }
}
