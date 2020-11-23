#pragma once

#include "ray.h"
#include "texture.h"
#include <cmath>
#include <numbers>
#include <iostream>

namespace png {
  class SceneLight {
  public:
    float multiply;

  private:
    vec3* ambientLight;
    Texture* enviromentLight;

  public:
    SceneLight(float mul = 1.0f) : multiply(mul), ambientLight(nullptr), enviromentLight(nullptr) {}
    SceneLight(vec3* col, float mul = 1.0f) : multiply(mul), ambientLight(col), enviromentLight(nullptr) {}
    SceneLight(Texture* tex, float mul = 1.0f) : multiply(mul), ambientLight(nullptr), enviromentLight(tex) {}

    vec3 GetColor(vec3 dir) {
      if (ambientLight != nullptr) {
        return *ambientLight * multiply;
      } else if (enviromentLight != nullptr) {
        const float length = std::sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
        double theta = std::acosf(dir.y / length);
        double phi = dir.x / std::abs(dir.x) * std::acosf(dir.z / std::sqrtf(dir.z * dir.z + dir.x * dir.x));
        double theta01 = theta / std::numbers::pi;
        double phi01 = phi / std::numbers::pi * 0.5f + 0.5f;
        if (std::isnan(phi01) || std::abs(phi01 - 1.0f) < 1.0e-10f) {
          phi01 = 0;
        }
        if (std::isnan(theta01) || std::abs(theta01 - 1.0f) < 1.0e-10f) {
          theta01 = 0;
        }
        return enviromentLight->GetColor(enviromentLight->width * phi01, enviromentLight->height * theta01) * multiply;
      } else {
        return vec3{};
      }
    }

  };
}
