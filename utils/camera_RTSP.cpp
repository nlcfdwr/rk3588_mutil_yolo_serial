#include "camera_RTSP.h"
#include "kaylordut/log/logger.h"
#include "thread"


Camera::Camera(uint16_t index, cv::Size size, double framerate)
    : size_(size) {
  KAYLORDUT_LOG_INFO("Instantiate a Camera object");

  // 构造 GStreamer 管道，等效于你测试效果好的 gst-launch 命令
  std::string gst_pipeline =
      "rtspsrc location=rtsp://admin:admin@192.168.0.15/live/chn=1 latency=60 ! "
      "rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! appsink";

  KAYLORDUT_LOG_INFO("Using GStreamer pipeline: {}", gst_pipeline);

  for (int attempt = 0; attempt < 3; ++attempt) {
    capture_.open(gst_pipeline, cv::CAP_GSTREAMER);
    if (capture_.isOpened()) break;
    KAYLORDUT_LOG_WARN("Attempt {}: Failed to open GStreamer RTSP stream. Retrying...", attempt + 1);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  if (!capture_.isOpened()) {
    KAYLORDUT_LOG_ERROR("Failed to open RTSP stream after multiple attempts");
    exit(EXIT_FAILURE);
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  KAYLORDUT_LOG_INFO("RTSP stream width: {}, height: {}, fps: {}",
                     capture_.get(cv::CAP_PROP_FRAME_WIDTH),
                     capture_.get(cv::CAP_PROP_FRAME_HEIGHT),
                     capture_.get(cv::CAP_PROP_FPS));
}

Camera::~Camera() {
  if (capture_.isOpened()) {
    KAYLORDUT_LOG_INFO("Release camera");
    capture_.release();
  }
}

std::unique_ptr<cv::Mat> Camera::GetNextFrame() {
  auto frame = std::make_unique<cv::Mat>();
  capture_ >> *frame;
  if (frame->empty()) {
    KAYLORDUT_LOG_ERROR("Get frame error");
    return nullptr;
  }
  return std::move(frame);
}





















// Camera::Camera(uint16_t index, cv::Size size, double framerate) : size_(size) {
//   //std::string pipeline = "rtsp://admin:a12345678@192.168.1.199:554/cam/realmonitor?channel=1%26subtype=1";
//   // std::string pipeline =     "rtspsrc location=rtsp://admin:admin@192.168.0.15/live/chn=1 latency=30 ! "
//   //                            "rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! appsink";
//   std::string pipeline = "rtsp://admin:admin@192.168.0.15/live/chn=1";
//   for (int attempt = 0; attempt < 3; ++attempt) {
//     capture_.open(pipeline, cv::CAP_GSTREAMER);
//     if (capture_.isOpened()) break;
//     std::this_thread::sleep_for(std::chrono::seconds(1));
//   }

//   if (!capture_.isOpened()) {
//     std::cerr << "Failed to open RTSP stream" << std::endl;
//     exit(EXIT_FAILURE);
//   }

//   // 后台线程读取最新帧
//   capture_thread_ = std::thread([this]() {
//     while (running_) {
//       cv::Mat frame;
//       capture_ >> frame;
//       if (frame.empty()) continue;

//       {
//         std::lock_guard<std::mutex> lock(frame_mutex_);
//         latest_frame_ = std::make_unique<cv::Mat>(frame); // 始终只保存最新帧
//       }
//     }
//   });
// }

// Camera::~Camera() {
//   running_ = false;
//   if (capture_thread_.joinable())
//     capture_thread_.join();

//   if (capture_.isOpened())
//     capture_.release();
// }

// std::unique_ptr<cv::Mat> Camera::GetNextFrame() {
//   std::lock_guard<std::mutex> lock(frame_mutex_);
//   if (!latest_frame_ || latest_frame_->empty()) return nullptr;
//   return std::make_unique<cv::Mat>(*latest_frame_); // 返回帧的拷贝
// }
