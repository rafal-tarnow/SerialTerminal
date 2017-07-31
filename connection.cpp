#include "connection.hpp"
#include <QMessageBox>

Connection::Connection(){
    serialPort = new QSerialPort();
    connect(serialPort,SIGNAL(readyRead()),this,SLOT(dataFromSerialPort()));

    UDP_socket = new QUdpSocket(this);

    TCP_server = new QTcpServer(this);
    connect(TCP_server,SIGNAL(newConnection()),this,SLOT(newConnection()));

    TCP_socket = new QTcpSocket(this);
    connect(TCP_socket,SIGNAL(connected()),this,SLOT(connected()));
    connect(TCP_socket,SIGNAL(disconnected()),this,SLOT(disconnected()));
    connect(TCP_socket,SIGNAL(readyRead()),this,SLOT(TCPReadyRead()));
    connect(TCP_socket,SIGNAL(bytesWritten(qint64)),this,SLOT(bytesWritten(qint64)));
    addSocket = true;
}

Connection::~Connection(){
    UDP_socket->close();
    delete UDP_socket;

    TCP_socket->close();
    delete TCP_socket;

    serialPort->close();
    delete serialPort;

    TCP_server->close();
    delete TCP_server;
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
    if(connctionType == SettingsDialog::SerialPortConnection)
    {
        qDebug() << "Serial" << endl;
        return serialPort->open(QIODevice::ReadWrite);
    }else if(connctionType == SettingsDialog::UDP_Connection)
    {
        qDebug() << "UDP" << endl;
        connect(UDP_socket, SIGNAL(readyRead()),this,SLOT(UDPReadyRead()));
        return UDP_socket->bind(/*targetAddress, */mSettings.listen_port_UDP, QAbstractSocket::ShareAddress);
    }
    else
    {
        qDebug() << "TCP:" << mSettings.ip_adress_TCP << mSettings.send_port_TCP << endl;
        qDebug() << mSettings.serverApp;

        if(mSettings.serverApp)
        {
            qDebug() << "Server";
            if(!TCP_server->listen(mSettings.ip_adress_TCP,mSettings.send_port_TCP))
            {
                qDebug() << "Serwer could not start!";
                return false;
            }
            else
            {
                qDebug() << "Server started";
                return true;
            }
        }
        else
        {
            qDebug() << "client";
            TCP_socket->connectToHost(mSettings.ip_adress_TCP,mSettings.send_port_TCP);
            return TCP_socket->waitForConnected(1000);
        }
    }
    return false;
}

void Connection::newConnection()
{

    qDebug() << TCP_socket->state();
    //if(addSocket)
    if (TCP_socket->state() == QAbstractSocket::UnconnectedState)
    {
        TCP_socket = TCP_server->nextPendingConnection();
        connect(TCP_socket,SIGNAL(readyRead()),this,SLOT(TCPReadyRead()));
        TCP_socket->write("hello client\r\n");
        TCP_socket->flush();
        qDebug() << TCP_socket->state();
        TCP_socket->waitForBytesWritten(3000);
        qDebug() << TCP_socket->state();
        //qDebug() << "wait: " << TCP_socket->waitForDisconnected(2000);
        addSocket = false;
    }
    else
    {
        QTcpSocket *tmtSocket;
        tmtSocket = TCP_server->nextPendingConnection();
        tmtSocket->close();
    }
}

void Connection::closeConnection(){
    if(connctionType == SettingsDialog::SerialPortConnection){
        serialPort->close();
    }else if(connctionType == SettingsDialog::UDP_Connection){
        disconnect(UDP_socket, SIGNAL(readyRead()),this,SLOT(UDPReadyRead()));
        UDP_socket->close();
    }
    else{
        disconnect(TCP_socket,SIGNAL(readyRead()),this,SLOT(TCPReadyRead()));
        TCP_socket->close();
        TCP_server->close();
        qDebug() << "TCP close" << endl;
    }
}

void Connection::sendData(QByteArray data){
    if(connctionType == SettingsDialog::SerialPortConnection){
        serialPort->write(data);
    }else if(connctionType == SettingsDialog::UDP_Connection){
        UDP_socket->writeDatagram(data, mSettings.ip_adress_UDP,mSettings.send_port_UDP);
    }
    else{
        TCP_socket->write(data);
    }
}

void Connection::sendData(char * data, int size){
    if(connctionType == SettingsDialog::SerialPortConnection){
        serialPort->write(data,size);
    }else if(connctionType == SettingsDialog::UDP_Connection){
        UDP_socket->writeDatagram(data,size, mSettings.ip_adress_UDP,mSettings.send_port_UDP);
    }
    else{
        TCP_socket->write(data,size);
    }
}

QString Connection::errorString(){
    if(connctionType == SettingsDialog::SerialPortConnection){
        return serialPort->errorString();
    }else if(connctionType == SettingsDialog::UDP_Connection){
        return UDP_socket->errorString();
    }
    else{
        return TCP_socket->errorString();
    }
}

bool Connection::isOpen()
{
    if(connctionType == SettingsDialog::SerialPortConnection)
    {
        return serialPort->isOpen();
    }
    else if(connctionType == SettingsDialog::UDP_Connection)
    {
        if(QAbstractSocket::BoundState == UDP_socket->state())
            return true;
        else
            return false;
    }
    else
    {
        return TCP_socket->isOpen();
    }
    qDebug() << "return False(isOpen)";
    return false;
}

bool Connection::isServer()
{
    return mSettings.serverApp;
}

bool Connection::isTCP()
{
    if(mSettings.connectionType == SettingsDialog::TCP_Connection)
        return true;
    else
        return false;
}

void Connection::UDPReadyRead(){
    QByteArray buffer;
    buffer.resize(UDP_socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    UDP_socket->readDatagram(buffer.data(), buffer.size(),&sender, &senderPort);

    emit onReceivedData(buffer);
}

void Connection::TCPReadyRead()
{
    emit onReceivedData(TCP_socket->readAll());
}

void Connection::dataFromSerialPort()
{

    emit onReceivedData(serialPort->readAll());
}

void Connection::connected()
{
    qDebug() << "Connected!";
}

void Connection::disconnected()
{
    qDebug() << "Disconnected!";
    emit closeSocket();
}

void Connection::bytesWritten(qint64 bytes)
{
    qDebug() << "We wrote: " << bytes;
}
