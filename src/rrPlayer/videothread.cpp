#include "videothread.h"
#include "decode.h"
#include <iostream>
VideoThread::VideoThread()
{

}

VideoThread::~VideoThread()
{
}

void VideoThread::run()
{
    while(!isExit)
    {
        vmux.lock();
        //音视频同步
        if(synpts > 0 && synpts < decode->pts)
        {
            vmux.unlock();
            msleep(1);
            continue;
        }

        AVPacket* pkt = Pop();
        bool re = decode->Send(pkt);
        if(!re)
        {
            vmux.unlock();
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
        vmux.unlock();
    }
}

//打开，不管成功与否都清理
bool VideoThread::Open(AVCodecParameters* para,VideoCall* call,int width,int height)
{
    if(!para || !call)
        return false;
    Clear();
    vmux.lock();
    this->call = call;
    synpts = 0;
    if(call)
    {
        //初始化显示窗口
        call->Init(width,height);
    }
    vmux.unlock();

    bool re = true;
    if(!decode->Open(para))
    {
        re = false;
        std::cout << "Decode open filed!" << std::endl;
    }
    return re;
}

