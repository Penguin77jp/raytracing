#include "bluenoise.h"

png::BlueNoiseSampler::BlueNoiseSampler(Texture _tex)
  : blueNoise(_tex) {
}

png::vec3 png::BlueNoiseSampler::Sample(double x, double y) {
  const int width = blueNoise.width;
  const int height = blueNoise.height;
  const int indexX = (int)width * x;
  const int indexY = (int)height * y;
  const vec3 col = blueNoise.GetColor(x, y);

  return vec3((indexX + col.x) / width, (indexY + col.y) / height, 0);
}
