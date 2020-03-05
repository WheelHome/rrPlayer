#include "widget.h"
#include "demux.h"
#include "decode.h"
#include "videowidget.h"
#include "resample.h"
#include "audioplay.h"
#include <QApplication>
#include <iostream>
#include <QThread>
class TestThread: public QThread
{
public:
    Demux m;
    Decode vdecode;
    Decode adecode;
    VideoWidget* video;
    Resample resample;
    void Init()
    {
        //解封装 读取头信息测试
        const char* url = "rtmp://202.69.69.180:443/webcast/bshdlive-pc";
        const char* src = "test.mp4";
        std::cout << m.Open(src);
        m.Read();
        m.Clear();
        m.Close();
        std::cout << m.Open(src);
        std::cout << "CopyAPara=" << m.CopyAPara();
        std::cout << "CopyVPara=" <<m.CopyVPara();
        //std::cout << "seek=" <<m.Seek(0.9);

        //解码测试
        std::cout << "vdecode.Open=" << vdecode.Open(m.CopyVPara()) << std::endl;
        //vdecode.Clear();
        //vdecode.Close();
        std::cout << "adecode.Open=" << adecode.Open(m.CopyAPara()) << std::endl;
        std::cout << "resample.Open=" << resample.Open(m.CopyAPara()) << std::endl;

        AudioPlay::GetInstance()->channels = m.channels;
        AudioPlay::GetInstance()->sampleRate = m.sampleRate;
        std::cout << "AudioPlay::GetInstance()->Open=" << AudioPlay::GetInstance()->Open() << std::endl;


    }

    unsigned char* pcm = new unsigned char[1024*1024];
    void run()
    {
        for(;;)
        {
            AVPacket* pkt = m.Read();
            if(m.IsAudio(pkt))
            {
                adecode.Send(pkt);
                AVFrame* frame = adecode.Recv();
                int len = resample.AudioResample(frame,pcm);
                std::cout << "resample.AudioResample=" << len <<" ";


                while(len > 0)
                {
                    if(AudioPlay::GetInstance()->GetFree() >= len)
                    {
                        AudioPlay::GetInstance()->Write(pcm,len);
                        break;
                    }
                }
            }else
            {
                vdecode.Send(pkt);
                AVFrame* frame = vdecode.Recv();
                //msleep(40);
                video->Repaint(frame);
            }
            if(!pkt)
            {
                break;
            }
        }
    }
};
int main(int argc, char *argv[])
{

    //初始化显示
    TestThread tt;
    tt.Init();

    QApplication a(argc, argv);
    Widget w;
    w.show();

    //初始化gl窗口
    w.Init(tt.m.width,tt.m.height);
    tt.video = w.getVideoWidget();
    tt.start();

    return a.exec();
}
