#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
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
private:
    Ui::Widget *ui;
public slots:
    void OpenFile();
//private:
};
#endif // WIDGET_H
