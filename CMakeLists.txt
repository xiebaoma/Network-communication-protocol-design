cmake_minimum_required(VERSION 3.10)
project(high_performance_network_protocol)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 添加库的源文件（不包含main.cpp）
set(LIB_SOURCES
    src/Protocol.cpp
    src/CongestionControl.cpp
    src/LoadBalancer.cpp
    src/TcpChunkOptimization.cpp
    src/Utils.cpp
)

# 创建静态库
add_library(${PROJECT_NAME} STATIC ${LIB_SOURCES})

# 添加示例程序
add_executable(${PROJECT_NAME}_example src/main.cpp)

# 链接库到示例程序
target_link_libraries(${PROJECT_NAME}_example ${PROJECT_NAME})

# 链接系统网络库
if(WIN32)
    target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
    target_link_libraries(${PROJECT_NAME}_example wsock32 ws2_32)
else()
    target_link_libraries(${PROJECT_NAME} pthread)
    target_link_libraries(${PROJECT_NAME}_example pthread)
endif()

# 安装配置
install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# 安装头文件
install(DIRECTORY include/
    DESTINATION include/${PROJECT_NAME}
    FILES_MATCHING PATTERN "*.h"
)