CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
project(autopusher C CXX)

# 设置指定的C++编译器版本是必须的，如果不设置，或者为OFF，则指定版本不可用时，会使用上一版本。
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_subdirectory: 添加一个编译的子路径，在子路径中查找CMakeLists.txt文件，并进行编译。

# 指定为C++11 版本
set(CMAKE_CXX_STANDARD 11)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/3rdparts/include/libavcodec)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/3rdparts/include/libavformat)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/3rdparts/include/libswscale)

ADD_DEFINITIONS(-DAUTO_EXPORTS)

#SET(3RDPARTS_LIBS "")
FILE(GLOB 3RDPARTS_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/./3rdparts/lib)

FILE(GLOB 3RDPARTS_STATIC_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/./3rdparts/lib_static/*.a)

#link_directories(${3RDPARTS_LIBS})

SET(CPP_SRC
    src/FfmpegTool.cpp
    )
#1.
ADD_LIBRARY(${PROJECT_NAME} SHARED
${CPP_SRC})

#2.如果使用将第三方的静态库链接到项目中的动态库中则开启如下命令,如果不将三方的静态库链接到动态库中则不执行这步
# target_link_libraries(
#     ${PROJECT_NAME}
#     ${${PROJECT_NAME}_EXTRA_LDFLAGS}
#     "-Wl,--whole-archive" #告诉编译器，从这里开始，所有的库的内容都包含到so中
#     ${3RDPARTS_STATIC_LIBS} #以是预先生成的静态库
#     "-Wl,--no-whole-archive" #告诉编译器，从这里开始，以后的库的内容不用都包含到so中
# )

SET_TARGET_PROPERTIES(autopusher PROPERTIES VERSION 1.1.0 SOVERSION 1.1) #设置版本号

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_SOURCE_DIR}/lib)








