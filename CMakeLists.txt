cmake_minimum_required(VERSION 3.1)
project(realsense_rgbd_capture)

find_package(realsense2 REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(save_rgb_depth src/save_rgb_depth.cpp)
target_link_libraries(save_rgb_depth ${realsense2_LIBRARY} ${OpenCV_LIBS})

