#pragma once
#include <string>

void udp_listen(int port);
void udp_send(const std::string& ip_str, int port, const std::string& message);
void udp_send_loop(const std::string & ip, int port);
