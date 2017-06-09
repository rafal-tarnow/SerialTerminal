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

#include "settingsdialog.hpp"
#include "ui_settingsdialog.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QIntValidator>
#include <QLineEdit>
#include <QSettings>
#include <QNetworkInterface>
#include <QCoreApplication>

QT_USE_NAMESPACE

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SettingsDialog::SettingsDialog(QString organizationName, QString appName,QString settingsName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    mOrganizationName = organizationName;
    mAppName = appName;

    mSettingsName = settingsName;

    intValidator = new QIntValidator(0, 4000000, this);

    ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

    connect(ui->applyButton, SIGNAL(clicked()),
            this, SLOT(apply()));
    connect(ui->serialPortInfoListBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(showPortInfo(int)));
    connect(ui->baudRateBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(checkCustomBaudRatePolicy(int)));
    connect(ui->serialPortInfoListBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(checkCustomDevicePathPolicy(int)));



    ui->lineEdit_ip->setInputMask( "000.000.000.000" );
    ui->lineEdit_send_port->setInputMask("00000");
    ui->lineEdit_listen_port->setInputMask("00000");
    ui->lineEdit_ip->clear();
    ui->lineEdit_send_port->clear();
    ui->lineEdit_listen_port->clear();

    fillPortsParameters();
    fillPortsInfo();

    loadSavedSettings();

    updateSettings();

    QString ip_text;

    QList<QHostAddress> list = QNetworkInterface::allAddresses();

    for(int nIter=0; nIter<list.count(); nIter++)

    {
        if(!list[nIter].isLoopback())
            if (list[nIter].protocol() == QAbstractSocket::IPv4Protocol )
                ip_text += list[nIter].toString();

    }

    ui->groupBox_ethernet->setTitle("UDP Ethernet    (this machine IP: " + ip_text + ")");
}

void SettingsDialog::loadSavedSettings(){
    QSettings settings(mOrganizationName, mAppName);
    settings.beginGroup(mSettingsName);
    ui->baudRateBox->setCurrentIndex(settings.value("baudrateIndex").toInt());
    ui->dataBitsBox->setCurrentIndex(settings.value("dataBitsIndex").toInt());
    ui->parityBox->setCurrentIndex(settings.value("parityIndex").toInt());
    ui->stopBitsBox->setCurrentIndex(settings.value("stopBitsIndex").toInt());
    ui->flowControlBox->setCurrentIndex(settings.value("flowControlIndex").toInt());

    ui->lineEdit_listen_port->setText(settings.value("listen_port").toString());
    ui->lineEdit_send_port->setText(settings.value("send_port").toString());
    ui->lineEdit_ip->setText(settings.value("ip_addres").toString());

    if(settings.value("serialPortCheckBox").toBool()){
        ui->selectBox_serial_port->setChecked(true);
        ui->groupBox_ethernet->setChecked(false);
    }else{
        ui->selectBox_serial_port->setChecked(false);
        ui->groupBox_ethernet->setChecked(true);
    }
    settings.endGroup();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::saveDefaultSettings(){
    QSettings settings(mOrganizationName, mAppName);
    settings.beginGroup(mSettingsName);
    settings.setValue("baudrateIndex", ui->baudRateBox->currentIndex());
    settings.setValue("dataBitsIndex", ui->dataBitsBox->currentIndex());
    settings.setValue("parityIndex", ui->parityBox->currentIndex());
    settings.setValue("stopBitsIndex", ui->stopBitsBox->currentIndex());
    settings.setValue("flowControlIndex", ui->flowControlBox->currentIndex());
    settings.setValue("listen_port",ui->lineEdit_listen_port->text());
    settings.setValue("send_port",ui->lineEdit_send_port->text());
    settings.setValue("ip_addres", ui->lineEdit_ip->text());
    settings.setValue("serialPortCheckBox",ui->selectBox_serial_port->isChecked());
    settings.endGroup();
    settings.sync();
}

SettingsDialog::Settings SettingsDialog::getSettings() const
{
    return currentSettings;
}

void SettingsDialog::showPortInfo(int idx)
{
    if (idx == -1)
        return;

    QStringList list = ui->serialPortInfoListBox->itemData(idx).toStringList();
    ui->descriptionLabel->setText(tr("Description: %1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
    ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
    ui->serialNumberLabel->setText(tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr(blankString)));
    ui->locationLabel->setText(tr("Location: %1").arg(list.count() > 4 ? list.at(4) : tr(blankString)));
    ui->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr(blankString)));
    ui->pidLabel->setText(tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}

void SettingsDialog::apply()
{
    updateSettings();
    this->saveDefaultSettings();
    hide();
}

void SettingsDialog::checkCustomBaudRatePolicy(int idx)
{
    bool isCustomBaudRate = !ui->baudRateBox->itemData(idx).isValid();
    ui->baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        ui->baudRateBox->clearEditText();
        QLineEdit *edit = ui->baudRateBox->lineEdit();
        edit->setValidator(intValidator);
    }
}

void SettingsDialog::checkCustomDevicePathPolicy(int idx)
{
    bool isCustomPath = !ui->serialPortInfoListBox->itemData(idx).isValid();
    ui->serialPortInfoListBox->setEditable(isCustomPath);
    if (isCustomPath)
        ui->serialPortInfoListBox->clearEditText();
}

void SettingsDialog::fillPortsParameters()
{
    ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->baudRateBox->addItem(tr("Custom"));



    ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->dataBitsBox->setCurrentIndex(3);

    ui->parityBox->addItem(tr("None"), QSerialPort::NoParity);
    ui->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    ui->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopBitsBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    ui->flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
}

void SettingsDialog::fillPortsInfo()
{
    ui->serialPortInfoListBox->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        ui->serialPortInfoListBox->addItem(list.first(), list);
    }

    ui->serialPortInfoListBox->addItem(tr("Custom"));
}

void SettingsDialog::updateSettings()
{
    if(ui->selectBox_serial_port->isChecked()){
        currentSettings.name = ui->serialPortInfoListBox->currentText();

        if (ui->baudRateBox->currentIndex() == 4) {
            currentSettings.baudRate = ui->baudRateBox->currentText().toInt();
        } else {
            currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
                        ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
        }
        currentSettings.stringBaudRate = QString::number(currentSettings.baudRate);

        currentSettings.dataBits = static_cast<QSerialPort::DataBits>(
                    ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
        currentSettings.stringDataBits = ui->dataBitsBox->currentText();

        currentSettings.parity = static_cast<QSerialPort::Parity>(
                    ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
        currentSettings.stringParity = ui->parityBox->currentText();

        currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
                    ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
        currentSettings.stringStopBits = ui->stopBitsBox->currentText();

        currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
                    ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
        currentSettings.stringFlowControl = ui->flowControlBox->currentText();
    }

    if(ui->groupBox_ethernet->isChecked()){
        currentSettings.ip_adress.setAddress(ui->lineEdit_ip->text());
        currentSettings.send_port = (quint16)ui->lineEdit_send_port->text().toUInt();
        currentSettings.listen_port = (quint16)ui->lineEdit_listen_port->text().toUInt();
    }

    if(ui->selectBox_serial_port->isChecked()){
        currentSettings.connectionType = SerialPortConnection;
    }else{
        currentSettings.connectionType = EthernetConnection;
    }
}



void SettingsDialog::on_selectBox_serial_port_clicked(bool checked)
{
    if(checked){
        ui->groupBox_ethernet->setChecked(false);
    }

    if(!checked){
        ui->groupBox_ethernet->setChecked(true);
    }
}

void SettingsDialog::on_groupBox_ethernet_clicked(bool checked)
{
    if(checked){
        ui->selectBox_serial_port->setChecked(false);
    }

    if(!checked){
        ui->selectBox_serial_port->setChecked(true);
    }
}


