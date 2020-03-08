#ifndef DEMUX_H
#define DEMUX_H

#include <mutex>
struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;
class Demux
{
public:
    //媒体总时长(毫秒)
    int totalMs = 0;
    int width = 0;
    int height = 0;
    //音频参数
    int sampleRate = 0;
    int channels = 0;

    Demux();
    virtual ~Demux();
    //打开媒体文件，或者流媒体 rtmp http rstp
    virtual bool Open(const char* url);

    //空间需要调用者释放,释放AVPacket对象空间和数据av_packet_free空间
    virtual AVPacket* Read();
    //只读视频帧，音频帧丢弃
    virtual AVPacket* ReadVideo();

    //获取视频参数 返回的空间需要清理 avcodec_parameters_free
    virtual AVCodecParameters* CopyVPara();
    //获取音频参数 返回的空间需要清理
    virtual AVCodecParameters* CopyAPara();

    //seek 位置　pos in 0.0~1.0
    virtual bool Seek(double pos);

    //清空读取缓存
    virtual void Clear();
    virtual void Close();

    virtual bool IsAudio(AVPacket* pkt);
protected:
    std::mutex mux;
    AVFormatContext* ic = nullptr;
    //音视频索引，读取时区分音视频
    int videoStream = 0;
    int audioStream = 1;
};

#endif // DEMUX_H
