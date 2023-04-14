ffmpeg处理工具：
适用于视频通过opencv进行图片帧处理后，进行rtmp、rtsp推流。

opencv版本：2.4.13.6

ffmpeg版本：ffmpeg version 2.8.17-0ubuntu0.1 Copyright (c) 2000-2020 the FFmpeg developers

编译说明：

    项目中的build.sh编译推流库；
    test目录下为demo测试程序,build.sh编译测试程序；


解耦说明：
    编译此项目为动态库，只需要依赖三方库头文件即可编译成功；在实际项目中链接需要自行添加动态库链接，可以参考test目录下：

    ```
    g++ main.cpp -o test -I../src \
    -I/usr/local/include/opencv4 \
    -L/usr/local/lib -lopencv_core -lopencv_videoio -lopencv_highgui \
    -L../lib -lautopusher  \
    -L/usr/local/ffmpeg -lavcodec -lavformat -lswscale 
    ```


第三方库版本说明：
    
    ffmpeg版本不要太高，最好是在ffmpeg-4.4.2 以下（ffmpeg-3.1），最好是在3.0版本上，记得带上x264 x265编解码；


    opencv版本也可使用4.0版本即可（4.0.1 4.5.1），编译时需带上ffmpeg模块编译进去；

特别说明：目前仓库中的ffmpeg版本是Linux x86_64版本，如果需要支持交叉编译，请自行编译ffmpeg、opencv。
        此推流也可用于嵌入式设备，用对应工具链编译即可；