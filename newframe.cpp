#include "newframe.h"
#include "ui_newframe.h"
#include <QMessageBox>

NewFrame::NewFrame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewFrame)
{
    ui->setupUi(this);
}

NewFrame::~NewFrame()
{
    delete ui;
}

void NewFrame::show(QString comboBoxText){
    ui->lineEdit_frameData->setText(comboBoxText);
    ui->comboBox_newFrameFrameName->clear();
    ui->comboBox_newFrameFrameName->setFocus();
    QWidget::show();
}

void NewFrame::on_pushButton_newFrameCancel_clicked()
{
    this->close();
}

void NewFrame::on_pushButton_newFrameSave_clicked()
{
    QString text = ui->lineEdit_frameData->text();

    emit newFrameAdded(ui->comboBox_newFrameFrameName->currentText().toStdString(),text);

    this->close();
}

