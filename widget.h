#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "settingsdialog.hpp"
#include "newframe.h"
#include "deleteframedialog.h"
#include "newlinepolicydialog.h"
#include "connection.hpp"
#include <QSerialPort>
#include <QMap>
#include <map>
#include <QTimer>
#include <vector>


using namespace std;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void newFrame(string frameName, QString frameData);
    void onNewLineSignChanged(unsigned char);
    void onDialogFrameDeleted(string);
    void updateComboBox();
    void saveFrames();
    void readAndSetWindowDimmentions();
    void saveWindowDimmentions();
    void readNewLineSign();
    void saveNewLineSign();
    void readFrames();
    void dataFromSerial(QByteArray);
    void openConnection();
    void closeConnection();
    void on_pushButton_open_clicked();
    void on_pushButton_configure_clicked();

    void on_pushButton_send_clicked();

    void on_pushButton_clicked();

    void on_pushButton_addFrame_clicked();


    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_pushButton_delFrame_clicked();

    void timerEventDodajTekst();
    void sendByTimerSlot();
    void on_pushButton_newLinePolicy_clicked();

    void on_checkBox_sendByTimer_clicked(bool checked);

private:
    typedef enum{
        RX_DATA,
        TX_DATA
    }DataType;

    QTimer * sendByTimer;

    list<pair<DataType, QByteArray>> linie;
    unsigned char currentNewLineSign = 0xFF;
    QTimer * timerDodawaniaTekstu;
    map<string,QString > mapaRamek;
    QString normalStyleSheet;
    Ui::Widget *ui;
    SettingsDialog * settingsDialog;
    NewFrame * newFrameDialog;
    NewLinePolicyDialog * newLinePolicyDialog;
    DeleteFrameDialog * deleteFrameDialog;
    Connection * dataConnection;
    int tx_count = 0;
    int rx_count = 0;
};

#endif // WIDGET_H
