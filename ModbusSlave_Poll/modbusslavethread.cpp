#include "modbusslavethread.h"

modbusSlaveThread::modbusSlaveThread(int ID, QObject *parent)
{
    m_Id = ID;
    qDebug() << "modbusSlaveThread" << QThread::currentThreadId();
    isWork = false;
    m_slvaeThread = new QThread();
    this->moveToThread(m_slvaeThread);
    connect(m_slvaeThread,SIGNAL(started()),this,SLOT(initSlaveThread()));
    connect(m_slvaeThread,SIGNAL(finished()),this,SLOT(CloseSlaveThread()));
    m_slvaeThread->start();
}

modbusSlaveThread::~modbusSlaveThread()
{
    m_slvaeThread->quit();
    m_slvaeThread->wait();
    qDebug() << "~modbusSlaveThread" << QThread::currentThreadId();
}

void modbusSlaveThread::initSlaveThread()
{
    qDebug() << "initSlaveThread" << QThread::currentThreadId();
    Time_one = new QTimer();
    Time_one->stop();
    connect(Time_one,&QTimer::timeout,this,&modbusSlaveThread::modbus_slave_work);                //modbus从机数据扫描时钟
}

void modbusSlaveThread::CloseSlaveThread()
{
    qDebug() << "CloseSlaveThread" << QThread::currentThreadId();
    Time_one->stop();
    delete Time_one;
    if(isWork)
    {
        modbus_close(my_bus);
        modbus_free(my_bus);
    }
}

void modbusSlaveThread::modbus_slave_work()
{
    int rc;
    //qDebug() << "modbus_slave_work" << QThread::currentThreadId();
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    //轮询接收数据，并做相应处理
    rc = modbus_receive(my_bus, query);
    if (rc > 0) {
        modbus_reply(my_bus, query, rc, mb_mapping);
    }
}

void modbusSlaveThread::connnectModbusSlave(QString portName, int baud)
{
    qDebug() << "connnectModbusSlave" << QThread::currentThreadId();
    my_bus = modbus_new_rtu(portName.toLatin1().data(),baud,'N',8,1);
    modbus_set_slave(my_bus,m_Id);                    //设置从机地址为1
    modbus_connect(my_bus);

    //寄存器map初始化
    mb_mapping = modbus_mapping_new(40099, 40099,
                                    40099, 40099); //依次设置 bits、input_bits、registers、input_registers寄存器的大小，他们默认起始地址均为0
    if (mb_mapping == NULL) {
        qDebug() << "connect fail" << (stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        modbus_free(my_bus);
        return;
    }

    mb_mapping->tab_registers[1] = 77; //设置一下hold寄存器的值
    mb_mapping->tab_registers[2] = 77;
    mb_mapping->tab_registers[3] = 3;
    mb_mapping->tab_registers[4] = 4;
    mb_mapping->tab_registers[5] = 5;

    //modbus_register_monitor_add_item_fnc(my_bus, modbusSlaveThread::stBusMonitorAddItem);
    modbus_register_monitor_raw_data_fnc(my_bus, modbusSlaveThread::stBusMonitorRawData);
    Time_one->start(300);
    isWork = true;
}

void modbusSlaveThread::disconnnectModbusSlave()
{
    qDebug() << "disconnnectModbusSlave" << QThread::currentThreadId();
    Time_one->stop();
    modbus_close(my_bus);
    modbus_free(my_bus);
    isWork = false;
}


// static   modbus回调函数
void modbusSlaveThread::stBusMonitorAddItem( modbus_t * modbus, uint8_t isRequest, uint8_t slave, uint8_t func, uint16_t addr, uint16_t nb, uint16_t expectedCRC, uint16_t actualCRC )
{
    Q_UNUSED(modbus);
    if(isRequest == 0)
    {
        qDebug() << "modbus解析成功" << "isRequest" << isRequest << "slave" << slave << "func" << func\
                 << "addr" << addr << "nb" << nb << "expectedCRC" << expectedCRC  << "actualCRC" << actualCRC;
    }
   // MyCommon::globalModbus->busMonitorAddItem( isRequest, slave, func, addr, nb, expectedCRC, actualCRC );
}

// static  发送成功/接受成功会到次回掉函数
void modbusSlaveThread::stBusMonitorRawData( modbus_t * modbus, uint8_t * data, uint8_t dataLen, uint8_t addNewline , uint8_t direction)
{
    Q_UNUSED(modbus);
    QString dump;
    for( int i = 0; i < dataLen; ++i )
    {
        dump += QString::asprintf( "%.2x ", data[i] );
    }
    if(direction == 0)
        qDebug() << "从机接受主机命令并执行成功"  << "data" << dump;
//    else
//        qDebug() << "串口接受" << "data" << dump;
   // MyCommon::globalModbus->busMonitorRawData( data, dataLen, addNewline != 0 , direction);
}
