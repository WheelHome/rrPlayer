#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

#include <QThread>
struct AVCodecParameters;
struct AVPacket;
class Decode;
class AudioPlay;
class Resample;
class AudioThread : public QThread
{
public:
    //当前音频播放的pts
    long long pts = 0;
    AudioThread();
    virtual ~AudioThread();
    void run() override;
    //打开，不管成功与否都清理
    virtual bool Open(AVCodecParameters* para,int sampleRate,int channels);
    virtual void Push(AVPacket* pkt);
protected:
    Decode* decode = nullptr;
    AudioPlay* ap = nullptr;
    Resample* res = nullptr;
    std::list<AVPacket*> packs;
    std::mutex mux;
    //最大队列
    size_t maxList = 100;
    bool isExit = false;
};

#endif // AUDIOTHREAD_H
