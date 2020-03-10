#include <QCoreApplication>
#include "sdlaudio.h"
#include <stdio.h>
#include <chrono>
int pcm_buffer_size = 4096;


int main(int argc, char *argv[]) {

    auto sdl = SDLAudio::GetInstance();
    sdl->Open();

    FILE *fp = fopen("out.pcm", "rb+");
    if (fp == NULL) {
        printf("cannot open this file\n");
        return -1;
    }
    char *pcm_buffer = (char *) malloc(pcm_buffer_size);
    sdl->Play();

    while (1) {
        if (fread(pcm_buffer, 1, pcm_buffer_size, fp) != pcm_buffer_size) { //从文件中读取数据，剩下的就交给音频设备去完成了，它播放完一段数据后会执行回调函数，获取等多的数据
            break;
        }
        sdl->Write((Uint8*)pcm_buffer,pcm_buffer_size);
    }
    free(pcm_buffer);

    QCoreApplication a(argc, argv);

    return a.exec();
}
