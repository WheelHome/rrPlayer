#ifndef VIDEOCALL_H
#define VIDEOCALL_H

struct AVFrame;
class VideoCall
{
public:
    virtual void Init(int width,int height) = 0;
    virtual void Repaint(AVFrame* frame) = 0;
};

#endif // VIDEOCALL_H
