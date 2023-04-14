#include <opencv2/opencv.hpp>

#include "FfmpegTool.hpp"
int main() {
    FfmpegTool pusher(1280, 720, 30, "rtmp://192.168.3.213/live/test");
    pusher.Init();
    pusher.init_RGB_to_YUV();
    pusher.InitOutPutData();
    pusher.InitEncodeContext();
    pusher.CreatFormatContext();

    int frameCount = 0;

    cv::VideoCapture video;  // 用VideoCapture来读取摄像头
    cv::Mat frame;           // 声明一个保存图像的类
    video.open(0);           // 括号的0表示使用电脑自带的摄像头
    if (!video.isOpened())   // 判断摄像头是否读取成功
    {
        std::cout << "open video error! \n";
        return -1;  // 返回一个代数值，表示函数失败（若为return 1，则表示ture）
    }
    int width = video.get(cv::CAP_PROP_FRAME_WIDTH);    // 帧宽度
    int height = video.get(cv::CAP_PROP_FRAME_HEIGHT);  // 帧高度
    std::cout << "before:" << width << " ," << height << std::endl;

    video.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    video.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
    video.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    width = video.get(cv::CAP_PROP_FRAME_WIDTH);    // 帧宽度
    height = video.get(cv::CAP_PROP_FRAME_HEIGHT);  // 帧高度
    std::cout << "after:" << width << " ," << height << std::endl;
    while (1) {
        video >> frame;
        if (frame.data == NULL) break;
        std::cout << "111111\n";
        frameCount++;
        cv::imshow("origin", frame);  // 使用imshow语句将图片显示出来
        cv::waitKey(30);              // 停顿30ms
        pusher.Rtsp_Pusher(frame, frameCount);
    }

    return 0;
}