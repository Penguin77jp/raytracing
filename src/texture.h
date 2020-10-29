#pragma once

#include "ray.h"

namespace png {
  class Texture {
  public:
    Texture(const char* fileName);
    vec3 GetColor(int x, int y);

    int width, height, bpp;
    unsigned char* image_data;
  };
}
