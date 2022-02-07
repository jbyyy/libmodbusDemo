#include "mainwindow.h"
#include "ui_mainwindow.h"



#if _MSC_VER >= 1600                        //中文乱码
#pragma execution_character_set("utf-8")
#endif
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Serial_search_function();

    m_slave = new modbusSlaveThread(1);
    connect(this,&MainWindow::signal_connectSlave,m_slave,&modbusSlaveThread::connnectModbusSlave);
    connect(this,&MainWindow::signal_disconnectSlave,m_slave,&modbusSlaveThread::disconnnectModbusSlave);
    m_poll = new modbusPollThread(1);
    connect(this,&MainWindow::signal_connectPoll,m_poll,&modbusPollThread::connnectModbusPoll);
    connect(this,&MainWindow::signal_disconnectPoll,m_poll,&modbusPollThread::disconnnectModbusPoll);
    connect(m_poll,&modbusPollThread::SIGNAL_SENDGETSLAVEINFO,this,&MainWindow::onShowSlaveInfo);

}

MainWindow::~MainWindow()
{
    delete m_slave;
    delete ui;
}

void MainWindow::Serial_search_function()
{
    QStringList newPortStringList;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    {
        newPortStringList += info.portName();
    }
    //更新串口号
    if(newPortStringList.size() != oldPortStringList.size())
    {
        oldPortStringList = newPortStringList;
        ui->comboBox_name_SLAVE->clear();
        ui->comboBox_name_SLAVE->addItems(oldPortStringList);
        ui->comboBox_name_POLL->clear();
        ui->comboBox_name_POLL->addItems(oldPortStringList);
    }
}

void MainWindow::on_pushButton_Poll_clicked()
{
    if(ui->pushButton_Poll->text() == "开启主机")
    {
        if(ui->comboBox_name_POLL->currentText().isEmpty()==true)
        {
            ui->textEdit_POLL->setText(ui->textEdit_POLL->toPlainText().append("未设置设备号\r\n"));
            ui->textEdit_POLL->moveCursor(QTextCursor::End);        //textedit 滚动条自动往下滚动
            return;
        }
        QString namestring = ui->comboBox_name_POLL->currentText();
        uint modbus_baud = ui->comboBox_baud_POLL->currentText().toUInt();

        emit signal_connectPoll(namestring,modbus_baud);
        ui->pushButton_Poll->setText("关闭主机");
    }else {
        emit signal_disconnectPoll();
        ui->pushButton_Poll->setText("开启主机");
    }
}
void MainWindow::on_pushButton_Slave_clicked()
{
    if(ui->pushButton_Slave->text() == "开启从机")
    {
        if(ui->comboBox_name_SLAVE->currentText().isEmpty()==true)
        {
            ui->textEdit_SLAVE->setText(ui->textEdit_SLAVE->toPlainText().append("未设置设备号\r\n"));
            ui->textEdit_SLAVE->moveCursor(QTextCursor::End);        //textedit 滚动条自动往下滚动
            return;
        }
        QString namestring = ui->comboBox_name_SLAVE->currentText();
        uint modbus_baud = ui->comboBox_baud_SLAVE->currentText().toUInt();

        emit signal_connectSlave(namestring,modbus_baud);
        ui->pushButton_Slave->setText("关闭从机");
    }else {
        emit signal_disconnectSlave();
        ui->pushButton_Slave->setText("开启从机");
    }
}

//清空主机打印信息
void MainWindow::on_pushButton_2_clicked()
{
    ui->textEdit_POLL->clear();
}
//清空从机打印信息
void MainWindow::on_pushButton_3_clicked()
{
    ui->textEdit_SLAVE->clear();
}

void MainWindow::onShowSlaveInfo(QString info)
{
    ui->textEdit_POLL->setText(ui->textEdit_POLL->toPlainText().append(info + "\r\n"));
    ui->textEdit_POLL->moveCursor(QTextCursor::End);        //textedit 滚动条自动往下滚动
}

