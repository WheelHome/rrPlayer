#include "decodethread.h"
#include "decode.h"
DecodeThread::DecodeThread()
{
    if(!decode)
    {
        //打开解码器
        decode = new Decode();
    }
}

DecodeThread::~DecodeThread()
{
    //等待线程退出
    isExit = true;
    wait();
}

void DecodeThread::Push(AVPacket* pkt)
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

AVPacket* DecodeThread::Pop()
{
    mux.lock();
    if(packs.empty())
    {
        mux.unlock();
        return nullptr;
    }
    AVPacket* pkt = packs.front();
    packs.pop_front();
    mux.unlock();
    return pkt;
}

void DecodeThread::Clear()
{
    mux.lock();
    decode->Clear();
    while(!packs.empty())
    {
        AVPacket* pkt = packs.front();
        FreePacket(&pkt);
        packs.pop_front();

    }
    mux.unlock();
}

void DecodeThread::Close()
{
    Clear();
    //等待线程退出
    if(decode)
    {
        decode->Close();
        mux.lock();
        delete decode;
        decode = nullptr;
        mux.unlock();
    }
}
