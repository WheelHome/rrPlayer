#include "videowidget.h"
#include <QDebug>
#include <QTimer>
//自动加双引号
#define GET_STR(x) #x
#define A_VER 3
#define T_VER 4

FILE* fp = nullptr;

//顶点shader
const char* vString = GET_STR(
            attribute vec4 vertexIn;
            attribute vec2 textureIn;
            varying vec2 textureOut;
            void main(void)
            {
                gl_Position = vertexIn;
                textureOut = textureIn;
            }
            );

//片元shader
const char* tString = GET_STR(
            varying vec2 textureOut;
            uniform sampler2D tex_y;
            uniform sampler2D tex_u;
            uniform sampler2D tex_v;
            void main(void)
            {
                vec3 yuv;
                vec3 rgb;
                yuv.x = texture2D(tex_y,textureOut).r;
                yuv.y = texture2D(tex_u,textureOut).r - 0.5;
                yuv.z = texture2D(tex_v,textureOut).r - 0.5;
                rgb = mat3(1.0,1.0,1.0,
                           0.0,-0.39465,2.03211,
                           1.13983,-0.58060,0.0
                            ) * yuv;
                gl_FragColor = vec4(rgb,1.0);
            }
            );
VideoWidget::VideoWidget(QWidget* parent)
    :QOpenGLWidget(parent)
{

}

VideoWidget::~VideoWidget()
{

}

//刷新显示
void VideoWidget::paintGL()
{
    if(feof(fp))
    {
        fseek(fp,0,SEEK_SET);
    }
    fread(data[0],1,width*height,fp);
    fread(data[1],1,width*height/4,fp);
    fread(data[2],1,width*height/4,fp);

    //0层绑定到材质Y
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texs[0]);
    //修改材质内容(复制内存内容)
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,width,height,GL_RED,GL_UNSIGNED_BYTE,data[0]);
    //与shader uni变量关联
    glUniform1i(unis[0],0);

    //1层绑定到材质U
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D,texs[1]);
    //修改材质内容(复制内存内容)
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,width/2,height/2,GL_RED,GL_UNSIGNED_BYTE,data[1]);
    //与shader uni变量关联
    glUniform1i(unis[1],1);

    //2层绑定到材质V
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D,texs[2]);
    //修改材质内容(复制内存内容)
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,width/2,height/2,GL_RED,GL_UNSIGNED_BYTE,data[2]);
    //与shader uni变量关联
    glUniform1i(unis[2],2);

    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
    qDebug() << "paintGL";
}

//初始化opengl
void VideoWidget::initializeGL()
{
    //初始化opengl(QOpenGLFunctions继承)函数
    initializeOpenGLFunctions();

    //program 加载shader(顶点和片元)脚本
    //片元(像素)shader
    qDebug() << program.addShaderFromSourceCode(QGLShader::Fragment,tString);
    //顶点shader
    qDebug() << program.addShaderFromSourceCode(QGLShader::Vertex,vString);

    //设置顶点坐标的变量
    program.bindAttributeLocation("vertexIn",A_VER);

    //设置材质坐标
    program.bindAttributeLocation("textureIn",T_VER);

    //编译shader
    qDebug() << "program.link()=" << program.link();

    qDebug() << "program.bind()=" << program.bind();

    //传递顶点和材质坐标
    //顶点
    static const GLfloat ver[] = {
        -1.0f,-1.0f,
        1.0f,-1.0f,
        -1.0f,1.0f,
        1.0f,1.0f
    };

    //材质
    static const GLfloat tex[] = {
        0.0f,1.0f,
        1.0f,1.0f,
        0.0f,0.0f,
        1.0f,0.0f
    };

    //顶点坐标
    glVertexAttribPointer(A_VER,2,GL_FLOAT,0,0,ver);
    glEnableVertexAttribArray(A_VER);

    //材质坐标
    glVertexAttribPointer(T_VER,2,GL_FLOAT,0,0,tex);
    glEnableVertexAttribArray(T_VER);

    //从shader获取材质
    unis[0] = program.uniformLocation("tex_y");
    unis[1] = program.uniformLocation("tex_u");
    unis[2] = program.uniformLocation("tex_v");

    //创建材质
    glGenTextures(3,texs);

    //Y
    glBindTexture(GL_TEXTURE_2D,texs[0]);
    //放大过滤，线性插值
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    //创建材质显存空间
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,width,height,0,GL_RED,GL_UNSIGNED_BYTE,0);

    //U
    glBindTexture(GL_TEXTURE_2D,texs[1]);
    //放大过滤，线性插值
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    //创建材质显存空间
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,width/2,height/2,0,GL_RED,GL_UNSIGNED_BYTE,0);

    //V
    glBindTexture(GL_TEXTURE_2D,texs[2]);
    //放大过滤，线性插值
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    //创建材质显存空间
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,width/2,height/2,0,GL_RED,GL_UNSIGNED_BYTE,0);

    //分配材质内存空间
    data[0] = new unsigned char [width * height];       //Y
    data[1] = new unsigned char [width * height / 4];   //U
    data[2] = new unsigned char [width * height / 4];   //V

    fp = fopen("out240x128.yuv","rb");
    if(!fp)
    {
        qDebug() << "out240x128.yuv file open error!";
    }

    //启动定时器
    QTimer* ti = new QTimer(this);
    connect(ti,SIGNAL(timeout()),this,SLOT(update()));
    ti->start(40);

}

//窗口尺寸变化
void VideoWidget::resizeGL(int w, int h)
{

}
