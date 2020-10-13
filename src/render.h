#pragma once

#include "ray.h"
#include "embree3/rtcore.h"
#include <vector>
#include "sceneData.h"

namespace png {
  class Renderer {
  private:
    RTCScene sceneHandle;
    RTCDevice deviceHandle;
    RTCIntersectContext context;
    vec3 Hit(RTCRayHit& rayhit, int depth);
    vec3 PrimalRayTracing(RTCRayHit& rayhit);
    vec3 LambertDiffuse(RTCRayHit& rayhit);
    Scene scene;
  public:
    Renderer();
    ~Renderer();
    void Draw(std::vector<float>& image);
  };
}
