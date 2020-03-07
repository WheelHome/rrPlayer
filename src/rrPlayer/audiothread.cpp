#include "audiothread.h"
#include "audioplay.h"
#include "decode.h"
#include "resample.h"

#include <iostream>
AudioThread::AudioThread()
{
    if(!res)
    {
        res = new Resample();
    }
    if(!ap)
    {
        ap = AudioPlay::GetInstance();
    }
}

AudioThread::~AudioThread()
{
    //等待线程退出
    isExit = true;
    wait();
}

void AudioThread::run()
{
    unsigned char* pcm = new unsigned char[1024*1024*10];
    while(!isExit)
    {
        amux.lock();
        AVPacket* pkt = Pop();
        bool re = decode->Send(pkt);
        if(!re)
        {
            amux.unlock();
            msleep(1);
            continue;
        }
        //一次send，多次recv
        while(!isExit)
        {
            AVFrame* frame = decode->Recv();
            if(!frame)
            {
                break;
            }
            //显示缓冲中未播放的时间
            pts = decode->pts - ap->getNoPlayPts();
            //重采样
            int size = res->AudioResample(frame,pcm);
            //播放音频
            while(!isExit)
            {
                if(size <= 0)
                {
                    break;
                }
                //缓冲未播完,空间不够
                if(ap->GetFree() < size)
                {
                    msleep(1);
                    continue;
                }
                ap->Write(pcm,size);
                break;
            }
        }
        amux.unlock();
    }
}

bool AudioThread::Open(AVCodecParameters* para,int sampleRate,int channels)
{
    if(!para)
        return false;
    Clear();
    amux.lock();
    pts = 0;
    bool re = true;
    if(!res->Open(para,false))
    {
        std::cout << "Resample open filed!" << std::endl;
        re = false;
    }
    ap->sampleRate = sampleRate;
    ap->channels = channels;
    if(!ap->Open())
    {
        re = false;
        std::cout << "AudioPlay open filed!" << std::endl;
    }

    if(!decode->Open(para))
    {
        re = false;
        std::cout << "Decode open filed!" << std::endl;
    }

    amux.unlock();
    if(re)
        std::cout << "AudioThread::Open successed!" << std::endl;

    return re;
}

void AudioThread::Close()
{
    DecodeThread::Close();
    if(res)
    {
        res->Close();
        amux.lock();
        delete  res;
        res = nullptr;
        amux.unlock();
    }
    if(ap)
    {
        ap->Close();
        amux.lock();
        ap = nullptr;
        amux.unlock();
    }
}
