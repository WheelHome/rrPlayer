#include <iostream>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include "libavdevice/avdevice.h"
};

static double r2d(AVRational& r)
{
    return r.den == 0 ? 0 :r.num / (double)r.den;
}

int main()
{
    std::cout << "Test Demux!" << std::endl;
    const char* path = "test.mp4";
    //初始化封装库
    //av_register_all();//该接口已被弃用
    avdevice_register_all();//注册所有组件
    //初始化网络库(可以打开rtsp rtmp http协议的流媒体视频)
    avformat_network_init();

    //解封装上下文
    AVFormatContext* ic = nullptr;
    AVDictionary* opts = nullptr;//参数设置
    av_dict_set(&opts,"rtsp_transport","tcp",0);//设置rtsp流以tcp协议打开
    av_dict_set(&opts,"max_delay","500",0);
    int re = avformat_open_input(&ic,path,nullptr,&opts);
    if(re != 0)
    {
        char buf[1024] = {};
        av_strerror(re,buf,sizeof(buf) - 1);
        std::cout << "open " << path << " failed!Error:" << buf << std::endl;
        return -1;
    }
    std::cout << "open " << path << " successed!" << std::endl;

    //获取流信息
    re = avformat_find_stream_info(ic,0);

    //总时长 毫秒
    int totalMs = ic->duration / (AV_TIME_BASE / 1000) ;
    std::cout << "totalMs = " << totalMs << std::endl;

    //打印视频流详细信息
    av_dump_format(ic,0,path,0);

    //音视频索引，读取时区分音视频
    int videoStream = 0;
    int audioStream = 1;
    //获取音视频流信息(遍历、接口获取)
    for(unsigned int i = 0 ; i < ic->nb_streams ; i++)
    {
        AVStream* as = ic->streams[i];
        std::cout << "codec_id = " << as->codecpar->codec_id << std::endl;
        std::cout << "format = " << as->codecpar->format << std::endl;
        if(as->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audioStream = i;
            //音频
            std::cout << i << "音频信息" << std::endl;
            std::cout << "\tsample_rate = " << as->codecpar->sample_rate << std::endl;
            std::cout << "\tchannels = " << as->codecpar->channels << std::endl;
            //一帧数据 一定量的样本数 单通道样本数
            std::cout << "\tframe_size = " << as->codecpar->frame_size << std::endl;
            //1024 * 2(双通道) * 2(字节) = 4096 fps = sample_rate / frame_size

        }else
        {
            videoStream = i;
            //视频
            std::cout << i << "视频信息" << std::endl;
            std::cout << "\twidth = " << as->codecpar->width << std::endl;
            std::cout << "\theight = " << as->codecpar->height << std::endl;
            //帧率 fps
            std::cout << "\tfps = " << r2d(as->avg_frame_rate) << std::endl;
        }
    }

    //通过接口获取视频流信息
    videoStream = av_find_best_stream(ic,AVMEDIA_TYPE_AUDIO,-1,-1,nullptr,0);
    //通过接口获取音频流信息
    audioStream = av_find_best_stream(ic,AVMEDIA_TYPE_AUDIO,-1,-1,nullptr,0);

    if(ic)
    {
        avformat_close_input(&ic);//释放封装上下文并且将上下文置空

    }
    return 0;
}
