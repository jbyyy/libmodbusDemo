#ifndef MODBUSSLAVETHREAD_H
#define MODBUSSLAVETHREAD_H

#include <QObject>
#include <QThread>
#include "modbus.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QTimer>
#include <QStringList>
#include <QThread>
class modbusSlaveThread : public QObject
{
    Q_OBJECT
public:
    explicit modbusSlaveThread(int ID=1,QObject *parent = nullptr);
    ~modbusSlaveThread();
    void Serial_search_function();
    void modbus_update_text();
    static void stBusMonitorAddItem( modbus_t * modbus,
                                     uint8_t isOut, uint8_t slave, uint8_t func, uint16_t addr,
                                     uint16_t nb, uint16_t expectedCRC, uint16_t actualCRC );

    //显示接受内容  在右上角
    static void stBusMonitorRawData( modbus_t * modbus, uint8_t * data,
                                     uint8_t dataLen, uint8_t addNewline,uint8_t );
signals:
    void SIGNAL_SENDGETPOLLINFO(QString);
public slots:
    void initSlaveThread();
    void CloseSlaveThread();
    void modbus_slave_work();

    void connnectModbusSlave(QString portName,int baud);
    void disconnnectModbusSlave();

private:
    QThread*    m_slvaeThread;
    bool        isWork;                     //是否连接成功
    modbus_t    *my_bus;                    //定义第一个modbus类
    modbus_mapping_t *mb_mapping = NULL;    //modbus相关的寄存器
    QTimer      *Time_one;                  //modbus数据更新时钟
    int         m_Id;                  //查询从机ID
};

#endif // MODBUSSLAVETHREAD_H
