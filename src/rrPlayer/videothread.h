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
    bool isPause = false;
    VideoThread();
    virtual ~VideoThread();
    void run() override;
    //打开，不管成功与否都清理AVCodecParameters 同步时间由外部传入
    virtual bool Open(AVCodecParameters* para,VideoCall* call,int width,int height);
    void SetPause(bool isPause);
    //解码pts,如果接收到的解码数据pts >= seekPts return true 并且显示画面
    virtual bool RepaintPts(AVPacket* pkt,long long seekPts);

protected:
    VideoCall* call = nullptr;
    std::mutex vmux;
    bool isExit = false;
};

#endif // VIDEOTHREAD_H
