#ifndef DECODE_H
#define DECODE_H

#include <mutex>
struct AVCodecParameters;
struct AVCodecContext;
struct AVPacket;
struct AVFrame;

extern void FreePacket(AVPacket** pkt);
extern void FreeFrame(AVFrame** pkt);
class Decode
{
public:
    bool isAudio = false;
    //当前解码到的pts
    long long pts = 0;

    Decode();
    virtual ~Decode();

    //打开解码器,不管成功与否都释放para空间
    virtual bool Open(AVCodecParameters* para);

    //关闭解码器
    virtual void Close();
    //清理解码器
    virtual void Clear();

    //发送到解码线程，不管成功与否都释放pkt空间(对象和媒体内容)
    virtual bool Send(AVPacket* pkt);
    //获取解码数据,一次send可能需要多次Recv，获取缓冲中搞得数据Send Null再Recv多次
    //每次复制一份，由调用者释放av_frame_free
    virtual AVFrame* Recv();
protected:
    AVCodecContext* codec = nullptr;
    std::mutex mux;
};

#endif // DECODE_H
