#include "demux.h"

#include <iostream>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libavcodec/avcodec.h>
};
Demux::Demux()
{
    static bool isFirst = true;
    static std::mutex dmutex;
    dmutex.lock();
    if(isFirst)
    {
        //初始化封装库
        avdevice_register_all();//注册所有组件
        //初始化网络库(可以打开rtsp rtmp http协议的流媒体视频)
        avformat_network_init();
        isFirst = false;
    }
    dmutex.unlock();
}

Demux::~Demux()
{

}

static double r2d(AVRational& r)
{
    return r.den == 0 ? 0 :r.num / (double)r.den;
}

bool Demux::Open(const char* url)
{
    Close();
    //解封装上下文
    AVDictionary* opts = nullptr;//参数设置
    av_dict_set(&opts,"rtsp_transport","tcp",0);//设置rtsp流以tcp协议打开
    av_dict_set(&opts,"max_delay","500",0);

    mux.lock();

    int re = avformat_open_input(&ic,url,nullptr,&opts);
    if(re != 0)
    {
        mux.unlock();
        char buf[1024] = {};
        av_strerror(re,buf,sizeof(buf) - 1);
        std::cout << "open " << url << " failed!Error:" << buf << std::endl;
        return false;
    }
    std::cout << "open " << url << " successed!" << std::endl;

    //获取流信息
    re = avformat_find_stream_info(ic,0);

    //总时长 毫秒
    totalMs = ic->duration / (AV_TIME_BASE / 1000) ;
    std::cout << "totalMs = " << totalMs << std::endl;

    //打印视频流详细信息
    av_dump_format(ic,0,url,0);

    //通过接口获取视频流信息
    videoStream = av_find_best_stream(ic,AVMEDIA_TYPE_VIDEO,-1,-1,nullptr,0);
    AVStream* as = ic->streams[videoStream];
    this->width = as->codecpar->width;
    this->height = as->codecpar->height;

    std::cout << "codec_id = " << as->codecpar->codec_id << std::endl;
    //视频
    std::cout << "videoInfo:" << std::endl;
    std::cout << "videoStream:" << videoStream << std::endl;
    std::cout << "format = " << as->codecpar->format << std::endl;
    std::cout << "\twidth = " << as->codecpar->width << std::endl;
    std::cout << "\theight = " << as->codecpar->height << std::endl;
    //帧率 fps
    std::cout << "\tfps = " << r2d(as->avg_frame_rate) << std::endl;

    //通过接口获取音频流信息
    audioStream = av_find_best_stream(ic,AVMEDIA_TYPE_AUDIO,-1,-1,nullptr,0);
    as = ic->streams[audioStream];
    sampleRate = as->codecpar->sample_rate;
    channels = as->codecpar->channels;

    //音频
    std::cout << "codec_id = " << as->codecpar->codec_id << std::endl;
    std::cout << "audioInfo:" << std::endl;
    std::cout << "audioStream:" << audioStream << std::endl;
    std::cout << "\tsample_rate = " << as->codecpar->sample_rate << std::endl;
    std::cout << "\tchannels = " << as->codecpar->channels << std::endl;
    //一帧数据 一定量的样本数 单通道样本数
    std::cout << "\tframe_size = " << as->codecpar->frame_size << std::endl;
    //1024 * 2(双通道) * 2(字节) = 4096 fps = sample_rate / frame_size

    mux.unlock();
    return true;
}

AVPacket* Demux::Read()
{
    if(!ic)
    {
        return nullptr;
    }
    mux.lock();
    AVPacket* pkt = av_packet_alloc();
    //读取一帧，并分配对应空间
    int re = av_read_frame(ic,pkt);
    if(re != 0)
    {
        mux.unlock();
        av_packet_free(&pkt);
        return nullptr;
    }
    //pts转换为毫秒
    pkt->pts = pkt->pts * (1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
    pkt->dts = pkt->dts * (1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
    mux.unlock();
    //std::cout << pkt->pts << " ";
    return pkt;
}

AVPacket* Demux::ReadVideo()
{
    mux.lock();
    if(!ic)
    {
        mux.unlock();
        return nullptr;
    }
    mux.unlock();
    AVPacket* pkt = nullptr;
    //防止阻塞
    for(int i = 0 ; i< 20 ; i++)
    {
        pkt = Read();
        if(!pkt)
        {
            break;
        }
        if(pkt->stream_index == videoStream)
        {
            break;
        }
        av_packet_free(&pkt);
    }
    return pkt;
}

AVCodecParameters* Demux::CopyVPara()
{
    mux.lock();
    if(!ic)
    {
        mux.unlock();
        return nullptr;
    }
    AVCodecParameters* pa = avcodec_parameters_alloc();
    int re = avcodec_parameters_copy(pa,ic->streams[videoStream]->codecpar);
    if(re < 0)
    {
        mux.unlock();
        return nullptr;
    }
    mux.unlock();
    return pa;
}

AVCodecParameters* Demux::CopyAPara()
{
    mux.lock();
    if(!ic)
    {
        mux.unlock();
        return nullptr;
    }
    AVCodecParameters* pa = avcodec_parameters_alloc();
    int re = avcodec_parameters_copy(pa,ic->streams[audioStream]->codecpar);
    if(re < 0)
    {
        mux.unlock();
        return nullptr;
    }
    mux.unlock();
    return pa;
}

bool Demux::Seek(double pos)
{
    mux.lock();
    if(!ic)
    {
        mux.unlock();
        return false;
    }
    //清理读取缓冲
    avformat_flush(ic);
    long long seekPos = 0;
    seekPos = ic->streams[videoStream]->duration * pos;
    int re = av_seek_frame(ic,videoStream,seekPos,AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    mux.unlock();
    if(re < 0 )
    {
        return false;
    }
    return true;
}

void Demux::Clear()
{
    mux.lock();
    if(!ic)
    {
        mux.unlock();
        return;
    }
    //清理读取缓冲
    avformat_flush(ic);
    mux.unlock();
}

void Demux::Close()
{
    mux.lock();
    if(!ic)
    {
        mux.unlock();
        return;
    }
    avformat_close_input(&ic);
    totalMs = 0;
    mux.unlock();
}

bool Demux::IsAudio(AVPacket* pkt)
{
    if(!pkt)
        return false;
    if(pkt->stream_index == videoStream)
        return false;
    return true;
}
