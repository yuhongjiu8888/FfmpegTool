g++ main.cpp -o test -I../src \
 -I/usr/local/include/opencv4 \
 -L/usr/local/lib -lopencv_core -lopencv_videoio -lopencv_highgui \
 -L../lib -lautopusher  \
 -L/usr/local/ffmpeg -lavcodec -lavformat -lswscale 