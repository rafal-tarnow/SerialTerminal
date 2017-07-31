#ifndef DATA_CONNECTION_HPP
#define DATA_CONNECTION_HPP

#include <QObject>
#include <QSerialPort>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include "settingsdialog.hpp"

class Connection: public QObject{
    Q_OBJECT
public:
    Connection();
    ~Connection();
    void setSettings(SettingsDialog::Settings mSettings);

    bool openConnection();
    void closeConnection();
    void sendData(QByteArray data);
    void sendData(char * data, int size);
    bool isOpen();
    bool isServer();
    bool isTCP();
    QString errorString();

signals:
    void onReceivedData(QByteArray data);
    void onTCPReceivedData(QByteArray data);
    void closeSocket();

private slots:
    void dataFromSerialPort();
    void UDPReadyRead();
    void TCPReadyRead();
    void newConnection();
    void connected();
    void disconnected();
    void bytesWritten(qint64);

private:
    SettingsDialog::ConnectionType connctionType = SettingsDialog::SerialPortConnection;
    SettingsDialog::Settings mSettings;
    QSerialPort * serialPort;
    QUdpSocket * UDP_socket;
    QTcpServer * TCP_server;
    QTcpSocket * TCP_socket;
    bool addSocket;

};

#endif // DATA_CONNECTION_HPP
