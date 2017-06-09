#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
#include <QByteArray>
#include <QDebug>
#include <QSettings>
#include <vector>
#include <QApplication>
#include <QDesktopWidget>
#include <QNetworkInterface>

using namespace std;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    QFont font("Monospace", 9, QFont::Normal);
    ui->textBrowser->setFont(font);
    this->setWindowTitle("Serial Terminal");

    normalStyleSheet =  ui->pushButton_open->styleSheet();

    timerDodawaniaTekstu = new QTimer();
    connect(timerDodawaniaTekstu,SIGNAL(timeout()),this,SLOT(timerEventDodajTekst()));

    sendByTimer = new QTimer();
    connect(sendByTimer,SIGNAL(timeout()),this,SLOT(sendByTimerSlot()));

    settingsDialog = new SettingsDialog("Reyfel", "Onion Terminal", "settingsDialog");
    newFrameDialog = new NewFrame();
    connect(newFrameDialog,SIGNAL(newFrameAdded(string,QString)),this,SLOT(newFrame(string,QString)));

    newLinePolicyDialog = new NewLinePolicyDialog();
    connect(newLinePolicyDialog,SIGNAL(newLineSignChanged(unsigned char)),this,SLOT(onNewLineSignChanged(unsigned char)));

    deleteFrameDialog = new DeleteFrameDialog();
    connect(deleteFrameDialog,SIGNAL(frameDeleted(string)),this,SLOT(onDialogFrameDeleted(string)));


    dataConnection = new Connection();
    connect(dataConnection,SIGNAL(onReceivedData(QByteArray)),this,SLOT(dataFromSerial(QByteArray)));


    readAndSetWindowDimmentions();
    readNewLineSign();
    readFrames();
    updateComboBox();

    ui->comboBox->setToolTip("Sample correct frame:\n \"0xFF 0b0111 0x15 0x2\"");

}

Widget::~Widget()
{
    dataConnection->closeConnection();
    saveWindowDimmentions();
    this->width();
    this->height();
    delete dataConnection;
    delete settingsDialog;
    delete newFrameDialog;
    delete deleteFrameDialog;
    delete newLinePolicyDialog;
    delete ui;
}

void Widget::newFrame(string frameName, QString frameData){
    qDebug() << "frameName = " << frameName.c_str();

    mapaRamek[frameName] = frameData;

    updateComboBox();
    saveFrames();
}

void Widget::onNewLineSignChanged(unsigned char sign){
    currentNewLineSign = sign;
    saveNewLineSign();
    qDebug() << "Widget new line sign changed = " << currentNewLineSign;
}

void Widget::onDialogFrameDeleted(string deletedFrameName){
    mapaRamek.erase (deletedFrameName);

    updateComboBox();
    saveFrames();
}

void Widget::updateComboBox(){

    map<string,QString >::iterator it;

    ui->comboBox->clear();

    QString napis;

    for(it = mapaRamek.begin(); it != mapaRamek.end(); it++) {
        napis = QString::fromStdString(it->first);
        ui->comboBox->addItem(napis);
    }
}

void Widget::saveFrames(){
    map<string,QString >::iterator it;


    QSettings settings("Reyfel", "Onion Terminal");

    settings.beginWriteArray("ramki");

    int k = 0;
    for(it = mapaRamek.begin(); it != mapaRamek.end(); it++) {

        settings.setArrayIndex(k);
        settings.setValue("frameName",QString::fromStdString(it->first));
        settings.setValue("frameData",it->second);

        k++;
    }

    settings.endArray();
    settings.sync();
}

void Widget::readAndSetWindowDimmentions(){
    int windowHeight;
    int windowWidth;
    QSettings settings("Reyfel", "Onion Terminal");
    windowHeight = settings.value("windowHeight").toInt();
    windowWidth = settings.value("windowWidth").toInt();

    if(windowHeight < 100){
        windowHeight = 320;
    }

    if(windowWidth < 100){
        windowWidth = 480;
    }

    QRect rec = QApplication::desktop()->screenGeometry();
    int screen_height = rec.height();
    int screen_width = rec.width();

    this->setGeometry(screen_width/2,screen_height/2,windowWidth,windowHeight);
}

void Widget::saveWindowDimmentions(){
    QSettings settings("Reyfel", "Onion Terminal");
    settings.setValue("windowHeight",this->height());
    settings.setValue("windowWidth",this->width());
    settings.sync();
}

void Widget::readNewLineSign(){
    QSettings settings("Reyfel", "Onion Terminal");
    currentNewLineSign = (unsigned char)settings.value("currentNewLineSign").toInt();
}

void Widget::saveNewLineSign(){
    QSettings settings("Reyfel", "Onion Terminal");
    settings.setValue("currentNewLineSign",(unsigned char)currentNewLineSign);
    settings.sync();
}


void Widget::readFrames(){
    map<string,QString >::iterator it;


    QSettings settings("Reyfel", "Onion Terminal");
    string frameName;
    QString frameData;


    int size = settings.beginReadArray("ramki");

    for(int i = 0; i < size; i++){
        settings.setArrayIndex(i);
        frameName = settings.value("frameName").toString().toStdString();
        frameData = settings.value("frameData").toString();

        mapaRamek[frameName] = frameData;
    }

    settings.endArray();
}



void Widget::dataFromSerial(QByteArray data){
    //rawBuffer += data;
    linie.push_back(make_pair(RX_DATA,data));
    rx_count++;
    ui->label_RX_count->setText("RX: "+QString::number(rx_count,10));
}

void Widget::openConnection(){
    dataConnection->setSettings(settingsDialog->getSettings());

    if (dataConnection->openConnection()) {
        ui->pushButton_open->setText("Close");
        ui->pushButton_open->setStyleSheet("background-color: green");
        timerDodawaniaTekstu->start(100);
    } else {
        QMessageBox::critical(this, tr("Error"), dataConnection->errorString());
    }


}

void Widget::closeConnection(){
    if (dataConnection->isOpen())
        dataConnection->closeConnection();

    ui->pushButton_open->setText("Open");
    ui->pushButton_open->setStyleSheet(normalStyleSheet);
    timerDodawaniaTekstu->stop();
}

void Widget::on_pushButton_open_clicked()
{
    if(dataConnection->isOpen()){
        this->closeConnection();
    }else{
        this->openConnection();
    }
}

void Widget::on_pushButton_configure_clicked()
{
    settingsDialog->show();
}

void Widget::on_pushButton_send_clicked()
{
    QString text = ui->comboBox->currentText();

    QStringList stringList = text.split(" ");

    QByteArray dataToSend;

    bool ok;

    QString templ = "0b";
    QString replace = "";
    QString nonConstString;

    for(int i = 0; i < stringList.size(); i++){
        nonConstString = stringList.at(i);

        if(nonConstString.startsWith("0x")){
            dataToSend.append((char)(nonConstString.toInt(&ok,0)));
        }else if(nonConstString.startsWith("0b")){
            nonConstString.replace(nonConstString.indexOf(templ),templ.size(),replace);
            dataToSend.append(nonConstString.toInt(&ok,2));
        }

    }


    // ui->lineEdit->clear();

    if(dataConnection->isOpen() ){
        dataConnection->sendData(dataToSend);
        linie.push_back(make_pair(TX_DATA,dataToSend));
        tx_count++;
        ui->label_TX_count->setText("TX: "+QString::number(tx_count,10));
    }
}

void Widget::on_pushButton_clicked()
{
    ui->textBrowser->clear();
}

void Widget::on_pushButton_addFrame_clicked()
{
    newFrameDialog->show(ui->comboBox->currentText());
}

void Widget::on_comboBox_currentIndexChanged(const QString &arg1)
{
    QString frameData;
    frameData = mapaRamek[arg1.toStdString()];

    ui->comboBox->setCurrentText(frameData);
}

void Widget::on_pushButton_delFrame_clicked()
{
    deleteFrameDialog->show(mapaRamek);
}

void Widget::timerEventDodajTekst(){

    QByteArray lineData;
     QString tmp;
     DataType dataType;

    while(!linie.empty()){

        dataType = linie.front().first;
        lineData = linie.front().second;
        linie.pop_front();

        if(!lineData.isEmpty()){




            if(dataType == RX_DATA){

                for(int i = 0; i < lineData.size(); i++){
                    if(lineData[i] == (char)currentNewLineSign){
                         tmp.append("<br>");
                    }
                    tmp.append(QString("%1 ").arg((unsigned char)lineData[i],2,16,QChar('0')));
                }
                tmp = "<font color=#00AA00>"+tmp+"</font>";

            }else if(dataType == TX_DATA){

                for(int i = 0; i < lineData.size(); i++){
                    tmp.append(QString("%1 ").arg((unsigned char)lineData[i],2,16,QChar('0')));
                }
                tmp = "<font color=#0000AA><br>TX: "+tmp+"<br></font>";


            }



        }
    }

    ui->textBrowser->insertHtml(tmp);
    ui->textBrowser->moveCursor(QTextCursor::End);

}

void Widget::sendByTimerSlot(){
    ui->pushButton_send->click();
}

void Widget::on_pushButton_newLinePolicy_clicked()
{
    newLinePolicyDialog->show(currentNewLineSign);
}

void Widget::on_checkBox_sendByTimer_clicked(bool checked)
{
    if(checked == true){
        sendByTimer->start(100);
    }else{
        sendByTimer->stop();
    }
}
