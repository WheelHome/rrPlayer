# rrPlayer
##  Base on Qt with FFmpeg
- FFmpeg国内镜像源下载:https://gitee.com/rzkn/ffmpeg?_from=gitee_search
### 音视频基础知识
#### MPEG-4
- MPEG-4是一套用于音频、视频信息的压缩编码标注
- MPEG-4 Part 14 MPEG-4文件格式
- MPEG-4 Part 15 AVC文件格式
- MPEG-4 Part 15 AVC Part10 H264编码
#### 编码格式
- 视频H264(AVC Part10)
- WMV
- XviD(Part2)
- mjpeg
- 视频
  - AVI 压缩标准可任意选择
  - FLV ts 流媒体格式
  - ASF
  - MP4
- 音频
    - aac
    - MP3
    - ape
    - flac
#### 封装
封装格式(格式头)|视频编码帧|音频编码帧
:-:|:--:|---:|---:|---:
mp4(Part14)flv mov avi<br>box音视频信息(编码和格式关键帧索引)| h264 mepg-4-10<br>NAL VCL<br>SPS PPS<br>I B P  | AAC<br>APE、FLAC无损压缩<br>PCM原始音频
*解码*|解码为YUV|解码为PCM FLT
*重采样*|转为RGB显示|转为声卡支持的S16播放
#### 解码
#### 重采样
#### 像素格式
- BGRA RGBA ARGB32 RGB32 YUV420
- R = Y + 1.4075 * (V - 128)
- G = Y - 0.3455 * (U - 128) - 0.7169 * (V - 128)
- B = Y + 1.779 * (U - 128)
- YUV
    - "Y"表示明亮度，也就是灰度值
    - "U"和"V"表示的则是色度
### PCM音频参数
- 采样率smaple_rate 44100(CD)
- 通道channels(左右声道)
- 样本大小(格式) sample_size
    - AV_SAMPLE_FMT_S16
    - AV_SAMPLE_FMT_FLTP
- 样本类型planar
    - AV_SAMPLE_FMT_S16在内存中的格式为:c1,c2,c1,c2...
    - AV_SAMPLE_FMT_FLTP在内存中的格式为:c1,c1,c1,...,c2,c2,c2...
### H.264/AVC视频编码标准
- 视频编码层面(VCL)
    - 视频数据的内容
- 网络抽象层面(NAL)
    - 格式化数据并提供头信息