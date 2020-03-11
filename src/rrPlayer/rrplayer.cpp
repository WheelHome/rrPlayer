#include "rrplayer.h"
#include "ui_rrplayer.h"
#include "demuxthread.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
static DemuxThread dt;

rrPlayer::rrPlayer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::rrPlayer)
{
    ui->setupUi(this);

    proSlider = ui->groupBox->findChild<Slider*>("progressSlider");
    volSlider = ui->groupBox->findChild<Slider*>("volumeSlider");
    isPlay = ui->groupBox->findChild<QPushButton*>("isPlay");

    dt.Start();
    startTimer(40);
}

rrPlayer::~rrPlayer()
{
    delete ui;
}
void rrPlayer::timerEvent(QTimerEvent* event)
{

    if(isSliderPress)
    {
        return;
    }
    long long total = dt.totalMs;
    if(dt.totalMs > 0)
    {
        double pos = dt.pts / (double)total;
        int v = proSlider->maximum() * pos;
        proSlider->setValue(v);
    }
}

void rrPlayer::resizeEvent(QResizeEvent *event)
{
    int pheight = this->height() - ui->groupBox->height() - ui->statusbar->height() - ui->menubar->height();
    ui->video->resize(this->width(),pheight);
    ui->groupBox->resize(this->width(),ui->groupBox->height());
    ui->groupBox->move(0,pheight);
    isPlay->move(0,0);
    proSlider->move(isPlay->width(),20);
    proSlider->resize(this->width()/1.4,proSlider->height());
    volSlider->move(this->width()-120,20);
}

void rrPlayer::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(isFullScreen())
    {
        this->showNormal();
    }else
    {
        this->showFullScreen();
    }
}

void rrPlayer::SetPause(bool isPause)
{
    if(isPause)
    {
        ui->isPlay->setText("播放");
    }else
    {
        ui->isPlay->setText("暂停");
    }
}

void rrPlayer::OpenFile()
{
    //dt.Clear();
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
    SetPause(dt.isPause);
}

void rrPlayer::PlayOrPause()
{
    bool isPause = !dt.isPause;
    SetPause(isPause);
    dt.SetPause(isPause);
}

void rrPlayer::SliderPress()
{
    isSliderPress = true;
}

void rrPlayer::SliderRelease()
{
    isSliderPress = false;
    double pos = 0.0;
    pos = (double)proSlider->value() / (double)proSlider->maximum();
    dt.Seek(pos);
}

void rrPlayer::volumeSliderRelease()
{
    dt.setVolume(volSlider->value());
}

void rrPlayer::volumeSliderPress()
{
}
