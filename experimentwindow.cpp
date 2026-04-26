#include "experimentwindow.h"
#include "mainwindow.h"
#include "controlpanelwidget.h"

#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QGraphicsView>
#include <QResizeEvent>
#include <QTimer>

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
        // Получаем реальные границы объектов
        QRectF contentRect = scene->itemsBoundingRect();

        // Добавляем небольшой отступ (10%)
        qreal padding = qMin(contentRect.width(), contentRect.height()) * 0.1;
        contentRect.adjust(-padding, -padding, padding, padding);

        // Масштабируем вид под объекты
        graphicsView->fitInView(contentRect, Qt::KeepAspectRatio);

        // ВАЖНО: Обновляем sceneRect под реальные объекты
        // Это убирает возможность скролла
        scene->setSceneRect(contentRect);

        // Центрируем
        graphicsView->centerOn(contentRect.center());
    }
}

void ExperimentWindow::setupUI()
{
    // === 1. Создаём центральный виджет и ОСНОВНОЙ ГОРИЗОНТАЛЬНЫЙ layout ===
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainHLayout = new QHBoxLayout(central);  // ✅ ГОРИЗОНТАЛЬНЫЙ для всего окна
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
    tableWidget->setMaximumHeight(200);  // ✅ Ограничиваем высоту таблицы
    tableWidget->verticalHeader()->setVisible(false);

    tableLayout->addWidget(tableWidget);
    leftLayout->addWidget(tableGroup, 0);  // 0 = фиксированная высота

    // Добавляем левую часть в основной layout
    mainHLayout->addWidget(leftContainer, 4);  // 4 = 80% ширины окна

    // === 3. ПРАВАЯ ЧАСТЬ ОКНА (панель управления на всю высоту) ===
    auto* controlPanel = new ControlPanelWidget(this);
    controlPanel->setMinimumWidth(350);  // ✅ Делаем панель шире (было 280)
    controlPanel->setMaximumWidth(450);  // ✅ Но не даём стать слишком широкой

    mainHLayout->addWidget(controlPanel, 1);  // 1 = 20% ширины окна
}
