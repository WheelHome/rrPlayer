#ifndef RRPLAYER_H
#define RRPLAYER_H

#include <QMainWindow>

namespace Ui {
class rrPlayer;
}

class Slider;
class QPushButton;
class rrPlayer : public QMainWindow
{
    Q_OBJECT

public:
    explicit rrPlayer(QWidget *parent = nullptr);
    ~rrPlayer();

    //定时器 滑动条显示
    void timerEvent(QTimerEvent* event)override;

    //窗口尺寸变化
    void resizeEvent(QResizeEvent *event) override;

    //双击全屏
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void SetPause(bool isPause);

public slots:
    void OpenFile();
    void PlayOrPause();
    void SliderPress();
    void SliderRelease();
    void volumeSliderRelease();
    void volumeSliderPress();
private:
    Ui::rrPlayer *ui;
    bool isSliderPress = false;
    Slider* proSlider;
    Slider* volSlider;
    QPushButton* isPlay;

};

#endif // RRPLAYER_H
