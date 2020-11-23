#pragma once

#include <vector>

namespace png {
  class RenderTarget {
  public:
    uint16_t width, height;
    unsigned int superSampling;
    unsigned int sampleCounter;
    unsigned int* image_id; // GLuint
    std::vector<float> image_dataF;
    std::vector<unsigned char> image_data;

  public:
    RenderTarget(const uint16_t w, const uint16_t h);
    void Update();
    void Init();
    void WriteImage(const char* fileName);
  };
}
