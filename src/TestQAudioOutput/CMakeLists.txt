cmake_minimum_required(VERSION 3.5)

project(TestQAudioOutput LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Core)
find_package(Qt5Multimedia) #增加多媒体库

add_executable(TestQAudioOutput main.cpp
)

target_link_libraries(
    TestQAudioOutput
    Qt5::Core
    Qt5::Multimedia
    avcodec
    avformat
    avutil
    avdevice
    swresample
    swscale
    pthread)

