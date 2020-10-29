#pragma once

#include <numbers>
#include "ray.h"
#include "random.h"

namespace png {
  struct Material {
    virtual vec3 GetDirection(const vec3& direction, const vec3& normal, Random& sampler) = 0;
    virtual vec3 GetColor() = 0;
    virtual vec3 GetEmission() = 0;
  };

  class MaterialDiffuse : public Material {
  private:
    vec3 color;
    float kd;
    float emission;

  public:
    MaterialDiffuse(const vec3& col, const float kd, const float emission) : color(col), kd(kd), emission(emission) {}

    vec3 GetDirection(const vec3& direction, const vec3& normal, Random& sampler) override {
      vec3 dir;
      vec3 w, u, v;
      w = vec3::Normalize(vec3{ normal.x,normal.y,normal.z });
      constexpr float kEPS = 1e-6;
      if (fabs(w.x) > kEPS) // ベクトルwと直交するベクトルを作る。w.xが0に近い場合とそうでない場合とで使うベクトルを変える。
        u = vec3::Normalize(vec3::Cross(vec3(0.0, 1.0, 0.0), w));
      else
        u = vec3::Normalize(vec3::Cross(vec3(1.0, 0.0, 0.0), w));
      v = vec3::Cross(w, u);
      while (true) {
        const double r1 = 2.0 * std::numbers::pi * sampler.next01();
        const double r2 = std::acos(2.0 * sampler.next01() - 1.0);
        dir = w * std::sin(r2) * std::cos(r1) + u * std::sin(r2) * std::sin(r1) + v * std::cos(r2);
        if (vec3::Dot(dir, w) >= 0) {
          break;
        }
      }
      return dir;
    }
    vec3 GetColor() override {
      return color * kd;
    }
    vec3 GetEmission() override {
      return color * emission;
    }
  };

  class MaterialReflect : public Material {
  private:
    vec3 color;
    float kd;
    float emission;

  public:
    MaterialReflect(const vec3& col, const float kd, const float emission) : color(col), kd(kd), emission(emission) {}

    vec3 GetDirection(const vec3& direction, const vec3& normal, Random& sampler) override {
      return direction - normal * 2.0 * vec3::Dot(normal, direction);
    }
    vec3 GetColor() override {
      return color * kd;
    }
    vec3 GetEmission() override {
      return color * emission;
    }
  };
}
