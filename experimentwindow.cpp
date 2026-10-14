#include "experimentwindow.h"
#include "mainwindow.h"
#include "controlpanelwidget.h"
#include "CalorimeterScene.h"

#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QGraphicsView>
#include <QResizeEvent>
#include <QTimer>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QFile>
#include <QTextStream>
#include <QFrame>
#include <QPainter>

ExperimentWindow::ExperimentWindow(MainWindow *parentWindow, QWidget *parent)
    : QMainWindow(parent), mainWindowPtr(parentWindow), rowCount(0), currentTime(0)
{
    setupUI();
    setWindowTitle("Лабораторная работа №3: Измерение теплоемкости (Остывание)");
    showMaximized();

    QTimer::singleShot(200, this, [this]() {
        fitSceneToView();
    });
}

ExperimentWindow::~ExperimentWindow() {}

void ExperimentWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    fitSceneToView();
}

void ExperimentWindow::fitSceneToView()
{
    if (graphicsView && scene) {
        QRectF contentRect = scene->itemsBoundingRect();
        contentRect.adjust(-10, -10, 10, 10);
        graphicsView->fitInView(contentRect, Qt::KeepAspectRatio);
        scene->setSceneRect(contentRect);
        graphicsView->centerOn(contentRect.center());
    }
}

void ExperimentWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainHLayout = new QHBoxLayout(central);
    mainHLayout->setContentsMargins(0, 0, 0, 0);
    mainHLayout->setSpacing(0);

    // === ЛЕВАЯ ЧАСТЬ: СЦЕНА + ТАБЛИЦА ===
    QWidget *leftContainer = new QWidget(this);
    leftContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *leftLayout = new QVBoxLayout(leftContainer);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);

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

    leftLayout->addWidget(graphicsView, 4);

    QGroupBox *tableGroup = new QGroupBox("Протокол измерений", this);
    QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);
    tableLayout->setContentsMargins(5, 5, 5, 5);

    tableWidget = new QTableWidget(0, 6, this);
    tableWidget->setHorizontalHeaderLabels({"№", "Время (с)", "T1 (°C)", "T2 (°C)", "T3 (°C)", "T4 (°C)"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setStyleSheet("background-color: white;");
    tableWidget->setMinimumHeight(150);
    tableWidget->setMaximumHeight(250);
    tableWidget->verticalHeader()->setVisible(false);

    tableLayout->addWidget(tableWidget);
    leftLayout->addWidget(tableGroup, 1);

    // Добавляем левую часть с коэффициентом растяжения 1
    mainHLayout->addWidget(leftContainer, 1);

    // === ПРАВАЯ ЧАСТЬ: ПАНЕЛЬ УПРАВЛЕНИЯ ===
    controlPanel = new ControlPanelWidget(this);
    controlPanel->setFixedWidth(380);

    // Добавляем правую часть с коэффициентом растяжения 0 (занимает только фиксированную ширину, но растягивается по высоте благодаря своей QSizePolicy)
    mainHLayout->addWidget(controlPanel, 0);

    // Явное указание распределения пространства для надежности
    mainHLayout->setStretch(0, 1);
    mainHLayout->setStretch(1, 0);

    // === КОННЕКТЫ ===
    connect(controlPanel->getEngine(), &PhysicsEngine::displayTemperaturesUpdated, scene, &CalorimeterScene::onDisplayTemperaturesUpdated);
    connect(controlPanel->getEngine(), &PhysicsEngine::temperaturesUpdated, scene, &CalorimeterScene::onPhysicsTemperaturesUpdated);
    connect(controlPanel, &ControlPanelWidget::environmentTemperatureSet, scene, &CalorimeterScene::setEnvironmentTemperature);
    connect(controlPanel->getEngine(), &PhysicsEngine::pointRecorded, this, &ExperimentWindow::onPointRecorded);
    connect(controlPanel, &ControlPanelWidget::exportRequested, this, &ExperimentWindow::onExportRequested);
    connect(controlPanel, &ControlPanelWidget::samplesStatusUpdated, this, &ExperimentWindow::onSamplesStatusUpdated);
    connect(controlPanel, &ControlPanelWidget::sampleActivationRequested, scene, &CalorimeterScene::setSampleActive);
    connect(scene, &CalorimeterScene::sampleActiveChanged, controlPanel->getEngine(), &PhysicsEngine::setSampleActive);
    connect(scene, &CalorimeterScene::sampleIsChanged, controlPanel, &ControlPanelWidget::onSampleCheckboxChanged);
    connect(controlPanel->getEngine(), &PhysicsEngine::tableReset, this, &ExperimentWindow::onTableReset);
    connect(scene, &CalorimeterScene::differentialModeChanged, controlPanel->getEngine(), &PhysicsEngine::setDifferentialMode);
    connect(scene, &CalorimeterScene::tableHeaderChanged, this, &ExperimentWindow::onTableHeaderChanged);
    connect(controlPanel, &ControlPanelWidget::backToMenuRequested, this, &ExperimentWindow::returnToMainMenuRequested);
    connect(controlPanel, &ControlPanelWidget::materialSymbolChanged, this, &ExperimentWindow::onMaterialSymbolChanged);

    // Передаем название материала на сцену для обновления штриховки
    connect(controlPanel, &ControlPanelWidget::materialNameChanged,
            scene, &CalorimeterScene::setSampleMaterial);

    // При старте остывания сообщаем сцене начальную температуру для расчета анимации
    connect(controlPanel, &ControlPanelWidget::coolingStarted,
            scene, [this](double initialTemp, bool isAccelerated) {
                Q_UNUSED(isAccelerated);
                scene->setCoolingStartTemperature(initialTemp);
            });
}

void ExperimentWindow::onPointRecorded(int id, double time, const QVector<double>& temps)
{
    int newRow = tableWidget->rowCount();
    tableWidget->insertRow(newRow);
    tableWidget->setItem(newRow, 0, new QTableWidgetItem(QString::number(id)));
    tableWidget->setItem(newRow, 1, new QTableWidgetItem(QString::number(time, 'f', 2)));

    for(int i = 0; i < 4; i++){
        tableWidget->setItem(newRow, i + 2, new QTableWidgetItem(QString::number(temps[i], 'f', 3)));
    }
    tableWidget->scrollToBottom();
}

void ExperimentWindow::onTableHeaderChanged(int sampleIndex, bool isDifferential)
{
    if (sampleIndex >= 0 && sampleIndex < 4) m_isDifferentialMode[sampleIndex] = isDifferential;
    updateColumnHeader(sampleIndex);
}

void ExperimentWindow::onMaterialSymbolChanged(int sampleIndex, const QString& symbol)
{
    if (sampleIndex < 0 || sampleIndex >= 4) return;
    m_materialSymbols[sampleIndex] = symbol;
    updateColumnHeader(sampleIndex);
}

void ExperimentWindow::updateColumnHeader(int sampleIndex)
{
    if (sampleIndex < 0 || sampleIndex >= 4) return;
    QString symbol = m_materialSymbols[sampleIndex];
    bool isDifferential = m_isDifferentialMode[sampleIndex];
    int column = sampleIndex + 2;

    if (!tableWidget) return;
    QTableWidgetItem* headerItem = tableWidget->horizontalHeaderItem(column);
    if (!headerItem) return;

    int sampleNumber = sampleIndex + 1;
    QString headerText = symbol.isEmpty()
                             ? (isDifferential ? QString("T%1-T0 (°C)").arg(sampleNumber) : QString("T%1 (°C)").arg(sampleNumber))
                             : (isDifferential ? QString("T%1-T0 (°C) %2").arg(sampleNumber).arg(symbol) : QString("T%1 (°C) %2").arg(sampleNumber).arg(symbol));

    headerItem->setText(headerText);
}

void ExperimentWindow::onTableReset()
{
    tableWidget->setRowCount(0);
    for(int i = 0; i < 4; ++i) {
        auto* item = tableWidget->horizontalHeaderItem(i + 2);
        if(item) item->setText(QString("T%1 (°C)").arg(i + 1));
    }
}

void ExperimentWindow::onExportRequested()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Экспорт протокола", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл!");
        return;
    }

    QTextStream stream(&file);
    stream.setGenerateByteOrderMark(true);

    for (int col = 0; col < tableWidget->columnCount(); ++col) {
        if (tableWidget->isColumnHidden(col)) continue;
        QTableWidgetItem *headerItem = tableWidget->horizontalHeaderItem(col);
        stream << (headerItem ? headerItem->text() : "") << ";";
    }
    stream << "\n";

    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        for (int col = 0; col < tableWidget->columnCount(); ++col) {
            if (tableWidget->isColumnHidden(col)) continue;
            QTableWidgetItem* item = tableWidget->item(row, col);
            if (item) {
                QString text = item->text();
                if (col >= 1) text.replace('.', ',');
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

void ExperimentWindow::onSamplesStatusUpdated(const QVector<bool>& statuses)
{
    for (int i = 0; i < statuses.size(); ++i) {
        if (statuses[i]) tableWidget->showColumn(i + 2);
        else tableWidget->hideColumn(i + 2);
    }
}
