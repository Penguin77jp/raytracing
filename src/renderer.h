#pragma once

#include "ray.h"
#include "random.h"
#include "sceneData.h"
#include "renderTarget.h"
#include "camera.h"

#include "embree3/rtcore.h"

namespace png {
  class Renderer {
  private:
    Scene& scene;
    RenderTarget& renderTarget;
    Camera& cam;
    RTCScene sceneHandle;
    RTCDevice deviceHandle;
    RTCIntersectContext context;
    vec3 PathTracing(RTCRayHit& rayhit, int depth, Random& rnd);
    vec3 PrimalRayTracing(RTCRayHit& rayhit);
    vec3 LambertDiffuse(RTCRayHit& rayhit);
  public:
    Renderer(Scene& pram_scene, RenderTarget& pram_renderTarget, Camera& pram_cam);
    ~Renderer();
    /*Renderer operator=(const Renderer& a) {
      this->sceneHandle = a.sceneHandle;
      this->deviceHandle = a.deviceHandle;
      this->context = a.context;
      this->g_data = a.g_data;
      return *this;
    }*/
    void Draw();
  };
}
