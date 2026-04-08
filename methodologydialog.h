#ifndef METHODOLOGYDIALOG_H
#define METHODOLOGYDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QLabel>

class MethodologyDialog : public QDialog
{
    Q_OBJECT

public:
    MethodologyDialog(QWidget *parent = nullptr);
    ~MethodologyDialog();

private:
    void setupUI();
    QString getMethodologyText();

    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QTextEdit *textEdit;
    QPushButton *closeButton;
};

#endif // METHODOLOGYDIALOG_H
