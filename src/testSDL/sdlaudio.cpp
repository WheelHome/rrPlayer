#include "sdlaudio.h"

#include <SDL2/SDL.h>
#include <functional>
#include <mutex>
class SDLAudioPlay : public SDLAudio
{
public:
    SDLAudioPlay();
    ~SDLAudioPlay();
    //打开音频播放
    virtual bool Open();
    virtual void Close();
    virtual void Clear();

    virtual int GetFree();
    //返回缓冲中还没有播放的时间(ms)
    virtual long long getNoPlayPts();

    virtual void SetPos(bool isPause);
    virtual void Write(Uint8* buffer,Uint32 len);
    void Play()
    {
        //播放
        SDL_PauseAudio(0);
    }
private:
    std::mutex mux;
};

static Uint8 *audio_chunk;
static Uint32 audio_len;
static Uint8 *audio_pos;

void read_audio_data(void *udata, Uint8 *stream, int len)
{
    SDL_memset(stream, 0, len);
    if (audio_len == 0)
        return;
    len = (len > audio_len ? audio_len : len);

    SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
    audio_pos += len;
    audio_len -= len;
}

void SDLAudioPlay::Write(Uint8* buffer,Uint32 len)
{
    mux.lock();
    audio_chunk = buffer;
    audio_len = len;
    audio_pos = audio_chunk;
    while(audio_len > 0)
    {
        SDL_Delay(1);
    }
    mux.unlock();
}

//打开音频播放
bool SDLAudioPlay::Open()
{
    mux.lock();
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        mux.unlock();
        printf("Could not initialize SDL - %s\n", SDL_GetError());
        return false;
    }
    SDL_AudioSpec spec;
    spec.freq = sampleRate;//根据你录制的PCM采样率决定
    spec.format = AUDIO_S16;
    spec.channels = 2; //单声道
    spec.silence = 0;
    spec.samples = 1024;
    spec.callback = read_audio_data;
    spec.userdata = NULL;

    if (SDL_OpenAudio(&spec, NULL) < 0) {
        mux.unlock();
        printf("%s\n",SDL_GetError());
        printf("can't open audio.\n");
        return false;
    }

    mux.unlock();
    return true;
}

void SDLAudioPlay::Close()
{
    SDL_Quit();
}

void SDLAudioPlay::Clear()
{

}

int SDLAudioPlay::GetFree()
{

}
//返回缓冲中还没有播放的时间(ms)
long long SDLAudioPlay::getNoPlayPts()
{

}

void SDLAudioPlay::SetPos(bool isPause)
{

}

SDLAudioPlay::SDLAudioPlay()
{

}

SDLAudioPlay::~SDLAudioPlay()
{

}

SDLAudio::SDLAudio()
{

}

SDLAudio::~SDLAudio()
{

}

SDLAudio* SDLAudio::GetInstance()
{
    static SDLAudioPlay instance;
    return &instance;
}
