#include "serial_sender.h"
#include <iostream>

SerialSender::SerialSender(const std::string &port, int baudrate) {
  fd_ = open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd_ == -1) {
    std::cerr << "Failed to open serial port: " << port << std::endl;
    return;
  }

  struct termios options;
  tcgetattr(fd_, &options);

  cfsetispeed(&options, baudrate);
  cfsetospeed(&options, baudrate);

  options.c_cflag |= (CLOCAL | CREAD);
  options.c_cflag &= ~PARENB;
  options.c_cflag &= ~CSTOPB;
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;

  tcsetattr(fd_, TCSANOW, &options);
}

SerialSender::~SerialSender() {
  if (fd_ != -1) {
    close(fd_);
  }
}

bool SerialSender::sendData(const std::string &data) {
  if (fd_ == -1) return false;
  int n = write(fd_, data.c_str(), data.size());
  return n == static_cast<int>(data.size());
}

bool SerialSender::isOpen() const {
  return fd_ != -1;
}
