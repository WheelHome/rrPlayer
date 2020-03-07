#include "widget.h"
#include "ui_widget.h"
#include "demuxthread.h"
#include <QFileDialog>
#include <QMessageBox>
static DemuxThread dt;
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    dt.Start();
    startTimer(40);
}

Widget::~Widget()
{
    delete ui;
    dt.Close();
}


void Widget::timerEvent(QTimerEvent* event)
{
    long long total = dt.totalMs;
    if(dt.totalMs > 0)
    {
        double pos = dt.pts / (double)total;
        int v = ui->playPos->maximum() * pos;
        ui->playPos->setValue(v);
    }
}

void Widget::resizeEvent(QResizeEvent *event)
{
    ui->playPos->move(100,this->height()-100);
    ui->playPos->resize(this->width()-200,ui->playPos->height());
    ui->openFileButton->move(this->width()/2-50,this->height()-150);
    ui->video->resize(this->size());
}

void Widget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(isFullScreen())
    {
        this->showNormal();
    }else
    {
        this->showFullScreen();
    }
}
void Widget::OpenFile()
{
    //选择文件
    QString name = QFileDialog::getOpenFileName(this,"选择视频文件");
    if(name.isEmpty())
    {
        return;
    }
    this->setWindowTitle(name);
    if(!dt.Open(name.toLocal8Bit(),ui->video))
    {
        QMessageBox::information(0,"error","open file failed!");
        return;
    }
    //打开文件
    qDebug() << name;
}
