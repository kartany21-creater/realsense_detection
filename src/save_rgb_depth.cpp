#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

int main() {
    // 出力ディレクトリの作成
    fs::create_directories("dataset/color");
    fs::create_directories("dataset/depth");

    // RealSenseのパイプラインと設定
    rs2::pipeline pipe;
    rs2::config cfg;
    cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, 30);
    cfg.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 30);
    pipe.start(cfg);

    std::cout << "📷 Capturing RGB + Depth images every 5 seconds..." << std::endl;

    int frame_num = 0;
    const int max_frames = 100;
    auto last_capture_time = std::chrono::steady_clock::now();

    while (frame_num < max_frames) {
        // フレーム取得（常に最新のものを受け取る）
        rs2::frameset frames = pipe.wait_for_frames();

        // 時間を計測
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_capture_time);

        if (elapsed.count() >= 5) {
            // RGBとDepthのフレーム取得
            rs2::frame color_frame = frames.get_color_frame();
            rs2::frame depth_frame = frames.get_depth_frame();

            // OpenCV形式に変換
            cv::Mat color(cv::Size(640, 480), CV_8UC3, (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
            cv::Mat depth(cv::Size(640, 480), CV_16UC1, (void*)depth_frame.get_data(), cv::Mat::AUTO_STEP);

            // ファイル名を作成
            std::ostringstream ss;
            ss << std::setw(4) << std::setfill('0') << frame_num;
            std::string color_name = "dataset/color/frame_" + ss.str() + ".png";
            std::string depth_name = "dataset/depth/frame_" + ss.str() + ".png";

            // 保存
            cv::imwrite(color_name, color);
            cv::imwrite(depth_name, depth);

            std::cout << "✅ Saved frame " << frame_num << " at 5-second interval" << std::endl;

            frame_num++;
            last_capture_time = now;  // 時刻リセット
        }
    }

    pipe.stop();
    std::cout << "🎉 Capture completed. Files saved in ./dataset" << std::endl;
    return 0;
}
