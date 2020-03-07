#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H
#include "videocall.h"
#include "decodethread.h"
#include <mutex>
struct AVPacket;
struct AVCodecParameters;
class Decode;
//解码和显示视频
class VideoThread : public DecodeThread
{
public:
    long long synpts = 0;
    VideoThread();
    virtual ~VideoThread();
    void run() override;
    //打开，不管成功与否都清理AVCodecParameters 同步时间由外部传入
    virtual bool Open(AVCodecParameters* para,VideoCall* call,int width,int height);

protected:
    std::mutex vmux;
    VideoCall* call = nullptr;
    bool isExit = false;
};

#endif // VIDEOTHREAD_H
