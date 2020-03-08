#ifndef AUDIOPLAY_H
#define AUDIOPLAY_H


class AudioPlay
{
public:
    int sampleRate = 44100;
    int sampleSize = 8;
    int channels = 2;

    AudioPlay();
    virtual ~AudioPlay();
    static AudioPlay* GetInstance();
    //打开音频播放
    virtual bool Open() = 0;
    virtual void Close() = 0;
    virtual void Clear() = 0;

    //播放音频
    virtual bool Write(const unsigned char* data,int dataSize) = 0;
    virtual int GetFree() = 0;
    //返回缓冲中还没有播放的时间(ms)
    virtual long long getNoPlayPts() = 0;

    virtual void SetPos(bool isPause) = 0;
};

#endif // AUDIOPLAY_H
