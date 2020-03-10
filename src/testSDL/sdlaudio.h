#ifndef SDLAUDIO_H
#define SDLAUDIO_H

#include <SDL2/SDL_types.h>
class SDLAudio
{
public:
    SDLAudio();
    virtual ~SDLAudio();
    int sampleRate = 44100;
    int sampleSize = 8;
    int channels = 2;

    static SDLAudio* GetInstance();
    //打开音频播放
    virtual bool Open() = 0;
    virtual void Close() = 0;
    virtual void Clear() = 0;
    virtual int GetFree() = 0;
    //返回缓冲中还没有播放的时间(ms)
    virtual long long getNoPlayPts() = 0;

    virtual void SetPos(bool isPause) = 0;

    virtual void Write(Uint8* buffer,Uint32 len) = 0;
    virtual void Play() = 0;

};

#endif // SDLAUDIO_H
