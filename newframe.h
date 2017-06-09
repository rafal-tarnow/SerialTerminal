#ifndef NEWFRAME_H
#define NEWFRAME_H

#include <QWidget>

#include <vector>
#include <string>

using namespace std;

namespace Ui {
class NewFrame;
}

class NewFrame : public QWidget
{
    Q_OBJECT

public:
    explicit NewFrame(QWidget *parent = 0);
    ~NewFrame();
    void show(QString comboBoxText);

signals:
    void newFrameAdded(string frameName, QString frameData);

private slots:
    void on_pushButton_newFrameCancel_clicked();

    void on_pushButton_newFrameSave_clicked();

private:
    Ui::NewFrame *ui;
};

#endif // NEWFRAME_H
