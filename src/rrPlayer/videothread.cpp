#include "videothread.h"
#include "decode.h"
#include <iostream>
VideoThread::VideoThread()
{

}

VideoThread::~VideoThread()
{
    //等待线程退出
    isExit = true;
    wait();
}

void VideoThread::run()
{
    while(!isExit)
    {
        mux.lock();
        if(packs.empty() || !decode)
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
            //显示图像
            if(call)
            {
                call->Repaint(frame);
            }
        }

        mux.unlock();
    }

}

//打开，不管成功与否都清理
bool VideoThread::Open(AVCodecParameters* para,VideoCall* call,int width,int height)
{
    if(!para || !call)
        return false;
    mux.lock();
    this->call = call;
    if(call)
    {
        //初始化显示窗口
        call->Init(width,height);
    }
    if(!decode)
    {
        //打开解码器
        decode = new Decode();
    }
    bool re = true;
    if(!decode->Open(para))
    {
        re = false;
        std::cout << "Decode open filed!" << std::endl;
    }

    mux.unlock();

    return re;
}

void VideoThread::Push(AVPacket* pkt)
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
