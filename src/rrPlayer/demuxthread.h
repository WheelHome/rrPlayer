#ifndef DEMUXTHREAD_H
#define DEMUXTHREAD_H
#include "videocall.h"
#include <QThread>

class Demux;
class VideoThread;
class AudioThread;
class DemuxThread : public QThread
{
public:
    DemuxThread();
    virtual ~DemuxThread();
    //创建对象并打开
    virtual bool Open(const char* url,VideoCall* call);
    //启动所有线程
    virtual void Start();
    virtual void run() override;
    //关闭线程，清理资源
    virtual void Close();
    long long pts = 0;
    long long totalMs = 0;
protected:
    Demux* demux = nullptr;
    VideoThread* vt = nullptr;
    AudioThread* at = nullptr;
    std::mutex mux;
    bool isExit = false;
};

#endif // DEMUXTHREAD_H
