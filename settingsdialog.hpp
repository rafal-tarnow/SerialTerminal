/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QHostAddress>

QT_USE_NAMESPACE

QT_BEGIN_NAMESPACE

namespace Ui {
class SettingsDialog;
}

class QIntValidator;

QT_END_NAMESPACE

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    typedef enum{
        UDP_Connection,
        SerialPortConnection,
        TCP_Connection
    } ConnectionType;

    struct Settings {
        ConnectionType connectionType;
        /* Serial Port */
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        /* UDP */
        QHostAddress ip_adress_UDP;
        quint16 send_port_UDP;
        quint16 listen_port_UDP;
        /* TCP*/
        QHostAddress ip_adress_TCP;
        quint16 send_port_TCP;
        quint16 listen_port_TCP;
        bool serverApp;
    };


    explicit SettingsDialog(QString organizationName, QString appName, QString settingsName, QWidget *parent = 0);
    ~SettingsDialog();

    Settings getSettings() const;

private slots:
    void showPortInfo(int idx);
    void apply();
    void checkCustomBaudRatePolicy(int idx);
    void checkCustomDevicePathPolicy(int idx);

    void on_selectBox_serial_port_clicked(bool checked);
    void on_groupBox_ethernet_clicked(bool checked);
    void on_groupBox_TCP_clicked(bool checked);

    void on_radioButtonClient_clicked();
    void on_radioButtonServer_clicked();

private:
    void saveDefaultSettings();
    void loadSavedSettings();
    void fillPortsParameters();
    void fillPortsInfo();
    void updateSettings();

private:
    QString mOrganizationName, mAppName;
    QString mSettingsName;
    Ui::SettingsDialog *ui;
    Settings currentSettings;
    QIntValidator *intValidator;
};

#endif // SETTINGSDIALOG_HPP
