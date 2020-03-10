#include "slider.h"

Slider::Slider(QWidget *parent) : QSlider(parent)
{

}

Slider::~Slider()
{

}

void Slider::mousePressEvent(QMouseEvent *event)
{
    double pos = (double)event->x() / (double)width();
    setValue(pos * this->maximum());
    //原有事件处理
    QSlider::sliderReleased();
    //QSlider::mousePressEvent(event);
}
