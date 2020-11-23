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

  class MaterialReflect : public Material {
  private:
  public:
    vec3 color;
    float kd;
    float emission;
    float supecularRatio;
    MaterialReflect(const vec3& col, const float kd, const float emission,float supecular=0.0f) : color(col), kd(kd), emission(emission),supecularRatio(supecular) {}

    vec3 GetDirection(const vec3& direction, const vec3& normal, Random& sampler) override {
      vec3 dir_diffuse;
      {
        vec3 w, u, v;
        w = vec3::Normalize(vec3{ normal.x,normal.y,normal.z });
        constexpr float kEPS = 1e-6;
        if (fabs(w.x) > kEPS) // ベクトルwと直交するベクトルを作る。w.xが0に近い場合とそうでない場合とで使うベクトルを変える。
          u = vec3::Normalize(vec3::Cross(vec3(0.0, 1.0, 0.0), w));
        else
          u = vec3::Normalize(vec3::Cross(vec3(1.0, 0.0, 0.0), w));
        v = vec3::Cross(w, u);

        const double r1 = 2 * std::numbers::pi * sampler.next01();
        const double r2 = sampler.next01(), r2s = sqrt(r2);
        dir_diffuse = vec3::Normalize((
          u * cos(r1) * r2s +
          v * sin(r1) * r2s +
          w * sqrt(1.0 - r2)));
      }

      vec3 dir_specular;
      {
        dir_specular = vec3::Normalize(direction - normal * 2.0 * vec3::Dot(normal, direction));
      }

      return dir_diffuse * (1.0f - supecularRatio) + dir_specular * supecularRatio;
    }
    vec3 GetColor() override {
      return color * kd;
    }
    vec3 GetEmission() override {
      return color * emission;
    }
  };
}
