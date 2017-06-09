#include "connection.hpp"


Connection::Connection(){
    serialPort = new QSerialPort();
    connect(serialPort,SIGNAL(readyRead()),this,SLOT(dataFromSerialPort()));

    socket = new QUdpSocket(this);

}

Connection::~Connection(){
    socket->close();
    delete socket;

    serialPort->close();
    delete serialPort;
}

void Connection::setSettings(SettingsDialog::Settings settings){
    mSettings = settings;

    serialPort->setPortName(settings.name);
    serialPort->setBaudRate(settings.baudRate);
    serialPort->setDataBits(settings.dataBits);
    serialPort->setParity(settings.parity);
    serialPort->setStopBits(settings.stopBits);
    serialPort->setFlowControl(settings.flowControl);

    connctionType = settings.connectionType;
}

bool Connection::openConnection(){
    if(connctionType == SettingsDialog::SerialPortConnection){
        return serialPort->open(QIODevice::ReadWrite);
    }else{
        connect(socket, SIGNAL(readyRead()),this,SLOT(socketReadyRead()));
        return socket->bind(/*targetAddress, */mSettings.listen_port, QAbstractSocket::ShareAddress);
    }
    return false;
}

void Connection::closeConnection(){
    if(connctionType == SettingsDialog::SerialPortConnection){
        serialPort->close();
    }else{
        disconnect(socket, SIGNAL(readyRead()),this,SLOT(socketReadyRead()));
        socket->close();
    }
}

void Connection::sendData(QByteArray data){
    if(connctionType == SettingsDialog::SerialPortConnection){
        serialPort->write(data);
    }else{
        socket->writeDatagram(data, mSettings.ip_adress,mSettings.send_port);
    }
}

void Connection::sendData(char * data, int size){
    if(connctionType == SettingsDialog::SerialPortConnection){
        serialPort->write(data,size);
    }else{
        socket->writeDatagram(data,size, mSettings.ip_adress,mSettings.send_port);
    }
}

QString Connection::errorString(){
    if(connctionType == SettingsDialog::SerialPortConnection){
        return serialPort->errorString();
    }else{
        return socket->errorString();
    }
}

bool Connection::isOpen(){
    if(connctionType == SettingsDialog::SerialPortConnection){
        return serialPort->isOpen();
    }else{
        if(QAbstractSocket::BoundState == socket->state()){
            return true;
        }else{
            return false;
        }

    }
    return false;
}

void Connection::socketReadyRead(){
    QByteArray buffer;
    buffer.resize(socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    socket->readDatagram(buffer.data(), buffer.size(),&sender, &senderPort);

    emit onReceivedData(buffer);
}

void Connection::dataFromSerialPort(){

    emit onReceivedData(serialPort->readAll());
}
