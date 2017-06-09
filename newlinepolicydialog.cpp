#include "newlinepolicydialog.h"
#include "ui_newlinepolicydialog.h"

NewLinePolicyDialog::NewLinePolicyDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewLinePolicyDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("New Line Policy");
    for(int i = 0; i < 256; i++){
        ui->comboBox->addItem("0x" + QString::number((unsigned char)i,16));
    }
}

NewLinePolicyDialog::~NewLinePolicyDialog()
{
    delete ui;
}

void NewLinePolicyDialog::show(unsigned char currentNewLineSign){
    ui->comboBox->setCurrentIndex(ui->comboBox->findText("0x" + QString::number(currentNewLineSign,16)));
    QWidget::show();
}

void NewLinePolicyDialog::on_pushButton_cancel_clicked()
{
    this->close();
}

void NewLinePolicyDialog::on_pushButton_ok_clicked()
{
    emit newLineSignChanged(localNewLineSign);
    this->close();
}

void NewLinePolicyDialog::on_comboBox_currentIndexChanged(const QString &arg1)
{
    bool ok;
    localNewLineSign = (unsigned char)arg1.toInt(&ok,0);
}
