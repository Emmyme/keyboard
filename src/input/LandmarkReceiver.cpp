#include "LandmarkReceiver.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <nlohmann/json.hpp>
#include <vector>

LandmarkReceiver::~LandmarkReceiver() { stop(); }

bool LandmarkReceiver::start(unsigned short port)
{
  WSADATA wsa;
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return false;
  SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (s == INVALID_SOCKET) return false;

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  InetPtonA(AF_INET, "127.0.0.1", &addr.sin_addr);
  if (bind(s, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == SOCKET_ERROR)
  {
    closesocket(s);
    return false;
  }
  DWORD timeout = 200; // ms, so the loop can poll running_
  setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&timeout), sizeof(timeout));

  sock_ = s;
  running_ = true;
  thread_ = std::thread(&LandmarkReceiver::loop, this);
  return true;
}

void LandmarkReceiver::loop()
{
  std::vector<char> buf(65536);
  while (running_)
  {
    int n = recv(static_cast<SOCKET>(sock_), buf.data(), static_cast<int>(buf.size()) - 1, 0);
    if (n <= 0) continue; // timeout or error
    buf[n] = '\0';

    auto j = nlohmann::json::parse(buf.data(), buf.data() + n, nullptr, false);
    if (j.is_discarded() || !j.contains("hands")) continue;

    std::vector<HandLandmarks> hands;
    for (const auto &h : j["hands"])
    {
      const auto &lm = h.is_object() && h.contains("lm") ? h["lm"] : h; // tolerate old array form
      if (!lm.is_array() || lm.size() < static_cast<size_t>(kLandmarkCount) * 3) continue;
      HandLandmarks hl;
      for (int i = 0; i < kLandmarkCount; ++i)
      {
        hl.points[i].x = lm[i * 3 + 0].get<float>();
        hl.points[i].y = lm[i * 3 + 1].get<float>();
        hl.points[i].z = lm[i * 3 + 2].get<float>();
      }
      hl.handedness = (h.is_object() && h.contains("hd")) ? h["hd"].get<int>() : -1;
      hl.score = 1.0f; // MediaPipe only reports detected hands
      hands.push_back(hl);
    }
    {
      std::lock_guard<std::mutex> lock(mutex_);
      latest_ = std::move(hands);
    }
  }
}

std::vector<HandLandmarks> LandmarkReceiver::latest()
{
  std::lock_guard<std::mutex> lock(mutex_);
  return latest_;
}

void LandmarkReceiver::stop()
{
  running_ = false;
  if (thread_.joinable()) thread_.join();
  if (sock_ != ~0ull)
  {
    closesocket(static_cast<SOCKET>(sock_));
    WSACleanup();
    sock_ = ~0ull;
  }
}
