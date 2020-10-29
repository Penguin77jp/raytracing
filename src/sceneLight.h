#pragma once

#include "ray.h"
#include "texture.h"
#include <cmath>
#include <numbers>
#include <iostream>

namespace png {
  class SceneLight {
  public:
    SceneLight() : ambientLight(nullptr), enviromentLight(nullptr) {}
    SceneLight(vec3 col) : ambientLight(&col), enviromentLight(nullptr) {}
    SceneLight(Texture tex) : ambientLight(nullptr), enviromentLight(&tex) {}

    vec3 GetColor(vec3 dir) {
      if (ambientLight != nullptr) {
        return *ambientLight;
      } else if (enviromentLight != nullptr) {
        const float length = std::sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
        float theta = std::acosf(dir.y / length);
        float phi = dir.x / std::abs(dir.x) * std::acosf(dir.z / std::sqrtf(dir.z * dir.z + dir.x * dir.x));
        float theta01 = theta / std::numbers::pi;
        float phi01 = phi / std::numbers::pi * 0.5f + 0.5f;
        if (std::isnan(phi01) || std::abs(phi01 - 1.0f) < 1.0e-10f) {
          phi01 = 0;
        }
        //std::cout << enviromentLight->width << " , " << enviromentLight->height;
        //std::cout << enviromentLight->width << " , " << phi01 << std::endl;
        return enviromentLight->GetColor(enviromentLight->width * phi01, enviromentLight->height * theta01);
      } else {
        return vec3{};
      }
    }
  private:
    vec3* ambientLight;
    Texture* enviromentLight;
  };
}
