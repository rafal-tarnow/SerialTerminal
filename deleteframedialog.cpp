#include "deleteframedialog.h"
#include "ui_deleteframedialog.h"
#include <QMessageBox>

DeleteFrameDialog::DeleteFrameDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeleteFrameDialog)
{
    ui->setupUi(this);
}

DeleteFrameDialog::~DeleteFrameDialog()
{
    delete ui;
}

void DeleteFrameDialog::show(map<string, QString > ramki){
    map<string,QString >::iterator it;

    ui->comboBox->clear();

    QString napis;

    for(it = ramki.begin(); it != ramki.end(); it++) {
        napis = QString::fromStdString(it->first);
        ui->comboBox->addItem(napis);
    }
    ui->comboBox->setCurrentIndex(1);
    QWidget::show();
}

void DeleteFrameDialog::on_pushButton_delete_clicked()
{
    string deletedFrameName;
    QMessageBox msgBox;
    msgBox.setText("Delete frame.");
    msgBox.setInformativeText("Are you sure to delete frame?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if(ret == QMessageBox::Yes){
        deletedFrameName = ui->comboBox->currentText().toStdString();
        emit frameDeleted(deletedFrameName);
    }
    this->close();
}

void DeleteFrameDialog::on_pushButton_cancel_clicked()
{
    this->close();
}
