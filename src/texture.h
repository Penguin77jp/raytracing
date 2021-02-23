#pragma once

#include "ray.h"
#include <vector>

namespace png {
  enum class LoadTextureType {
    Normal, GammmaCorrection,
  };

  class Texture {
  public:
    int width, height, bpp;
    unsigned char* image_data;

  public:
    Texture(const char* fileName);
    Texture(std::vector<unsigned char>& data, unsigned int width, unsigned int height);
    vec3<> GetColor(int x, int y, const LoadTextureType _loadTextureType);
    vec3<> GetColorLerp(double x, double y, const LoadTextureType _loadTextureType);
    void WriteImage(const char* fileName, const LoadTextureType _loadTextureType = LoadTextureType::GammmaCorrection);
  private:
    vec3<> GetColorData(int x, int y, const LoadTextureType _loadTextureType);
    int GetIndexX(int x);
    int GetIndexY(int y);
  };
}
