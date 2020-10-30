#pragma once

#include "ray.h"
#include <vector>

namespace png {
  class Texture {
  public:
    Texture(const char* fileName);
    Texture(std::vector<unsigned char> data);
    vec3 GetColor(int x, int y);
    void WriteImage(const char* fileName);

    int width, height, bpp;
    unsigned char* image_data;
  };
}
