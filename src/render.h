#pragma once

#include "ray.h"
#include "embree3/rtcore.h"
#include <vector>
#include "sceneData.h"
#include "random.h"

namespace png {
  class Renderer {
  private:
    RTCScene sceneHandle;
    RTCDevice deviceHandle;
    RTCIntersectContext context;
    vec3 PathTracing(RTCRayHit& rayhit, int depth,Random &rnd);
    vec3 PrimalRayTracing(RTCRayHit& rayhit);
    vec3 LambertDiffuse(RTCRayHit& rayhit);
    Scene scene;
  public:
    Renderer();
    ~Renderer();
    void Draw(std::vector<float>& image);
  };
}
