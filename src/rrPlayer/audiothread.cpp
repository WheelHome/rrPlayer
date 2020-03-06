#include "audiothread.h"
#include "audioplay.h"
#include "decode.h"
#include "resample.h"

#include <iostream>
AudioThread::AudioThread()
{
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
        mux.lock();
        if(packs.empty() || !decode || !res || !ap)
        {
            mux.unlock();
            msleep(1);
            continue;
        }
        AVPacket* pkt = packs.front();
        packs.pop_front();
        bool re = decode->Send(pkt);
        if(!re)
        {
            mux.unlock();
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

        mux.unlock();
    }
}

bool AudioThread::Open(AVCodecParameters* para,int sampleRate,int channels)
{
    if(!para)
        return false;
    mux.lock();
    pts = 0;
    if(!decode)
    {
        decode = new Decode();
    }
    if(!res)
    {
        res = new Resample();
    }
    if(!ap)
    {
        ap = AudioPlay::GetInstance();
    }
    bool re = true;
    if(!res->Open(para,false))
    {
        std::cout << "Resample open filed!" << std::endl;
        re = false;
        //mux.unlock();
        //return false;
    }

    ap->sampleRate = sampleRate;
    ap->channels = channels;
    if(!ap->Open())
    {
        re = false;
        std::cout << "AudioPlay open filed!" << std::endl;
        //mux.unlock();
        //return false;
    }

    if(!decode->Open(para))
    {
        re = false;
        std::cout << "Decode open filed!" << std::endl;
    }

    mux.unlock();
    if(re)
        std::cout << "AudioThread::Open successed!" << std::endl;

    return re;
}

void AudioThread::Push(AVPacket* pkt)
{
    if(!pkt)
        return;
    //阻塞
    while(!isExit)
    {
        mux.lock();
        if(packs.size() < maxList)
        {
            packs.push_back(pkt);
            mux.unlock();
            break;
        }
        mux.unlock();
        msleep(1);
    }

}
