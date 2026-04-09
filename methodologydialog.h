#ifndef METHODOLOGYDIALOG_H
#define METHODOLOGYDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

class MethodologyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MethodologyDialog(QWidget *parent = nullptr);
    ~MethodologyDialog();

private:
    void setupUI();
    QString getMethodologyText();

    QVBoxLayout *mainLayout;
    QHBoxLayout *buttonLayout;
    QLabel *titleLabel;
    QTextEdit *textEdit;
    QPushButton *closeButton;
};

#endif // METHODOLOGYDIALOG_H
