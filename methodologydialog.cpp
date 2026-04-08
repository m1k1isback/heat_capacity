#include "methodologydialog.h"
#include <QFont>
#include <QScrollBar>

MethodologyDialog::MethodologyDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Методические указания");
    setMinimumSize(900, 700);
    setModal(true);  // Делаем окно модальным
    setupUI();
}

MethodologyDialog::~MethodologyDialog()  // <-- ОБЯЗАТЕЛЬНО: деструктор
{
}

void MethodologyDialog::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);

    titleLabel = new QLabel("МЕТОДИКИ ИЗМЕРЕНИЯ ВЕЛИЧИН", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("Arial", 16, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("QLabel { color: #2c3e50; padding: 10px; }");
    mainLayout->addWidget(titleLabel);

    textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);
    textEdit->setHtml(getMethodologyText());
    textEdit->setFont(QFont("Arial", 11));
    textEdit->setStyleSheet(
        "QTextEdit { background-color: white; border: 2px solid #bdc3c7; border-radius: 5px; padding: 10px; }"
        );
    mainLayout->addWidget(textEdit);

    closeButton = new QPushButton("Закрыть", this);
    closeButton->setMaximumWidth(150);
    closeButton->setMinimumHeight(40);
    QFont buttonFont("Arial", 11, QFont::Bold);
    closeButton->setFont(buttonFont);
    closeButton->setStyleSheet(
        "QPushButton { background-color: #3498db; color: white; border: none; border-radius: 5px; padding: 8px; }"
        "QPushButton:hover { background-color: #2980b9; }"
        "QPushButton:pressed { background-color: #1f6aa5; }"
        );
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
}

QString MethodologyDialog::getMethodologyText()
{
    return R"(
    <h2 style='color: #2c3e50;'>ОБЩИЕ ПОЛОЖЕНИЯ</h2>

    <h3>1. Вид зависимости температуры</h3>

    <h4>Случай без подвода мощности:</h4>
    <p>Уравнение теплового баланса:</p>
    <p style='background-color: #ecf0f1; padding: 10px; border-left: 4px solid #3498db;'>
        dT/dt = -K(T - T₀)
    </p>
    <p>После интегрирования:</p>
    <p style='background-color: #ecf0f1; padding: 10px; border-left: 4px solid #3498db;'>
        ln(T - T₀) = -Kt + const
    </p>

    <h4>Случай постоянной мощности:</h4>
    <p>Уравнение теплового баланса:</p>
    <p style='background-color: #ecf0f1; padding: 10px; border-left: 4px solid #3498db;'>
        dT/dt = P/C - K(T - T₀)
    </p>
    <p>Общее решение:</p>
    <p style='background-color: #ecf0f1; padding: 10px; border-left: 4px solid #3498db;'>
        T - T₀ = (P/(CK))·(1 - e^(-Kt))
    </p>

    <h2 style='color: #2c3e50; margin-top: 30px;'>ТЕПЛОЁМКОСТЬ</h2>

    <p>Теплоёмкость — производная от количества теплоты по температуре:</p>
    <p style='background-color: #ecf0f1; padding: 10px; border-left: 4px solid #3498db;'>
        C = δQ/dT
    </p>

    <h3>Метод наименьших квадратов:</h3>
    <p>Аппроксимация: y = ax + b</p>
    <p>Система уравнений:</p>
    <ul>
        <li>a·Σx² + b·Σx = Σxy</li>
        <li>a·Σx + b·n = Σy</li>
    </ul>

    <h3>Формулы для расчёта:</h3>
    <p style='background-color: #ecf0f1; padding: 10px;'>
        Теплоёмкость образца: c' = UI/(K·ΔT)·(1 - e^(-KΔt))<br>
        Удельная теплоёмкость: c = c'/m<br>
        Молярная теплоёмкость: C = c·M
    </p>

    <h2 style='color: #2c3e50; margin-top: 30px;'>ТЕПЛОПРОВОДНОСТЬ</h2>

    <p>Закон Био-Фурье:</p>
    <p style='background-color: #ecf0f1; padding: 10px; border-left: 4px solid #3498db;'>
        q = -λ·(dT/dx), &nbsp;&nbsp; λ = (P·L)/(S·ΔT)
    </p>

    <h2 style='color: #2c3e50; margin-top: 30px;'>ТЕПЛООТДАЧА</h2>

    <p>Коэффициент теплоотдачи:</p>
    <p style='background-color: #ecf0f1; padding: 10px; border-left: 4px solid #3498db;'>
        α = (c'·K)/S
    </p>

    <h2 style='color: #2c3e50; margin-top: 30px;'>ТЕПЛОПЕРЕДАЧА</h2>

    <p>Система уравнений теплового баланса:</p>
    <p style='background-color: #ecf0f1; padding: 10px; border-left: 4px solid #3498db;'>
        C₁·dT₁/dt = -k·(T₁ - T₂)<br>
        C₂·dT₂/dt = k·(T₁ - T₂)
    </p>

    <h2 style='color: #2c3e50; margin-top: 30px;'>ТЕПЛОУСВОЕНИЕ</h2>

    <p>Теплоусвоение — эффективность поглощения тепла при периодических колебаниях:</p>
    <p style='background-color: #ecf0f1; padding: 10px; border-left: 4px solid #3498db;'>
        b = Qₐ/Tₐ
    </p>

    <h2 style='color: #2c3e50; margin-top: 30px;'>ТЕМПЕРАТУРОПРОВОДНОСТЬ</h2>

    <p>Уравнение теплопроводности:</p>
    <p style='background-color: #ecf0f1; padding: 10px; border-left: 4px solid #3498db;'>
        ∂T/∂t = a·∇²T, &nbsp;&nbsp; a = λ/(ρ·c)
    </p>

    <h2 style='color: #2c3e50; margin-top: 30px;'>МЕТОД ТЕПЛОВОЙ ИНЕРЦИИ</h2>

    <p>Тепловая инерция:</p>
    <p style='background-color: #ecf0f1; padding: 10px; border-left: 4px solid #3498db;'>
        D = R·b = δ·√(π·ω·ρ·c/λ), &nbsp;&nbsp; τ = δ²/(π²·a)
    </p>

    <hr style='margin: 30px 0;'>
    <p style='color: #7f8c8d; font-style: italic;'>
        <b>Примечание:</b> Все формулы приведены в соответствии с методическими указаниями.
    </p>
    )";
}
