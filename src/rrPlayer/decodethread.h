#ifndef DECODETHREAD_H
#define DECODETHREAD_H

#include <mutex>
#include <QThread>
struct AVPacket;
class Decode;
class DecodeThread : public QThread
{
public:

    DecodeThread();
    ~DecodeThread();
    virtual void Push(AVPacket* pkt);
    //取出一帧数据,并出栈,如果没有，返回null
    virtual AVPacket* Pop();
    //清理队列
    virtual void Clear();
    //清理资源，停止线程
    virtual void Close();
protected:
    std::list<AVPacket*> packs;
    Decode* decode = nullptr;
    //最大队列
    size_t maxList = 100;
    std::mutex mux;
    bool isExit = false;
};

#endif // DECODETHREAD_H
