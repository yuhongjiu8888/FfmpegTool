#include"FfmpegTool.hpp"

#include<iostream>

FfmpegTool::FfmpegTool(){

}

FfmpegTool::FfmpegTool(int width,int height,int fps,char *url):m_width(width),m_height(height),m_fps(fps),m_url(url){
    Pixel_Format_Context = nullptr;
    Out_Data = nullptr;
    Encoder_Context = nullptr;
    Format_Wrapper = nullptr;
    codec = nullptr;
    out_stream = nullptr;
}

FfmpegTool::~FfmpegTool(){

}

void FfmpegTool::Init(){
    avcodec_register_all();     //注册所有的编解码器

    av_register_all();          //注册所有的封装器

    avformat_network_init();    //注册所有网络协议
}

bool FfmpegTool::init_RGB_to_YUV(){
    Pixel_Format_Context = sws_getCachedContext(Pixel_Format_Context,
        m_width, m_height, AV_PIX_FMT_BGR24,     //源宽、高、像素格式
        m_width, m_height, AV_PIX_FMT_YUV420P,//目标宽、高、像素格式
        SWS_BICUBIC,  // 尺寸变化使用算法
        0, 0, 0
    );
    if (!Pixel_Format_Context)
    {
        std::cout<<"init_RGB_to_YUV failed!"<<std::endl;
        return false;
    }
    return true;
}

bool FfmpegTool::InitOutPutData(){
    Out_Data = av_frame_alloc();
    Out_Data->format = AV_PIX_FMT_YUV420P;
    Out_Data->height = m_height;
    Out_Data->width = m_width;
    Out_Data->pts = 0;

    int iRet = av_frame_get_buffer(Out_Data,32);
    if(iRet !=0 ){
        char buf[1024] = { 0 };
        av_strerror(iRet, buf, sizeof(buf) - 1);
        std::cout<<buf<<std::endl;
        return false;
    }

    return true;
}

bool FfmpegTool::InitEncodeContext(){
    codec = avcodec_find_encoder(AV_CODEC_ID_H264); //找到编码协议是H264

    if (!codec)
    {
        std::cerr<<"Can`t find h264 encoder!"<<std::endl;
        return false;
    }

    Encoder_Context = avcodec_alloc_context3(codec); //创建编译器上下文

    if (!Encoder_Context)
    {
        std::cerr<<"avcodec_alloc_context3 failed!"<<std::endl;
        return false;
    }

    //设置编码器参数
    Encoder_Context->codec_id = codec->id;
    Encoder_Context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER; //全局参数
    Encoder_Context->thread_count = 8;

    Encoder_Context->bit_rate = 50 * 1024 * 8;//压缩后每秒视频的bit位大小 50kB
    Encoder_Context->width = m_width;
    Encoder_Context->height = m_height;

    Encoder_Context->time_base.num = 1;
    Encoder_Context->time_base.den = m_fps;
    Encoder_Context->framerate.den = m_fps;
    Encoder_Context->framerate.num = 1;

    Encoder_Context->qmin = 10;   //调节清晰度和编码速度 //这个值调节编码后输出数据量越大输出数据量越小，越大编码速度越快，清晰度越差
    Encoder_Context->qmax = 51;

    Encoder_Context->gop_size = 50;   //编码一旦有gopsize很大的时候或者用了opencodec，有些播放器会等待I帧，无形中增加延迟。
    Encoder_Context->max_b_frames = 0;    //编码时如果有B帧会再解码时缓存很多帧数据才能解B帧，因此只留下I帧和P帧。
    Encoder_Context->pix_fmt = AV_PIX_FMT_YUV420P;


    AVDictionary *param = 0;
    av_dict_set(&param, "preset", "superfast", 0);  //编码形式修改
    av_dict_set(&param, "tune", "zerolatency", 0);  //实时编码

    int ret = avcodec_open2(Encoder_Context, codec, &param); //打开编码器上下文
    if (ret != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        std::cout << buf << std::endl;
        return false;
    }
    std::cout << "avcodec_open2 success!" << std::endl;

    return true;
}

bool FfmpegTool::CreatFormatContext(){
    int ret = avformat_alloc_output_context2(&Format_Wrapper, 0, "flv", m_url); //rtmp 使用flv
    if (ret != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        std::cerr<<"avformat_alloc_output_context2 error,"<<buf<<std::endl;
        return false;
    }

    //添加视频流
    out_stream = avformat_new_stream(Format_Wrapper, NULL);
    if (!out_stream)
    {
        std::cerr<<"avformat_new_stream failed"<<std::endl;
        return false;
    }
    out_stream->codec->codec_tag = 0;

    //从编码器复制参数
    avcodec_copy_context(out_stream->codec, Encoder_Context);

    out_stream->time_base.num = 1;
    out_stream->time_base.den = m_fps;

    av_dump_format(Format_Wrapper, 0, m_url, 1);

    //打开rtmp 的网络输出IO
    ret = avio_open(&Format_Wrapper->pb, m_url, AVIO_FLAG_WRITE);
    if (ret != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        std::cerr<<"avio_open failed,"<<buf<<std::endl;
        return false;
    }

    // AVDictionary* options = NULL;
    // av_dict_set(&options, "rtsp_transport", "tcp", 0);
    // av_dict_set(&options, "stimeout", "8000000", 0);  //设置超时时间

    //写入封装头
    ret = avformat_write_header(Format_Wrapper,NULL);
    if (ret != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        std::cerr<<"avformat_write_header,"<<buf<<std::endl;
        return false;
    }

    return true;
}

bool FfmpegTool::Rtsp_Pusher(cv::Mat &frame,int frameCount){
    AVPacket pkt;
    av_init_packet(&pkt);

    //输入的数据结构
    uint8_t *indata[AV_NUM_DATA_POINTERS] = { 0 };

    indata[0] = frame.data;
    int insize[AV_NUM_DATA_POINTERS] = { 0 };
    //一行（宽）数据的字节数
    insize[0] = frame.cols * frame.elemSize();
    //insize[0] = frame.step[0];
    int h = sws_scale(Pixel_Format_Context, indata, insize, 0, frame.rows, //源数据
        Out_Data->data, Out_Data->linesize);
    if (h <= 0)
    {
        std::cerr<<"sws_scale error!"<<std::endl;
        return false;
    }

    Out_Data->pts = frameCount;
    //vpts++;
    int got_packet = 0;

    pkt.data = NULL;    // packet data will be allocated by the encoder
    pkt.size = 0;
    pkt.pts = AV_NOPTS_VALUE;
    pkt.dts =AV_NOPTS_VALUE;

    int ret = avcodec_encode_video2(Encoder_Context,&pkt,Out_Data,&got_packet);

    if(got_packet == 0 || ret != 0)
    {
        std::cerr << "avcodec_encode_video2 fail -------------"<<std::endl;
        return false;
    }

    if(out_stream->codec->codec_type == AVMEDIA_TYPE_VIDEO)  //视频
    {

        pkt.pts = av_rescale_q_rnd(pkt.pts, Encoder_Context->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, Encoder_Context->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, Encoder_Context->time_base, out_stream->time_base);
        pkt.pos = -1;
        std::cout<<"set pkt finishend!"<<std::endl;
    }


    int iret = av_interleaved_write_frame(Format_Wrapper, &pkt);
    if (iret == 0)
    {
        std::cout<<"push frame success!"<<std::endl;
        return true;
    }else{
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        std::cout<<"push frame failed!,"<<buf<<std::endl;
    }
    
    return false;
}