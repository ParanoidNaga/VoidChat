#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

class UdpNode{
public:
  UdpNode(int bind_port);
  ~UdpNode();

  void listen();
  void send(const std::string& ip_str, int port, const std::string& message);
  
private:
  int sockfd = -1;
  int listen_port;
};

