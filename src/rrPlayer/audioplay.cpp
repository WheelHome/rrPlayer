#include "audioplay.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <mutex>
#include <QDebug>
class CAudioPlay : public AudioPlay
{
public:
    virtual bool Open();
    virtual void Close();
    virtual bool Write(const unsigned char* data,int dataSize);
    virtual int GetFree();
    virtual long long getNoPlayPts();
private:
    QAudioOutput* output = nullptr;
    QIODevice* io = nullptr;
    std::mutex mux;
};

bool CAudioPlay::Open()
{
    Close();
    QAudioFormat fmt;
    fmt.setSampleRate(sampleRate);
    fmt.setSampleSize(sampleSize);
    fmt.setChannelCount(channels);
    fmt.setCodec("audio/pcm");
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setSampleType(QAudioFormat::UnSignedInt);


    mux.lock();
    output = new QAudioOutput(fmt);
    output->setBufferSize(49600);
    io = output->start();//开始播放
    mux.unlock();
    if(io)
        return true;
    return false;
}

void CAudioPlay::Close()
{
    mux.lock();
    if(io)
    {
        io->close();
        io = nullptr;
    }
    if(output)
    {
        output->stop();
        delete output;
        output = nullptr;
    }
    mux.unlock();
}

bool CAudioPlay::Write(const unsigned char* data,int dataSize)
{
    if(!data || dataSize <= 0)
        return false;

    mux.lock();
    if(!output || !io)
    {
        return false;
    }
    int size = io->write((char*)data,dataSize);
    mux.unlock();
    if(dataSize != size)
        return false;
    return true;
}

int CAudioPlay::GetFree()
{
    mux.lock();
    if(!output)
    {
        mux.unlock();
        return 0;
    }
    int free = output->bytesFree();
    mux.unlock();
    return free;
}

long long CAudioPlay::getNoPlayPts()
{
    mux.lock();
    if(!output)
    {
        mux.unlock();
        return 0;
    }
    long long pts = 0;
    //还未播放的字节数
    double size = output->bufferSize() - output->bytesFree();
    //一秒音频字节大小
    double secSize = sampleRate * (sampleSize / 8) * channels;
    if(secSize <= 0)
    {
        pts = 0;
    }else
    {
        pts = (size / secSize) * 1000;
    }
    mux.unlock();
    return pts;
}

AudioPlay::AudioPlay()
{
}

AudioPlay::~AudioPlay()
{

}

AudioPlay* AudioPlay::GetInstance()
{
    static CAudioPlay play;
    return &play;
}
