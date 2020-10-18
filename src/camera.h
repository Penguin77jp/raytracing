#pragma once
#include "render.h"
#include "ray.h"

namespace png {
  class Aperture {

  };

  class ThinLens {

  };

  class Camera {
  public:
    int type;
    vec3 origin;
    vec3 target;
    vec3 direction;
    vec3 upVec;
    vec3 l_camX;
    vec3 l_camY;
    vec3 l_camZ;
    float fov;
    Camera(int type=0,vec3 org = vec3(), vec3 tar = vec3(), float fov = 1.0f, vec3 up = vec3()) :type(type), origin(org), target(tar), upVec(up), fov(fov) {
      direction = vec3::Normalize(target - origin);
      l_camX = vec3::Normalize(vec3::Cross(direction, upVec));
      l_camY = vec3::Cross(l_camX, direction);
      l_camZ = direction;
    }
    void Change() {
      *this = Camera(type,origin,target,fov,upVec);
    }
  };
}
