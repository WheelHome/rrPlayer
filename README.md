# rrPlayer
##  Base on Qt with FFmpeg and SDL2.0
- FFmpeg国内镜像源下载:https://gitee.com/rzkn/ffmpeg?_from=gitee_search
- SDL2.0下载:http://www.libsdl.org/download-2.0.php
### SDL2.0遇到错误总结
#### No such audio device
- 原因：库依赖没有安装完全
- 解决方案："sudo apt-get install libasound2-dev libpulse-dev"安装库 并且重新配置sdl库.

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
:-:|:--:|:--:|
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
#### AVPacket
- AVBufferRef* buf; ->引用计数
- int64_t pts; ->pts * (num / den)
- int64_t dts; ->
- uint8_t* data; ->具体数据
- int size; ->大小
- 接口函数
  - AVPacket* av_packet_alloc(void); ->创建并初始化一个AVPacket对象
  - AVPacket* av_packet_clone(const AVPAcket* src); ->创建拷贝对象，并增加引用计数
  - int av_packet_ref(AVPacket* dst,const AVPacket* src); ->给目标AVPacket增加引用计数
  - void av_packet_unref(AVPacket* pkt); ->减少引用计数
  - void av_packet_free(AVPacket** pkt); ->清空对象并减少引用计数
  - void av_init_packet(AVPacket* pkt); ->初始化手工创建的对象
  - int av_packet_from_data(AVPacket* pkt,uint8_t* data,int size); ->传入参数手工创建对象
#### AVCodecContext
- int thread_count; ->解码器线程数量
- AVRational time_base; ->时间基数
- 接口函数
    1. AVCodecContext* avcodec_alloc_context3(const AVCodec* codec); ->创建一个AVCodecContext对象上下文
    2. int avcodec_open2(AVCodecContext* avctx,const AVCodec* codec,AVDictionary** options); ->打开AVCodecContext
    - void avcodec_free_context(AVCodecContext** avctx); ->清理AVCodecContext上下文
#### AVFrame
- uint8_t* data[AV_NUM_DATA_POINTERS]; ->数据区
- int linesize[AV_NUM_DATA_POINTERS]; ->每行大小 对视频是一行数据的大小，对音频是一个通道的大小
- int width,height; ->视频宽高
- int nb_samples; ->音频单通道样本数量
- int64_t pts; ->显示时间
- int64_t pkt_dts; ->AVPacket的解码时间
- int sample_rate; ->音频样本率
- uint64_t channel_layout; ->音频通道类型
- int channels; ->音频通道数量
- int format; ->AVPixelFormat AVSampleFormat 像素格式
- 接口函数
  - AVFrame* frame = av_frame_alloc(); ->创建一个AVFrame上下文
  - void av_frame_free(AVFrame** frame); ->释放AVFrame上下文空间
  - int av_frame_ref(AVFrame* dst,const AVFrame* src); ->对目标上下文引用计数加一
  - AVFrame* av_frame_clone(const AVFrame* src); ->拷贝源上下文
  - void av_frame_unref(AVFrame* frame); ->引用计数减一
#### SwrContext 音频上下文
- 接口函数
    1. SwrContext* swr_alloc(); ->创建上下文
    2. SwrContext* swr_alloc_set_opts(struct SwrContext* s,<br>int64_t out_ch_layout,AVSampleFormat out_sample_fmt,int out_sample_rate,//输出格式<br>int64_t in_ch_layout,AVSampleFormat in_sample_fmt,int in_sample_rate,<br>int log_offset,void* log_ctx //日志输出); ->设置上下文相关参数对应上下文
   1. int swr_init(struct SwrContext* s); ->初始化上下文
   2. void swr_free(struct SwrContext** s); ->释放上下文
#### QAudioFormat
- setSampleRate
- setSampleSize ->比特数
- setChannelCount
- setCodec("audio/pcm")
- setByteOrder(QAudioFormat::LittleEndian) ->设置字节存储模式
- setSampleType(QAudioFormat::unSignedInt) ->设置样本类型
#### QAudioOutput 音频输出类
- QIODevice* start() ->启动输出
- suspend() ->挂起输出
- resume() ->恢复输出
- bufferSize() ->缓冲区大小
- bytesFree() ->已用缓冲区大小
- periodSize() ->内部硬件一次读取多少数据量
#### QIODevice io输出类
- qint64 write(const char* data,qint64 len); ->向目的设备写入数据以及数据长度，返回实际写入的数据长度
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
##### int avformat_open_input( AVFormatContext** ps,const char* url,AVInputFormat* fmt,AVDictionary** options) ->打开输入流
- 确保av_register_all 或者 avformat_network_init已调用
- AVFormatContext** ps ->格式化上下文，一般传一个指针的地址，函数内部会在该指针指向的地址处生成所需要的空间。也可以传外部构建好的对象，但是会带来内存相关的问题。原则上库的内存和外部调用空间的内存应该相互独立，互不干扰。
- const char* url ->网络连接地址，支持http和rtsp，也支持本地文件。
- AVInputFormat* fmt ->指定输入的封装格式。
- AVDictionary** options ->字典数组，传递参数选择。
#### int av_read_frame(AVFormatContext* s,AVPacket* pkt) ->读取帧
- AVFormatContext* s ->上下文
- AVPacket* pkt ->存放压缩的数据
#### int av_seek_frame(AVFormatContext* s,int stream_index,int64_t timestamp,int flags) ->寻找帧
- AVFormatContext* s ->上下文
- int stream_index ->流计数 default -1
- int64_t timestamp ->时间戳
- int flags ->标志
  - AVSEEK_FLAG_BACKWARD 1 // < 向后查找
  - AVSEEK_FLAG_BYTE 2 //基于字节位寻找
  - AVSEEK_FLAG_ANY 4 //寻找任意帧,可能不是关键帧
  - AVSEEK_FLAG_FRAME 8 //基于帧的排序数寻找
### 软硬件解码
1. 注册解码器 ->已经过时，并不需要再注册所有解码器
   - avcodec_register_all()
2. 根据id号或者名称找到解码器
    - AVCodec* avcodec_find_decoder(enum AVCodecID id);
    - AVCodec* avcodec_decoder_by_name(const char* name);
#### 软硬件解码函数解析
##### int avcodec_send_packet(AVCodecContext* avctx,const AVPacket* avpkt); ->传递压缩数据给解码器进行解码 将AVPacket放到解码队列中去
- AVCodecContext* avctx; ->解码器上下文
- const AVPacket* avpkt; ->数据包上下文
##### int avcodec_receive_frame(AVCodecContext* avctx,AVFrame* frame); ->从已解码队列中取出一个AVFrame 立即返回 可能会收到多个解码帧
- AVCodecContext* avctx; ->解码器上下文
- AVFrame* frame; ->解码之后的帧
### 像素格式转换
- sws_getContext; ->格式转换上下文 创建一个新的上下文
- struct SwsContext* sws_getCachedContext(struct SwsContext* context,int srcW,int srcH,enum AVPixelFormat srcFormat,int dstW,int dstH,enum AVPixelFormat dsfFormat,int flags,SwsFilter* srcFilter,SwsFilter* dstFilter,const double* param);
#### 像素格式转换常用函数解析
##### int sws_scale(struct SwsContext* c,const uint8_t* const srcSlice[],const int srcStride[],int srcSliceY,int srcSliceH,uint8_t* const dst[],const int dstStride[]);
##### void sws_freeContext(struct SwsContext* swsContext); ->释放格式转换上下文
### 重采样
####  int swr_convert(struct SwrContext* s,uint8_t** out,int out_count,const uint8_t** in ,int in_count); 音频重采样接口函数
- struct SwrContext* s; ->音频上下文
- uint8_t** out; ->输出数据
- int out_count; ->输出通道数
- const uint8_t** in; ->输入音频数据
- int in_count; ->输入音频通道数
### pts/dts
### 同步策略
### sync

### Qt opengl编程
#### QOpenGLWidget:与界面如何交互
##### void paintGL();绘制函数
##### void initializeGL();初始化函数
##### void resizeGL(int width,int height);重设窗口大小
##### QOpenGLFunctions:Qt编程中提供给用户使用的公有opengl类对象，继承可以使用opengl函数
#### Programe GLSL 顶点和片元:如何与显卡交互
##### QGLShaderProgram
- 编译运行shader
- addShaderFromSourceCode
- bindAttributeLocation 设置传入的变量
- uniformLocation 获取变量
##### 着色器语言GLSL
- 顶点着色器是针对每个顶点执行一次，用于确定顶点的位置；片元着色器是针对每个片元(像素)执行一次，用于确定每个片元(像素)的颜色
- GLSL的基本语法与C基本相同
- 完美支持向量和矩阵操作
- 提供了大量的内置函数来提供丰富的扩展功能
- 通过限定符操作来管理输入输出类型的
- 三种GLSL变量类型
  - varying 顶点与片元共享
  - attribute 顶点使用，由bindAttributeLocation传入
  - uniform 程序传入uniformLocation获取地址,由glUniform1i(textureUniformY,0);来进行设置
#### 材质Texture:如何写入ffmpeg
##### 1.创建材质
##### 2.写入和绘制材质