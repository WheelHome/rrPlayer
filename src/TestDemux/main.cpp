#include <iostream>
#include <thread>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
};

static double r2d(AVRational& r)
{
    return r.den == 0 ? 0 :r.num / (double)r.den;
}

void Sleep(int ms)
{
    std::chrono::milliseconds du(ms);
    std::this_thread::sleep_for(du);
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

    //注册解码器
    //avcodec_register_all();//该接口已经过时

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

        }else if(as->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
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
    videoStream = av_find_best_stream(ic,AVMEDIA_TYPE_VIDEO,-1,-1,nullptr,0);
    //通过接口获取音频流信息
    audioStream = av_find_best_stream(ic,AVMEDIA_TYPE_AUDIO,-1,-1,nullptr,0);

    //找到视频解码器
    AVCodec* vcodec = avcodec_find_decoder(ic->streams[videoStream]->codecpar->codec_id);
    if(!vcodec)
    {
        std::cout << "Can't find the codec id " << ic->streams[videoStream]->codecpar->codec_id << std::endl;
        return -1;
    }
    std::cout << "Find the codec id " << ic->streams[videoStream]->codecpar->codec_id << std::endl;
    //创建解码器上下文
    AVCodecContext* vcc = avcodec_alloc_context3(vcodec);
    //配置解码器上下文参数
    avcodec_parameters_to_context(vcc,ic->streams[videoStream]->codecpar);
    //八线程解码
    vcc->thread_count = 8;
    //打开解码器上下文
    re = avcodec_open2(vcc,nullptr,nullptr);
    if(re != 0)
    {
        char buf[1024] = {};
        av_strerror(re,buf,sizeof(buf) - 1);
        std::cout << "video avcodec_open2 " << path << " failed!Error:" << buf << std::endl;
        return -1;
    }
    std::cout << "video avcodec_open2 " << path << " successed!" << std::endl;

    //找到音频解码器
    AVCodec* acodec = avcodec_find_decoder(ic->streams[audioStream]->codecpar->codec_id);
    if(!vcodec)
    {
        std::cout << "Can't find the codec id " << ic->streams[audioStream]->codecpar->codec_id << std::endl;
        return -1;
    }
    std::cout << "Find the codec id " << ic->streams[audioStream]->codecpar->codec_id << std::endl;
    //创建解码器上下文
    AVCodecContext* acc = avcodec_alloc_context3(acodec);
    //配置解码器上下文参数
    avcodec_parameters_to_context(acc,ic->streams[audioStream]->codecpar);
    //八线程解码
    acc->thread_count = 8;
    //打开解码器上下文
    re = avcodec_open2(acc,nullptr,nullptr);
    if(re != 0)
    {
        char buf[1024] = {};
        av_strerror(re,buf,sizeof(buf) - 1);
        std::cout << "audio avcodec_open2 " << path << " failed!Error:" << buf << std::endl;
        return -1;
    }
    std::cout << "audio avcodec_open2 " << path << " successed!" << std::endl;

    AVPacket* pkt = av_packet_alloc(); //分配packet对象空间并初始化
    AVFrame* frame = av_frame_alloc();

    for(;;)
    {
        int re = av_read_frame(ic,pkt);
        if(re != 0)
        {
            //循环播放
            std::cout << "==========================end===========================" << std::endl;;
            int ms = 3000;
            //根据时间基数(分数)转换
            long long pos = (double)ms * r2d(ic->streams[pkt->stream_index]->time_base) * 1000;
            av_seek_frame(ic,videoStream,pos,AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
            //break;
            continue;
        }
        std::cout << "pkt->size = " << pkt->size << std::endl; //帧大小
        std::cout << "pkt->pts = " << pkt->pts << std::endl; //显示的时间 Presentation Time Stamp
        std::cout << "pkt->dts = " << pkt->dts << std::endl; //解码时间 Decoding Time Stamp
        //转换为毫秒 方便做同步
        std::cout << "pkt->pts ms = " << pkt->pts * (r2d(ic->streams[pkt->stream_index]->time_base) * 1000) << std::endl;

        AVCodecContext* cc = 0;
        if(pkt->stream_index == videoStream)
        {
            std::cout << "图像" << std::endl;
            cc = vcc;
        }
        if(pkt->stream_index == audioStream)
        {
            std::cout << "音频" << std::endl;
            cc= acc;
        }
        //解码视频
        //发送packet到解码线程 最后几帧的时候，send传nullptr后调用多次receive,取出所有缓冲帧
        re = avcodec_send_packet(cc,pkt);

        //引用计数-1 为0释放空间
        av_packet_unref(pkt);
        if(re != 0)
        {
            char buf[1024] = {};
            av_strerror(re,buf,sizeof(buf) - 1);
            std::cout << "audio avcodec_open2 " << path << " failed!Error:" << buf << std::endl;
            continue;
        }
        //从线程中获取解码接口　不占用cpu 一次send可能对应多次received
        for(;;)
        {
            re = avcodec_receive_frame(cc,frame);
            if(re != 0)
                break;
            std::cout << "Received frame " << frame->format << " " << frame->linesize[0] << std::endl;
        }

        //Sleep(500);
    }

    av_frame_free(&frame);
    av_packet_free(&pkt);

    if(ic)
    {
        avformat_close_input(&ic);//释放封装上下文并且将上下文置空

    }
    return 0;
}
