cmake_minimum_required(VERSION 3.28)
project(bai2d)

set(CMAKE_CXX_STANDARD 17)

add_executable(bai2d main.cpp
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
        bai2d_mesh.cpp
        bai2d_mesh.h
        bai2d_collision.cpp
        bai2d_collision.h
        bai2d_shortcut_func.h
        bai2d_scene.cpp
        bai2d_scene.h
        bai2d_auto_release.cpp
        bai2d_auto_release.h
)

set(EasyX_INC "${CMAKE_SOURCE_DIR}/EasyX/include")
set(EasyX_LINK "${CMAKE_SOURCE_DIR}/EasyX/lib64")

include_directories(${EasyX_INC})
link_directories(${EasyX_LINK})

# 在lib对应的文件夹里面找lib文件（一般只有EasyXa和EasyXw这两个，进行链接）
target_link_libraries(bai2d ${EasyX_LINK}/libeasyx.a)

target_link_libraries(bai2d gdi32)