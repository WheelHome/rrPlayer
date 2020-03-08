#ifndef SLIDER_H
#define SLIDER_H

#include <QSlider>
#include <QMouseEvent>
class Slider : public QSlider
{
    Q_OBJECT
public:
    explicit Slider(QWidget *parent = nullptr);
    virtual ~Slider();
    void mousePressEvent(QMouseEvent *event) override;


signals:

};

#endif // SLIDER_H
