#include "RecordSystem.hpp"

#include "../core/Engine.hpp"
#include "SDL_gpu.h"
#include "random.hpp"
#include <iostream>
#include <string>

using Random = effolkronium::random_static;

void RecordSystem::start_recording() {
  if (!recording) {
    recording = true;
    frame_count = 0;
    std::cout << "Recording started." << std::endl;
  } else {
    stop_recording();
  }
}

void RecordSystem::stop_recording() {
  if (recording) {
    recording = false;

    for (size_t i = 0; i < frames.size(); i++) {
      std::string filename =
          "res/recordings/frame_" + std::to_string(i) + ".png";
      GPU_SaveImage(frames[i], filename.c_str(), GPU_FILE_PNG);
      GPU_FreeImage(frames[i]);
    }
  }
}

void RecordSystem::capture_frame(double dt) {
  if (recording) {
    m_current_frame += dt * 1000.0;
    if (m_current_frame < m_frame_delay) {
      return;
    }
    // Assuming 'gpu' is a valid GPU_Target* initialized elsewhere
    GPU_Image *frame = GPU_CopyImageFromTarget(GPU_GetActiveTarget());
    frames.push_back(frame);
    frame_count++;
    m_current_frame = 0;
    std::cout << "Captured frame " << frame_count << std::endl;
  }
}

void RecordSystem::take_screenshot() {
  std::string filename = "res/screenshots/screenshot_" +
                         std::to_string(Random::get(10000, 99999)) + ".png";
  // Assuming 'gpu' is a valid GPU_Target* initialized elsewhere
  GPU_Image *screenshot = GPU_CopyImageFromTarget(GPU_GetActiveTarget());
  GPU_SaveImage(screenshot, filename.c_str(), GPU_FILE_PNG);
  GPU_FreeImage(screenshot);
  std::cout << "Screenshot taken: " << filename << std::endl;
}
