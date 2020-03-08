#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMainWindow>
#include "videowidget.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    //定时器 滑动条显示
    void timerEvent(QTimerEvent* event)override;

    //窗口尺寸变化
    void resizeEvent(QResizeEvent *event) override;

    //双击全屏
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void SetPause(bool isPause);
private:
    Ui::Widget *ui;
    bool isSliderPress = false;
public slots:
    void OpenFile();
    void PlayOrPause();
    void SliderPress();
    void SliderRelease();
//private:
};
#endif // WIDGET_H
