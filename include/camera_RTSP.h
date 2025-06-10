#pragma once

#include <opencv2/opencv.hpp>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>

class Camera {
public:
  // 构造函数
  Camera(uint16_t index, cv::Size size, double framerate);

  // 析构函数
  ~Camera();

  // 获取最新帧（返回的是指向新 Mat 的智能指针）
  std::unique_ptr<cv::Mat> GetNextFrame();

private:
  cv::Size size_;                        // 图像大小
  cv::VideoCapture capture_;            // OpenCV 视频捕获器

  std::mutex frame_mutex_;              // 互斥锁保护帧数据
  std::atomic<bool> running_{true};     // 控制线程退出标志
  std::unique_ptr<cv::Mat> latest_frame_; // 最新帧缓存
  std::thread capture_thread_;          // 后台拉流线程
};
