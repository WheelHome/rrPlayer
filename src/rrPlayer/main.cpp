#include "widget.h"
#include "demuxthread.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    DemuxThread dt;
    dt.Open("test.mp4",w.getVideoWidget());
    dt.Start();
    return a.exec();
}
