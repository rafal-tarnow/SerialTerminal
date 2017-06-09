#ifndef DELETEFRAMEDIALOG_H
#define DELETEFRAMEDIALOG_H

#include <QWidget>
#include <string>
#include <vector>

using namespace std;

namespace Ui {
class DeleteFrameDialog;
}

class DeleteFrameDialog : public QWidget
{
    Q_OBJECT

public:
    explicit DeleteFrameDialog(QWidget *parent = 0);
    ~DeleteFrameDialog();

   void show(map<string, QString> ramki);

signals:
    void frameDeleted(string frameName);

private slots:
    void on_pushButton_delete_clicked();

    void on_pushButton_cancel_clicked();

private:
    Ui::DeleteFrameDialog *ui;
};

#endif // DELETEFRAMEDIALOG_H
