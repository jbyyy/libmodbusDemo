#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "modbusPollThread.h"
#include "modbusslavethread.h"

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QTimer>
#include <QStringList>
#include <QThread>


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void signal_connectSlave(QString portName,uint baud);
    void signal_disconnectSlave();
    void signal_connectPoll(QString portName,uint baud);
    void signal_disconnectPoll();
private slots:
    void Serial_search_function();

    void on_pushButton_2_clicked();

    void on_pushButton_Poll_clicked();

    void on_pushButton_Slave_clicked();

    void on_pushButton_3_clicked();

    //显示POLL向从机获取到的数据信息
    void onShowSlaveInfo(QString);
private:
    Ui::MainWindow *ui;

    QStringList oldPortStringList;

    modbusSlaveThread *m_slave;

    modbusPollThread  *m_poll;
};


#endif // MAINWINDOW_H
