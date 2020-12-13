#pragma once
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <iostream>
#include <algorithm>

png::Texture::Texture(const char* fileName) {
  image_data = stbi_load(fileName, &width, &height, &bpp, 0);
  std::cout << width << " , " << height << " , " << bpp << std::endl;
}

png::Texture::Texture(std::vector<unsigned char>& data, unsigned int width, unsigned int height)
  :width(width), height(height), bpp(data.size() / width / height), image_data(&data[0]) {
  std::cout << width << " , " << height << " , " << bpp << std::endl;
}

int png::Texture::GetIndexX(int x) {
  if (x >= width) {
    return width - 1;
  }
  return x;
}
int png::Texture::GetIndexY(int y) {
  if (y >= height) {
    return height - 1;
  }
  return y;
}

png::vec3 png::Texture::GetColorInt(int x, int y, const LoadTextureType _loadTextureType) {
  png::vec3 val = png::vec3(
    image_data[x * bpp + y * width * bpp] / 255.0f,
    image_data[x * bpp + y * width * bpp + 1] / 255.0f,
    image_data[x * bpp + y * width * bpp + 2] / 255.0f
  );
  if (_loadTextureType == LoadTextureType::Normal) {
    return val;
  } else if (_loadTextureType == LoadTextureType::GammmaCorrection) {
    const float l_pow = 2.2f;
    return png::vec3(std::powf(val.x, l_pow), std::powf(val.y, l_pow), std::powf(val.z, l_pow));
  } else {
    return png::vec3(-1, -1, -1);
  }
}

png::vec3 png::Texture::GetColor(double x, double y, const LoadTextureType _loadTextureType) {
  return GetColorInt(GetIndexX(x), GetIndexY(y), _loadTextureType);
}

png::vec3 png::Texture::GetColorLerp(double x, double y, LoadTextureType _loadTextureType) {
  png::vec3 val00 = GetColorInt(GetIndexX(x), GetIndexY(y), _loadTextureType);
  png::vec3 val10 = GetColorInt(GetIndexX(x + 1), GetIndexY(y), _loadTextureType);
  png::vec3 val01 = GetColorInt(GetIndexX(x), GetIndexY(y + 1), _loadTextureType);
  png::vec3 val11 = GetColorInt(GetIndexX(x + 1), GetIndexY(y + 1), _loadTextureType);

  const float weight_x = x - (int)x;
  const float weight_y = y - (int)y;
  png::vec3 val = val00 * (1.0f - weight_x) * (1.0f - weight_y) + val10 * weight_x * (1.0f - weight_y)
    + val01 * (1.0f - weight_x) * weight_y + val11 * weight_x * weight_y;

  return val;
}

void png::Texture::WriteImage(const char* fileName
                              , const LoadTextureType _loadTextureType) {
  stbi_write_png(fileName, width, height, bpp, image_data, 0);
  std::cout << "saved " << fileName << std::endl;
}
