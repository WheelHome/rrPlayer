cmake_minimum_required(VERSION 3.5)

project(TestDemux LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#LINK_DIRECTORIES(/opt/ffmpeg/lib/) #FFmpeg安装目录

add_executable(TestDemux main.cpp)

TARGET_LINK_LIBRARIES(
    TestDemux
    avcodec
    avformat
    avutil
    avdevice
    pthread)
