#include "resample.h"
#include <iostream>
extern "C"{
    #include <libswresample/swresample.h>
    #include <libavcodec/avcodec.h>
}

Resample::Resample()
{
}

Resample::~Resample()
{
}

//输出参数和输入参数一致,除了采样格式,输出为S16
bool Resample::Open(AVCodecParameters* para,bool isClearPara)
{
    mux.lock();
    //音频重采样
    actx = swr_alloc();
    actx = swr_alloc_set_opts(nullptr,
                              av_get_default_channel_layout(para->channels),//双声道输出格式
                              (AVSampleFormat)outFormat,    //输出样本格式
                              para->sample_rate, //输出采样率
                              av_get_default_channel_layout(para->channels), //输入声道数
                              (AVSampleFormat)para->format,  //输入样本格式
                              para->sample_rate, //输入样本率
                              0,0
                              );
    if(isClearPara)
    avcodec_parameters_free(&para);
    int re = swr_init(actx);
    mux.unlock();
    if(re != 0)
    {
        mux.unlock();
        char buf[1024] = {};
        av_strerror(re,buf,sizeof(buf) - 1);
        std::cout << " swr_init failed!Error:" << buf << std::endl;
        return -1;
    }
    //unsigned char* pcm = nullptr;
    return true;
}

void Resample::Close()
{
    mux.lock();
    if(actx)
        swr_free(&actx);
    mux.unlock();
}

int Resample::AudioResample(AVFrame* indata,unsigned char* d)
{
    if(!indata)
        return 0;
    if(!d)
    {
        av_frame_free(&indata);
        return 0;
    }
    //音频处理
    uint8_t* data[2] = {0};
    data[0] = d;
    int re = swr_convert(actx,
                data,   //输出数据
                indata->nb_samples, //输出样本数
                (const uint8_t**)indata->data,    //输入数据
                indata->nb_samples   //输入样本数
                );
    if(re <= 0)
    {
        av_frame_free(&indata);
        return re;
    }
    int outSize = re * indata->channels * av_get_bytes_per_sample((AVSampleFormat)outFormat);
    av_frame_free(&indata);
    return outSize;
}
