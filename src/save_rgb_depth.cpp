#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <thread>       // ← 追加
#include <chrono>       // ← 追加

namespace fs = std::filesystem;

int main() {
    fs::create_directories("dataset/color");
    fs::create_directories("dataset/depth");

    rs2::pipeline pipe;
    rs2::config cfg;
    cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, 30);
    cfg.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 30);
    pipe.start(cfg);

    std::cout << "📷 Capturing synchronized RGB + Depth images every 5 seconds..." << std::endl;

    int frame_num = 0;
    const int max_frames = 100;

    while (frame_num < max_frames) {
        rs2::frameset frames = pipe.wait_for_frames();
        rs2::frame color_frame = frames.get_color_frame();
        rs2::frame depth_frame = frames.get_depth_frame();

        cv::Mat color(cv::Size(640, 480), CV_8UC3, (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
        cv::Mat depth(cv::Size(640, 480), CV_16UC1, (void*)depth_frame.get_data(), cv::Mat::AUTO_STEP);

        std::ostringstream ss;
        ss << std::setw(4) << std::setfill('0') << frame_num;
        std::string color_name = "dataset/color/frame_" + ss.str() + ".png";
        std::string depth_name = "dataset/depth/frame_" + ss.str() + ".png";

        cv::imwrite(color_name, color);
        cv::imwrite(depth_name, depth);

        std::cout << "✅ Saved frame " << frame_num << std::endl;
        frame_num++;

        // ★★★ ここで5秒待機 ★★★
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    pipe.stop();
    std::cout << "🎉 Capture completed. Files saved in ./dataset" << std::endl;
    return 0;
}
