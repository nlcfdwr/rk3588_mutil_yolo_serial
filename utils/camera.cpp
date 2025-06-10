//
// Created by kaylor on 3/9/24.
//

#include "camera.h"
#include "kaylordut/log/logger.h"
#include "thread"


Camera::Camera(uint16_t index, cv::Size size, double framerate)
    : size_(size) {
  KAYLORDUT_LOG_INFO("Instantiate a Camera object");

  std::string pipeline = "rtsp://admin:admin@192.168.0.15/live/chn=1";
  KAYLORDUT_LOG_INFO("Using RTSP stream: {}", pipeline);

  for (int attempt = 0; attempt < 3; ++attempt) {
    capture_.open(pipeline);  // 不再使用 cv::CAP_GSTREAMER
    if (capture_.isOpened()) break;
    KAYLORDUT_LOG_WARN("Attempt {}: Failed to open RTSP stream. Retrying...", attempt + 1);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  if (!capture_.isOpened()) {
    KAYLORDUT_LOG_ERROR("Failed to open RTSP stream after multiple attempts");
    exit(EXIT_FAILURE);
  }

  // 延迟等待 RTSP 视频流稳定
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





// Camera::Camera(uint16_t index, cv::Size size, double framerate)
//     : capture_(index, cv::CAP_V4L2), size_(size) {
//   KAYLORDUT_LOG_INFO("Instantiate a Camera object");
//   // 这里使用V4L2捕获，因为使用默认的捕获不可以设置捕获的模式和帧率
//   if (!capture_.isOpened()) {
//     KAYLORDUT_LOG_ERROR("Error opening video stream or file");
//     exit(EXIT_FAILURE);
//   }
//   capture_.set(cv::CAP_PROP_FOURCC,
//                cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
//   // 检查是否成功设置格式
//   int fourcc = capture_.get(cv::CAP_PROP_FOURCC);
//   if (fourcc != cv::VideoWriter::fourcc('M', 'J', 'P', 'G')) {
//     KAYLORDUT_LOG_WARN("Set video format failed");
//   }
//   capture_.set(cv::CAP_PROP_FRAME_WIDTH, size_.width);
//   capture_.set(cv::CAP_PROP_FRAME_HEIGHT, size_.height);
//   if (!capture_.set(cv::CAP_PROP_FPS, framerate)) {
//     KAYLORDUT_LOG_WARN("set framerate failed!!");
//   }
//   std::this_thread::sleep_for(std::chrono::seconds(1));
//   KAYLORDUT_LOG_INFO("camera width: {}, height: {}, fps: {}",
//                      capture_.get(cv::CAP_PROP_FRAME_WIDTH),
//                      capture_.get(cv::CAP_PROP_FRAME_HEIGHT),
//                      capture_.get(cv::CAP_PROP_FPS));
// }


// Camera::Camera(uint16_t index, cv::Size size, double framerate)
//     : size_(size) {
//   KAYLORDUT_LOG_INFO("Instantiate a Camera object");

//   std::string pipeline = "gst-launch-1.0 v4l2src device=/dev/video11 ! \video/x-raw,format=NV12,width=1920,height=1080,framerate=30/1 ! autovideosink";

//   capture_.open(pipeline, cv::CAP_GSTREAMER);

//   if (!capture_.isOpened()) {
//     KAYLORDUT_LOG_ERROR("Error opening camera using GStreamer pipeline");
//     exit(EXIT_FAILURE);
//   }

//   // 设置分辨率可能失效，可以尝试在 pipeline 中设置
//   std::this_thread::sleep_for(std::chrono::seconds(1));
//   KAYLORDUT_LOG_INFO("camera width: {}, height: {}, fps: {}",
//                      capture_.get(cv::CAP_PROP_FRAME_WIDTH),
//                      capture_.get(cv::CAP_PROP_FRAME_HEIGHT),
//                      capture_.get(cv::CAP_PROP_FPS));
// }


// Camera::Camera(uint16_t index, cv::Size size, double framerate)
//     : size_(size) {
//   KAYLORDUT_LOG_INFO("Instantiate a Camera object");

//   std::string pipeline =
//       "v4l2src device=/dev/video11 ! "
//       "video/x-raw,format=NV12,width=" + std::to_string(size.width) +
//       ",height=" + std::to_string(size.height) +
//       ",framerate=" + std::to_string(static_cast<int>(framerate)) + "/1 ! "
//       "videoconvert ! video/x-raw,format=BGR ! "
//       "appsink";

//   KAYLORDUT_LOG_INFO("Using pipeline: {}", pipeline);

//   capture_.open(pipeline, cv::CAP_GSTREAMER);

//   if (!capture_.isOpened()) {
//     KAYLORDUT_LOG_ERROR("Error opening camera using GStreamer pipeline");
//     exit(EXIT_FAILURE);
//   }

//   std::this_thread::sleep_for(std::chrono::seconds(1));
//   KAYLORDUT_LOG_INFO("camera width: {}, height: {}, fps: {}",
//                      capture_.get(cv::CAP_PROP_FRAME_WIDTH),
//                      capture_.get(cv::CAP_PROP_FRAME_HEIGHT),
//                      capture_.get(cv::CAP_PROP_FPS));
// }






Camera::Camera(uint16_t index, cv::Size size, double framerate)
    : size_(size) {
  KAYLORDUT_LOG_INFO("Instantiate a Camera object");

  //构造 GStreamer pipeline 字符串
  std::string pipeline =
      "v4l2src device=/dev/video11 ! "
      "video/x-raw,format=NV12,width=" + std::to_string(size.width) +
      ",height=" + std::to_string(size.height) +
      ",framerate=" + std::to_string(static_cast<int>(framerate)) + "/1 ! "
      "videoconvert ! video/x-raw,format=BGR ! "
      "appsink drop=true max-buffers=1";

  KAYLORDUT_LOG_INFO("Using pipeline: {}", pipeline);

  //尝试多次打开 pipeline，防止偶发失败
  for (int attempt = 0; attempt < 3; ++attempt) {
    capture_.open(pipeline, cv::CAP_GSTREAMER);
    if (capture_.isOpened()) break;
    KAYLORDUT_LOG_WARN("Attempt {}: Failed to open camera. Retrying...", attempt + 1);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  if (!capture_.isOpened()) {
    KAYLORDUT_LOG_ERROR("Failed to open camera after multiple attempts");
    exit(EXIT_FAILURE);
  }

  //延迟等待摄像头稳定
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  KAYLORDUT_LOG_INFO("camera width: {}, height: {}, fps: {}",
                     capture_.get(cv::CAP_PROP_FRAME_WIDTH),
                     capture_.get(cv::CAP_PROP_FRAME_HEIGHT),
                     capture_.get(cv::CAP_PROP_FPS));
}