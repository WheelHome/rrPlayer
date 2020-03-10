#include "audioplay.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <mutex>
#include <QDebug>

extern "C"{
#include <SDL2/SDL.h>
}

class CAudioPlay : public AudioPlay
{
public:
    virtual bool Open(int format);
    virtual void Close();
    virtual void Clear();
    virtual bool Write(const unsigned char* data,int dataSize);
    virtual int GetFree();
    virtual long long getNoPlayPts();
    virtual void SetPos(bool isPause);
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

bool CAudioPlay::Open(int format)
{
    mux.lock();
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        mux.unlock();
        printf("Could not initialize SDL - %s\n", SDL_GetError());
        return false;
    }
    SDL_AudioSpec spec;
    spec.freq = sampleRate;//根据你录制的PCM采样率决定
    spec.format = AUDIO_S16SYS;
    spec.channels = channels;
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
    //播放
    SDL_PauseAudio(0);
    mux.unlock();
    return true;
}

void CAudioPlay::Clear()
{
    mux.lock();
    SDL_PauseAudio(1);
    mux.unlock();
}

void CAudioPlay::Close()
{
    mux.lock();
    SDL_CloseAudio();
    mux.unlock();
}

bool CAudioPlay::Write(const unsigned char* data,int dataSize)
{
    mux.lock();
    audio_chunk = (Uint8*)data;
    audio_len = dataSize;
    audio_pos = audio_chunk;
    mux.unlock();
}

int CAudioPlay::GetFree()
{
    mux.lock();
    int len = audio_len;
    mux.unlock();
    return len;
}

long long CAudioPlay::getNoPlayPts()
{
    mux.lock();
    long long pts = 0;
    //还未播放的字节数
    double size = audio_len;
    //一秒音频字节大小
    double secSize = sampleRate * (sampleSize / 8) * channels;
    if(secSize <= 0)
    {
        pts = 0;
    }else
    {
        pts = (size / secSize) * 1000;
    }
    mux.unlock();
    return pts;
}

void CAudioPlay::SetPos(bool isPause)
{
    mux.lock();
    if(isPause)
    {
        SDL_PauseAudio(1);
    }else
    {
        SDL_PauseAudio(0);
    }
    mux.unlock();
}

AudioPlay::AudioPlay()
{

}

AudioPlay::~AudioPlay()
{
    SDL_Quit();
}

AudioPlay* AudioPlay::GetInstance()
{
    static CAudioPlay play;
    return &play;
}
