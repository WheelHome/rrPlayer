#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H
#include "videocall.h"
#include <mutex>
#include <QThread>
struct AVPacket;
struct AVCodecParameters;
class Decode;
//解码和显示视频
class VideoThread : public QThread
{
public:
    VideoThread();
    virtual ~VideoThread();
    void run() override;
    //打开，不管成功与否都清理AVCodecParameters
    virtual bool Open(AVCodecParameters* para,VideoCall* call,int width,int height);
    virtual void Push(AVPacket* pkt);

protected:
    std::list<AVPacket*> packs;
    Decode* decode = nullptr;
    VideoCall* call = nullptr;
    std::mutex mux;
    //最大队列
    size_t maxList = 100;
    bool isExit = false;
};

#endif // VIDEOTHREAD_H
