#pragma once

#include "tracking/HandTypes.h"
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

// Receives hand landmarks from the Python MediaPipe sidecar over UDP.
class LandmarkReceiver
{
public:
  ~LandmarkReceiver();
  bool start(unsigned short port = 51234);
  std::vector<HandLandmarks> latest();
  void stop();

private:
  void loop();
  unsigned long long sock_ = ~0ull; // SOCKET (INVALID_SOCKET)
  std::thread thread_;
  std::mutex mutex_;
  std::vector<HandLandmarks> latest_;
  std::atomic<bool> running_{false};
};
