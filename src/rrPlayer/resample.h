#ifndef RESAMPLE_H
#define RESAMPLE_H
#include <mutex>
struct AVCodecParameters;
struct SwrContext;
struct AVFrame;
class Resample
{
public:
    int outFormat = 1;//AV_SAMPLE_FMT_S16
    Resample();
    ~Resample();
    //输出参数和输入参数一致,除了采样格式,输出为S16,会释放para
    virtual bool Open(AVCodecParameters* para,bool isClearPara = false);
    virtual void Close();
    //返回重采样大小,不管成功与否都释放indata空间
    virtual int AudioResample(AVFrame* indata,unsigned char* data);
protected:
    std::mutex mux;
    SwrContext *actx;
};

#endif // RESAMPLE_H
