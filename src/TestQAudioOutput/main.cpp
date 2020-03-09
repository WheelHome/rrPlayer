#include <QCoreApplication>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QThread>
#include <iostream>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QAudioFormat fmt;
    fmt.setSampleRate(48000);
    fmt.setSampleSize(8);
    fmt.setChannelCount(2);
    fmt.setCodec("audio/pcm");
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setSampleType(QAudioFormat::UnSignedInt);
    QAudioOutput* out = new QAudioOutput(fmt);
    QIODevice* io = out->start();//开始播放
    FILE* fp = fopen("out.pcm","rb+");
    int size = out->periodSize();
    char* buf = new char[size];
    while(!feof(fp))
    {
        if (out->bytesFree() < size)
        {
            QThread::msleep(20);
            continue;
        }
        int len = fread(buf,1,size,fp);
        if(len <= 0)
            break;
        io->write(buf,len);
    }
    fclose(fp);
    delete[] buf;
    buf = nullptr;
    delete out;
    return a.exec();
}
