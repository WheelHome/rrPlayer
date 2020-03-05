#include "decode.h"
extern "C"{
#include <libavcodec/avcodec.h>
}
#include <iostream>
Decode::Decode()
{

}

Decode::~Decode()
{

}

//打开解码器
bool Decode::Open(AVCodecParameters* para)
{
    if(!para)
        return false;
    Close();
    //找到视频解码器
    AVCodec* vcodec = avcodec_find_decoder(para->codec_id);
    if(!vcodec)
    {
        avcodec_parameters_free(&para);
        std::cout << "Can't find the codec id " << para->codec_id << std::endl;
        return false;
    }
    std::cout << "Find the codec id " << para->codec_id << std::endl;

    mux.lock();
    //创建解码器上下文
    codec = avcodec_alloc_context3(vcodec);

    //配置解码器上下文参数
    avcodec_parameters_to_context(codec,para);
    avcodec_parameters_free(&para);

    //八线程解码
    codec->thread_count = 8;

    //打开解码器上下文
    int re = avcodec_open2(codec,nullptr,nullptr);
    if(re != 0)
    {
        avcodec_free_context(&codec);
        mux.unlock();
        char buf[1024] = {};
        av_strerror(re,buf,sizeof(buf) - 1);
        std::cout << "video avcodec_open2 failed!Error:" << buf << std::endl;
        return false;
    }
    mux.unlock();
    std::cout << "video avcodec_open2 successed!" << std::endl;
    return true;
}

//关闭解码器
void Decode::Close()
{
    mux.lock();
    if(codec)
    {
        avcodec_close(codec);
        avcodec_free_context(&codec);
    }
    mux.unlock();
}

//清理解码器
void Decode::Clear()
{
    mux.lock();
    //清理解码缓冲
    if(codec)
    {
        avcodec_flush_buffers(codec);
    }
    mux.unlock();
}

//发送到解码线程，不管成功与否都释放pkt空间(对象和媒体内容)
bool Decode::Send(AVPacket* pkt)
{
    if(!pkt || pkt->size <=0 || !pkt->data)
        return false;
    mux.lock();
    if(!codec)
    {
        mux.unlock();
        return false;
    }
    int re = avcodec_send_packet(codec,pkt);
    mux.unlock();
    av_packet_free(&pkt);
    if(re != 0)
        return false;
    return true;
}

//获取解码数据,一次send可能需要多次Recv，获取缓冲中搞得数据Send Null再Recv多次
//每次复制一份，由调用者释放av_frame_free
AVFrame* Decode::Recv()
{
    mux.lock();
    if(!codec)
    {
        mux.unlock();
        return nullptr;
    }
    AVFrame* frame = av_frame_alloc();
    int re = avcodec_receive_frame(codec,frame);
    mux.unlock();
    if(re != 0)
    {
        av_frame_free(&frame);
        return nullptr;
    }
    //std::cout << "[" << frame->linesize[0] << "] " << std::flush;
    return frame;
}
