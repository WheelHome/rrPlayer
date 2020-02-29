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

## FFmpeg SDK软硬件解码基础
### 结构体解析
#### AVFormatContext
- AVIOContext* pb ->文件io上下文
- char filename[1024] ->文件名
- unsigned int nb_streams ->流数量
- AVStream** streams ->流信息
- int64_t duration ->AV_TIME_BASE 微秒值
- int64_t bit_rate ->比特率
- void avformat_close_input(AVFormatContext** s) ->关闭输入上下文
#### AVStream
- AVCodecContext* codec; ->已经过时
- AVRational time_base; ->分数 时间的系数
- int64_t duration; ->视频总长度
- AVRational avg_frame_rate; ->帧率
- AVCodecParameters* codecpar; ->音视频参数 用来替代codec
#### AVCodecParameters
- enum AVMediaType codec_type; ->编码类型
- enum AVCodecID codec_id; ->编码格式
- uint32_t codec_tag; ->编码器
- int format; ->视频像素格式、音频采样格式
- int width;int height; ->视频宽高
- uint64_t channel_layout; ->音频声道
- int channels; ->声道数
- int sample_rate; ->音频样本率
- int frame_size; ->一帧音频大小
### 解封装
1. 打开接口函数
   - av_register_all() ->注册所有的封装格式(已经弃用，不需要再做相关操作)
   - avformat_network_init() ->支持rtsp和http
2. 分析索引 打开封装
    - avformat_open_input(...)
3. 找到流信息
    - avformat_find_stream_info(...) 
4. 寻找音频流或者视频流
   - av_find_best_stream(...)
5. 读取帧信息
    - av_read_frame(...)
#### 解封装接口函数解析
##### int avformat_open_input( AVFormatContext** ps,const char* url,AVInputFormat* fmt,AVDictionary** options)
- 确保av_register_all 或者 avformat_network_init已调用
- AVFormatContext** ps ->格式化上下文，一般传一个指针的地址，函数内部会在该指针指向的地址处生成所需要的空间。也可以传外部构建好的对象，但是会带来内存相关的问题。原则上库的内存和外部调用空间的内存应该相互独立，互不干扰。
- const char* url ->网络连接地址，支持http和rtsp，也支持本地文件。
- AVInputFormat* fmt ->指定输入的封装格式。
- AVDictionary** options ->字典数组，传递参数选择。
### 软硬件解码
### 像素格式转换
### 重采样
### pts/dts
### 同步策略
### sync