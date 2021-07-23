#ifndef __FFMPEGTOOL__H__
#define __FFMPEGTOOL__H__

//按照C方式去编译
extern "C" {
#include "libswscale/swscale.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

#include<opencv2/opencv.hpp>

class FfmpegTool final
{
public:
    FfmpegTool(/* args */);
    FfmpegTool(int width,int height,int fps,char *url);
    ~FfmpegTool();

    void  Init();           //环境初始化

    bool init_RGB_to_YUV(); //图像格式转换

    bool InitOutPutData();  //初始化输出格式

    bool InitEncodeContext(); //初始化编码上下文

    bool CreatFormatContext();//创建封装器上下文

    bool Rtsp_Pusher(cv::Mat &frame,int frameCount);//推流

private:
    SwsContext *Pixel_Format_Context;   //像素格式转换上下文

    AVFrame *Out_Data;                  //输出数据结构

    AVCodecContext *Encoder_Context;    //编码器上下文

    AVFormatContext *Format_Wrapper;    //rtmp flv 封装器

    AVCodec *codec; //视频编码器

    AVStream *out_stream;

    int m_width;    //视频宽度
    int m_height;   //视频高度
    int m_fps;      //视频帧数
    char *m_url;    //URL地址
};




#endif