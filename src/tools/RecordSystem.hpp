#pragma once

#include <vector>
class GPU_Image;

class RecordSystem {
public:
  RecordSystem() = default;
  ~RecordSystem() = default;

  void start_recording();
  void stop_recording();
  void capture_frame(double dt);
  void take_screenshot();

private:
  bool recording = false;
  int frame_count = 0;
  double m_current_frame = 0;
  double m_frame_delay = 1000.0 / 30.0; // 30 FPS

  std::vector<GPU_Image *> frames;
};
