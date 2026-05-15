#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QFont>

class GuideDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GuideDialog(QWidget *parent = nullptr);
    ~GuideDialog() override;

private:
    void setupUI();
    QString getGuideText();

    // Виджеты
    QVBoxLayout* mainLayout;
    QLabel* titleLabel;
    QTextEdit* textEdit;
    QPushButton* closeButton;
};
