#pragma once
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <iostream>

png::Texture::Texture(const char* fileName) {
  image_data = stbi_load(fileName, &width, &height, &bpp, 0);
  std::cout << width << " , " << height;
}

png::vec3 png::Texture::GetColor(int x, int y) {
  return png::vec3(image_data[x * bpp + y * width * bpp] / 255.0f,
                   image_data[x * bpp + y * width * bpp + 1] / 255.0f,
                   image_data[x * bpp + y * width * bpp + 2] / 255.0f);
}

void png::Texture::WriteImage(const char* fileName) {
  stbi_write_png(fileName, width, height, bpp, image_data, 0);
  std::cout << "saved " << fileName << std::endl;
}
