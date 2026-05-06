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
    // === 1. Создаём центральный виджет и ОСНОВНОЙ ГОРИЗОНТАЛЬНЫЙ layout ===
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainHLayout = new QHBoxLayout(central);
    mainHLayout->setContentsMargins(0, 0, 0, 0);
    mainHLayout->setSpacing(0);

    // === 2. ЛЕВАЯ ЧАСТЬ ОКНА (сцена + таблица) ===
    QWidget *leftContainer = new QWidget(this);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftContainer);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);

    // 2.1: Сцена
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

    // 2.2: Таблица (внизу слева)
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

    // === 3. ПРАВАЯ ЧАСТЬ ОКНА (панель управления на всю высоту) ===
    auto* controlPanel = new ControlPanelWidget(this);
    controlPanel->setMinimumWidth(350);
    controlPanel->setMaximumWidth(450);

    mainHLayout->addWidget(controlPanel, 1);  // 1 = 20% ширины окна


    // 1. Подключаем сигнал температур от движка к сцене
    connect(controlPanel->getEngine(), &PhysicsEngine::temperaturesUpdated,
            scene, &CalorimeterScene::onPhysicsTemperaturesUpdated);

    connect(controlPanel, &ControlPanelWidget::environmentTemperatureSet,
            scene, &CalorimeterScene::setEnvironmentTemperature);

    // 1. Подключаем сигнал температур
    auto conn = connect(controlPanel->getEngine(), &PhysicsEngine::temperaturesUpdated,
                        scene, &CalorimeterScene::onPhysicsTemperaturesUpdated);

    connect(controlPanel->getEngine(), &PhysicsEngine::pointRecorded,
            this, &ExperimentWindow::onPointRecorded);

    connect(controlPanel, &ControlPanelWidget::exportRequested,
            this, &ExperimentWindow::onExportRequested);
}

void ExperimentWindow::onPointRecorded(int id, double time, const QVector<double>& temps){
    int newRow = tableWidget->rowCount();
    tableWidget->insertRow(newRow);

    tableWidget->setItem(newRow, 0, new QTableWidgetItem(QString::number(id)));
    tableWidget->setItem(newRow, 1, new QTableWidgetItem(QString::number(time, 'f', 2)));

    // 4. Запиши T1, T2, T3, T4 (колонки 2, 3, 4, 5)
    for(int i = 0; i < 4; i++){
        int column = i + 2;
        tableWidget->setItem(newRow, column, new QTableWidgetItem(QString::number(temps[i], 'f', 3)));
    }
    qDebug() << "Записана точка #" << id << "в строку" << newRow;
}

void ExperimentWindow::onExportRequested(){
    QString fileName = QFileDialog::getSaveFileName(this, "Экспорт в Excel", "", "CSV Files (*.csv);;Text Files (*.txt)" );
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл!");
        return;
    }

    QTextStream stream(&file);
      // stream.setCodec("UTF-8"); // Можно включить, если русские буквы будут кракозябрами
    //stream.setCodec("UTF-8");
    stream.setGenerateByteOrderMark(true);

    // 2. Записываем ЗАГОЛОВКИ (№, Время, T1...)
    for (int col = 0; col < tableWidget->columnCount(); ++col) {
        QTableWidgetItem *headerItem = tableWidget->horizontalHeaderItem(col);

        // 2. Если заголовок существует, берем его текст и пишем в файл
        if (headerItem) {
            stream << headerItem->text() << ";";
        } else {
            // На всякий случай, если заголовка нет, пишем пустоту
            stream << "\;";
        }
    }
    stream << "\n"; // Переход на новую строку после заголовков

    // 3. Записываем ДАННЫЕ (строки таблицы)
    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        for (int col = 0; col < tableWidget->columnCount(); ++col) {
            QTableWidgetItem* item = tableWidget->item(row, col);
            if (item) {
                QString text = item->text();

                if (col > 0) {
                    text.replace('.', ',');
                }

                stream << text << ";";
            } else {
                stream << ";";
            }
        }
        stream << "\n";
    }

    file.close();
    QMessageBox::information(this, "Успех", "Данные успешно экспортированы!");
}

