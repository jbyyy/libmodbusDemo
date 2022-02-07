#include "modbuspollthread.h"
#define READSTARTADDR 40030
modbusPollThread::modbusPollThread(int slaveID, QObject *parent)
{
    qDebug() << "modbusPollThread" << QThread::currentThreadId();
    m_slaveId = slaveID;        //从机ID
    isWork = false;             //modbus是否连接成功
    m_pollThread = new QThread();
    this->moveToThread(m_pollThread);
    connect(m_pollThread,SIGNAL(started()),this,SLOT(initPollThread()));
    connect(m_pollThread,SIGNAL(finished()),this,SLOT(ClosePollThread()));
    m_pollThread->start();
}

modbusPollThread::~modbusPollThread()
{
    m_pollThread->quit();
    m_pollThread->wait();
    qDebug() << "~modbusPollThread" << QThread::currentThreadId();
}

void modbusPollThread::initPollThread()
{
    qDebug() << "initPollThread" << QThread::currentThreadId();
    Time_one = new QTimer();
    Time_one->stop();
    connect(Time_one,&QTimer::timeout,this,&modbusPollThread::modbus_Poll_work);                //modbus从机数据扫描时钟
}

void modbusPollThread::ClosePollThread()
{
    qDebug() << "ClosePollThread" << QThread::currentThreadId();
    Time_one->stop();
    delete Time_one;
    if(isWork)
    {
        modbus_close(my_bus);
        modbus_free(my_bus);
    }
}
uint16_t modbus_hold_reg[100];      //缓存读取到的数据
void modbusPollThread::modbus_update_text()
{
    int readNum = 5;
    modbus_set_slave(my_bus,m_slaveId);//设置需要连接的从机地址

    int ret = modbus_read_registers(my_bus,40030,readNum,modbus_hold_reg);            //读取保持寄存器的第0位开始的前5位
    //modbus_read_input_registers(my_bus,0,50,modbus_input_reg);   //读取输入寄存器的第0位开始的前5位
    QString err;
    if(ret != readNum)      //读取错误
    {
        if( ret < 0 )
        {
            if(
    #ifdef WIN32
                    errno == WSAETIMEDOUT ||
    #endif
                    errno == EIO
                                                                    )
            {
                err += tr( "I/O error" );
                err += ": ";
                err += tr( "did not receive any data from slave." );
            }
            else
            {
                err += tr( "Protocol error" );
                err += ": ";
                err += tr( "Slave threw exception '" );
                err += modbus_strerror( errno );
                err += tr( "' or function not implemented." );
            }
        }
        else
        {
            err += tr( "Protocol error" );
            err += ": ";
            err += tr( "Number of registers returned does not "
                    "match number of registers requested!" );
        }
    }

    if( err.size() > 0 )
            emit SIGNAL_SENDGETSLAVEINFO("读取失败:" + err);
    else        //读取正常
    {
        QString info = QString("从机ID: %1 modbus read : %2  %3  %4  %5  %6 \r\n ")\
                .arg(QString::number(m_slaveId)).arg(modbus_hold_reg[0])\
             .arg(modbus_hold_reg[1]).arg(modbus_hold_reg[2])\
             .arg(modbus_hold_reg[3]).arg(modbus_hold_reg[4]);

        emit SIGNAL_SENDGETSLAVEINFO(info);
    }
}

void modbusPollThread::modbus_Poll_work()
{
    modbus_update_text();
}

void modbusPollThread::connnectModbusPoll(QString portName, int baud)
{
    qDebug() << "connnectModbusPoll" << QThread::currentThreadId();
    my_bus = modbus_new_rtu(portName.toLatin1().data(),baud,'N',8,1);   //无法验证串口端口是否被占用
    modbus_set_slave(my_bus,m_slaveId);                    //设置从机地址为1
    modbus_connect(my_bus);

    //寄存器map初始化
    mb_mapping = modbus_mapping_new(READSTARTADDR, READSTARTADDR,
                                    READSTARTADDR, READSTARTADDR); //依次设置 bits、input_bits、registers、input_registers寄存器的大小，他们默认起始地址均为0
    if (mb_mapping == NULL) {
        qDebug() << "connect fail" << (stderr, "Failed to allocate the mapping: %s\n",
                                       modbus_strerror(errno));
        modbus_free(my_bus);
        return;
    }

    modbus_register_monitor_add_item_fnc(my_bus, modbusPollThread::stBusMonitorAddItem);
    //modbus_register_monitor_raw_data_fnc(my_bus, modbusPollThread::stBusMonitorRawData);
    Time_one->start(1000);
    isWork = true;
}

void modbusPollThread::disconnnectModbusPoll()
{
    qDebug() << "disconnnectModbusPoll" << QThread::currentThreadId();
    Time_one->stop();
    modbus_close(my_bus);
    modbus_free(my_bus);
    isWork = false;
}


//modbus回调函数
void modbusPollThread::stBusMonitorAddItem( modbus_t * modbus, uint8_t isRequest, uint8_t slave, uint8_t func, uint16_t addr, uint16_t nb, uint16_t expectedCRC, uint16_t actualCRC )
{
    Q_UNUSED(modbus);
    qDebug() << "成功接受到从机回复" << "isRequest" << isRequest << "slave" << slave << "func" << func\
             << "addr" << addr << "nb" << nb << "expectedCRC" << expectedCRC  << "actualCRC" << actualCRC;
}

// 送成功/接受成功会到此回掉函数
void modbusPollThread::stBusMonitorRawData( modbus_t * modbus, uint8_t * data, uint8_t dataLen, uint8_t addNewline , uint8_t direction)
{
    Q_UNUSED(modbus);
    QString dump;
    for( int i = 0; i < dataLen; ++i )
    {
        dump += QString::asprintf( "%.2x ", data[i] );
    }
    //    if(direction == 0)
    //        qDebug() << "串口发送"  << "data" << dump;
    //    else
    //        qDebug() << "串口接受" << "data" << dump;
}
