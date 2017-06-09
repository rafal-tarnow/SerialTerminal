#ifndef DATA_CONNECTION_HPP
#define DATA_CONNECTION_HPP

#include <QObject>
#include <QSerialPort>
#include <QUdpSocket>
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
    QString errorString();
    bool isOpen();

signals:
    void onReceivedData(QByteArray data);

private slots:
    void socketReadyRead();
    void dataFromSerialPort();

private:
    SettingsDialog::ConnectionType connctionType = SettingsDialog::SerialPortConnection;
    SettingsDialog::Settings mSettings;
    QSerialPort * serialPort;
    QUdpSocket * socket;
};

#endif // DATA_CONNECTION_HPP

