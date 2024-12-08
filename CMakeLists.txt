cmake_minimum_required(VERSION 3.28)
project(Bai2D)

set(CMAKE_CXX_STANDARD 17)

add_executable(Bai2D main.cpp
        bai2d_event_base.cpp
        bai2d_event_base.h
        bai2d_input.cpp
        bai2d_input.h
        bai2d_constants.h
        bai2d_utils.h
        bai2d_registered_input.h
        bai2d_object.cpp
        bai2d_object.h
        bai2d_asset.cpp
        bai2d_asset.h
)

set(EasyX_INC "${CMAKE_SOURCE_DIR}/EasyX/include")
set(EasyX_LINK "${CMAKE_SOURCE_DIR}/EasyX/lib64")

include_directories(${EasyX_INC})
link_directories(${EasyX_LINK})

# 在lib对应的文件夹里面找lib文件（一般只有EasyXa和EasyXw这两个，进行链接）
target_link_libraries(Bai2D ${EasyX_LINK}/libeasyx.a)

target_link_libraries(Bai2D gdi32)