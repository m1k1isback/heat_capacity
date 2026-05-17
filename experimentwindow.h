#pragma once

// Forward declarations (используются только как указатели — ускоряет компиляцию)
class MainWindow;
class CalorimeterScene;
class QGraphicsView;
class QTableWidget;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QResizeEvent;

// Необходимые заголовки (базовый класс + типы по значению в сигнатурах)
#include <QMainWindow>
#include <QVector>

class ExperimentWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ExperimentWindow(MainWindow *parentWindow = nullptr, QWidget *parent = nullptr);
    ~ExperimentWindow() override;  // Деструктор объявлен в .cpp, поэтому не = default

protected:
    // Переопределяем событие изменения размера для динамического масштабирования
    void resizeEvent(QResizeEvent *event) override;

signals:
    void returnToMainMenuRequested();

public slots:
    void onPointRecorded(int id, double time, const QVector<double>& temps);
    void onExportRequested();
    void onSamplesStatusUpdated(const QVector<bool>& statuses);
    void onTableReset();
    void onTableHeaderChanged(int sampleIndex, bool isDifferential);
    void onMaterialSymbolChanged(int sampleIndex, const QString& symbol);

private:
    void setupUI();
    void fitSceneToView();
    void updateColumnHeader(int sampleIndex);

    // Указатели на основные компоненты (инициализация в конструкторе)
    MainWindow *mainWindowPtr = nullptr;
    CalorimeterScene *scene = nullptr;
    QGraphicsView *graphicsView = nullptr;
    QTableWidget *tableWidget = nullptr;

    // Кнопки управления
    QPushButton *btnStart = nullptr;
    QPushButton *btnRecord = nullptr;
    QPushButton *btnStop = nullptr;
    QPushButton *btnBack = nullptr;

    // Массив выбранных материалов
    QString m_materialSymbols[4] = {"", "", "", ""};
    bool m_isDifferentialMode[4] = {false, false, false, false};

    // Состояние таблицы и таймера
    int rowCount = 0;
    int currentTime = 0;
};
