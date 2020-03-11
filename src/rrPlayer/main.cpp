#include "rrplayer.h"
#include "demuxthread.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Widget w;
    //w.show();
    rrPlayer p;
    p.setWindowTitle("rrPlayer");
    p.show();

    //DemuxThread dt;
    //dt.Open("test.mp4",w.getVideoWidget());
    //dt.Open("test1.mp4",w.getVideoWidget());
    //dt.Open("rtmp://202.69.69.180:443/webcast/bshdlive-pc",w.getVideoWidget());
    //dt.Start();
    return a.exec();
}
