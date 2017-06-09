#ifndef NEWLINEPOLICYDIALOG_H
#define NEWLINEPOLICYDIALOG_H

#include <QWidget>

namespace Ui {
class NewLinePolicyDialog;
}

class NewLinePolicyDialog : public QWidget
{
    Q_OBJECT

public:
    explicit NewLinePolicyDialog(QWidget *parent = 0);
    ~NewLinePolicyDialog();
    void show(unsigned char currentNewLineSign);
signals:
    void newLineSignChanged(unsigned char newLineSign);

private slots:
    void on_pushButton_cancel_clicked();

    void on_pushButton_ok_clicked();

    void on_comboBox_currentIndexChanged(const QString &arg1);

private:
    unsigned char localNewLineSign;
    Ui::NewLinePolicyDialog *ui;
};

#endif // NEWLINEPOLICYDIALOG_H
