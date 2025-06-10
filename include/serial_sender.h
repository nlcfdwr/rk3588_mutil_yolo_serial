#ifndef SERIAL_SENDER_H
#define SERIAL_SENDER_H

#include <string>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

class SerialSender {
public:
  SerialSender(const std::string &port, int baudrate);
  ~SerialSender();
  bool sendData(const std::string &data);
  bool isOpen() const;

private:
  int fd_; // 文件描述符
};

#endif // SERIAL_SENDER_H
