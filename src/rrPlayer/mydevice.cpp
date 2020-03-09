#include "mydevice.h"

#include <QDebug>

MyDevice::MyDevice(QByteArray pcm) : data_pcm(pcm)
{
    this->open(QIODevice::ReadOnly); // 为了解决QIODevice::read (QIODevice): device not open
    len_written = 0;
}

MyDevice::~MyDevice()
{
    this->close();
}

qint64 MyDevice::readData(char *data, qint64 maxlen) // data为声卡的数据缓冲区地址， maxlen为声卡缓冲区最大能存放的字节数
{
    if (len_written >= data_pcm.size())
        return 0;
    int len;

    //计算未播放的数据的长度
    len = (len_written+maxlen) > data_pcm.size() ? (data_pcm.size() - len_written) : maxlen;

    memcpy(data, data_pcm.data()+len_written, len); //把要播放的pcm数据存入声卡缓冲区里
    len_written += len; //更新已播放的数据长度
    return len;
}

qint64 MyDevice::writeData(const char *data, qint64 len)
{
}
