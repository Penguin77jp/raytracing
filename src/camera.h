#pragma once
#include "ray.h"

namespace png {
  class CameraLens {
  public:
    CameraLens();
    virtual Ray GenerateRay() = 0;
  };

  class PinHole : public CameraLens {
  public:
    PinHole();
    Ray GenerateRay() override;
  };

  class ThinLens : public CameraLens {
  public:
    ThinLens();
    Ray GenerateRay() override;
  };

  class Camera {
  public:
    int width, height;
    vec3 origin;
    vec3 target;
    int type;
    //CameraLens* camLens;
    vec3 direction;
    vec3 upVec;
    vec3 l_camX;
    vec3 l_camY;
    vec3 l_camZ;
    float fov;
  public:
    Camera(int, int, vec3, vec3, int, float);
    Ray GenerateRay(const int x, const int y);
  };
}
