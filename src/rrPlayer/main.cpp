#include "widget.h"
#include "demux.h"
#include "decode.h"
#include <QApplication>
#include <iostream>
int main(int argc, char *argv[])
{
    Demux m;
    const char* url = "rtmp://202.69.69.180:443/webcast/bshdlive-pc";
    const char* src = "test.mp4";
    std::cout << m.Open(src);
    m.Read();
    m.Clear();
    m.Close();
    std::cout << m.Open(src);
    std::cout << "CopyAPara=" << m.CopyAPara();
    std::cout << "CopyVPara=" <<m.CopyVPara();
    std::cout << "seek=" <<m.Seek(0.999);
    Decode vdecode;
    std::cout << vdecode.Open(m.CopyVPara()) << std::endl;
    //vdecode.Clear();
    //vdecode.Close();
    Decode adecode;
    std::cout << adecode.Open(m.CopyAPara()) << std::endl;
    for(;;)
    {

        AVPacket* pkt = m.Read();
        if(m.IsAudio(pkt))
        {
            adecode.Send(pkt);
            AVFrame* frame = adecode.Recv();
        }else
        {
            vdecode.Send(pkt);
            AVFrame* frame = vdecode.Recv();
        }
        if(!pkt)
        {
            break;
        }
    }
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
