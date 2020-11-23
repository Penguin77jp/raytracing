#pragma once

#include "ray.h"
#include <vector>

namespace png {
  class Texture {
  public:
    int width, height, bpp;
    unsigned char* image_data;

  public:
    Texture(const char* fileName);
    Texture(std::vector<unsigned char>& data, unsigned int width, unsigned int height);
    vec3 GetColor(float x, float y);
    void WriteImage(const char* fileName);
  private :
    vec3 GetColorInt(int x, int y);
    int GetIndexX(int x);
    int GetIndexY(int y);
  };
}
