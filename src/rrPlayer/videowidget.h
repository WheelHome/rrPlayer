#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <mutex>
struct AVFrame;
class VideoWidget : public QOpenGLWidget,protected QOpenGLFunctions
{
    Q_OBJECT
public:
    VideoWidget(QWidget* parent);
    ~VideoWidget();
    void Init(int width,int height);
    //不管成功与否都释放frame空间
    virtual void Repaint(AVFrame* frame);
protected:
    void paintGL() override;
    void initializeGL() override;
    void resizeGL(int w, int h) override;
private:
    //shader程序
    QGLShaderProgram program;
    //shader中yuv变量地址
    GLuint unis[3] = {0};
    //opengl中的texture地址
    GLuint texs[3] = {0};

    std::mutex mux;
    //材质内存空间
    unsigned char* data[3] = {0};
    int width = 240;
    int height = 128;
};

#endif // VIDEOWIDGET_H
