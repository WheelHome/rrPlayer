#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H
#include "decodethread.h"
struct AVCodecParameters;
class AudioPlay;
class Resample;
class AudioThread : public DecodeThread
{
public:
    //当前音频播放的pts
    long long pts = 0;
    AudioThread();
    virtual ~AudioThread();
    void run() override;
    //打开，不管成功与否都清理
    virtual bool Open(AVCodecParameters* para,int sampleRate,int channels);
    //停止线程，清理资源
    virtual void Close()override;
protected:
    AudioPlay* ap = nullptr;
    Resample* res = nullptr;
    std::mutex amux;
    //最大队列
    bool isExit = false;
};

#endif // AUDIOTHREAD_H
