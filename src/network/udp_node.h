#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

#include <sodium.h>

class UdpNode{
public:
  UdpNode(int bind_port);
  ~UdpNode();

  void listen();
  void send(const std::string& ip_str, int port, const std::string& message);
 

  std::string get_my_pubkey_hex() const;
  bool set_peer_pubkey_from_hex(const std::string& hex_str);

private:
  int sockfd = -1;
  int listen_port;

  unsigned char my_publickey[crypto_box_PUBLICKEYBYTES];
  unsigned char my_secretkey[crypto_box_SECRETKEYBYTES];
  unsigned char peer_publickey[crypto_box_PUBLICKEYBYTES]{};
  bool have_peer = false;

  std::string encrypt(const std::string& plaintext) const;
  std::string decrypt(const std::string& data) const;
};

