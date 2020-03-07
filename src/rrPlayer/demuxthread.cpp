#include "demuxthread.h"
#include "demux.h"
#include "videothread.h"
#include "audiothread.h"

#include <iostream>
DemuxThread::DemuxThread()
{
}

DemuxThread::~DemuxThread()
{
    //等待线程退出
    isExit = true;
    wait();
}

bool DemuxThread::Open(const char* url,VideoCall* call)
{
    if(!url || url[0] == '\0')
    {
        return false;
    }
    mux.lock();
    if(!demux)
    {
        demux = new Demux();
    }
    if(!vt)
    {
        vt = new VideoThread();
    }
    if(!at)
    {
        at = new AudioThread();
    }

    //打开解封装
    bool re = true;
    re = demux->Open(url);
    if(!re)
    {
        mux.unlock();
        std::cout << "demux->Open(url) failed!" << std::endl;
        return false;
    }

    //打开视频解码器和处理线程
    if(!vt->Open(demux->CopyVPara(),call,demux->width,demux->height))
    {
        re = false;
        std::cout << "vt->Open(demux->CopyVPara(),call,demux->width,demux->height) failed!" << std::endl;
    }

    //打开音频解码器和处理线程
    if(!at->Open(demux->CopyAPara(),demux->sampleRate,demux->channels))
    {
        re = false;
        std::cout << "at->Open(demux->CopyAPara(),demux->sampleRate,demux->channels) failed!" << std::endl;
    }

    totalMs = demux->totalMs;
    mux.unlock();
    if(re)
    {
        std::cout << "DemuxThread::Open successed!" << std::endl;
    }
    return re;
}

void DemuxThread::Start()
{
    mux.lock();
    if(!demux)
    {
        demux = new Demux();
    }
    if(!vt)
    {
        vt = new VideoThread();
    }
    if(!at)
    {
        at = new AudioThread();
    }
    //启动当前线程
    QThread::start();
    if(vt)
    {
        vt->start();
    }
    if(at)
    {
        at->start();
    }
    mux.unlock();
}

void DemuxThread::run()
{
    while (!isExit)
    {
        mux.lock();
        if(!demux)
        {
            mux.unlock();
            msleep(5);
            continue;
        }

        //音视频同步
        if(vt && at)
        {
            vt->synpts = at->pts;
            pts = at->pts;
        }

        AVPacket* pkt = demux->Read();
        if(!pkt)
        {
            mux.unlock();
            msleep(5);
            continue;
        }
        //判断数据是音频
        if(demux->IsAudio(pkt))
        {
            if(at)
            {
                at->Push(pkt);
            }
        }else
        {
            if(vt)
            {
                vt->Push(pkt);
            }
        }
        mux.unlock();
    }
}

void DemuxThread::Close()
{
    isExit = true;
    wait();
    if(vt)
    {
        vt->Close();
        mux.lock();
        delete vt;
        vt = nullptr;
        mux.unlock();
    }
    if(at)
    {
        at->Close();
        mux.lock();
        delete at;
        at = nullptr;
        mux.unlock();
    }
}
