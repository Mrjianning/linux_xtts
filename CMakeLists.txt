cmake_minimum_required(VERSION 3.10)

# 项目名称
project(aikit_project)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 设置可执行文件的输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/workspace)

find_package(nlohmann_json 3.11.3 REQUIRED PATHS ~/project/rtsp_opencv_gpu_server/lib/install_json) # json c++

# 包含头文件目录
include_directories(
    ${PROJECT_SOURCE_DIR}/lib/tts_libs/include
    ${PROJECT_SOURCE_DIR}/lib/install_json/include
    ${PROJECT_SOURCE_DIR}/src/
)

# 链接库目录
link_directories(
    ${PROJECT_SOURCE_DIR}/lib/tts_libs/lib
    )

# 添加可执行文件
add_executable(aikit_test 
    src/XTTSClient.cpp
    src/main.cpp
)

# 链接库文件
target_link_libraries(
    aikit_test 
    aikit
    nlohmann_json::nlohmann_json
    )

# 设置运行时库路径
set_target_properties(aikit_test PROPERTIES
    BUILD_RPATH ${PROJECT_SOURCE_DIR}/lib/tts_libs/lib
)
