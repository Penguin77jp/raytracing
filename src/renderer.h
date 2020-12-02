#pragma once

#include <memory>

#include "ray.h"
#include "random.h"
#include "sceneData.h"
#include "renderTarget.h"
#include "camera.h"

#include "embree3/rtcore.h"

namespace png {
  class Renderer {
  private:
    Camera* cam;
    std::shared_ptr<RenderTarget> renderTarget;
    Scene& scene;
    int superSampling;
    RTCScene sceneHandle;
    RTCDevice deviceHandle;
    RTCIntersectContext context;
    vec3 PathTracing(RTCRayHit& rayhit, int depth, Random& rnd);
    vec3 PrimalRayTracing(RTCRayHit& rayhit);
    vec3 LambertDiffuse(RTCRayHit& rayhit);

  public:
    Renderer(Camera* _cam, const std::shared_ptr<RenderTarget> _renderTarget, Scene& _scene);
    Renderer(const Renderer& _renderer);
    ~Renderer();
    Renderer& operator=(const Renderer& a);
    void Draw();

    //Get
    // Render Target
    //int GetSampleCounter() { return renderTarget.GetSampleCounter(); };
    //int GetRenderResolution() { return renderTarget.GetWidth(); };
    //int GetSuperSampling() { return renderTarget.GetSuperSampling(); };
    // Camera
    vec3 GetCameraOrigin() { return cam->GetOrigin(); };
    vec3 GetCameraTarget() { return cam->GetTarget(); };
    float GetCameraFov() { return cam->GetFov(); };
    const type_info& GetCameraLensType() const {
      if (static_cast<ThinLens*>(cam) != nullptr) {
        return typeid(ThinLens*);
      } else if (static_cast<PinHole*>(cam) != nullptr) {
        return typeid(PinHole*);
      } else {
        return typeid(nullptr);
      }
      /*if (static_cast<PinHole*>(_cam) != nullptr) {
        return typeid(PinHole*).name();
      } else if (static_cast<ThinLens*>(_cam) != nullptr) {
        return typeid(ThinLens*).name();
      } else {
        return typeid(nullptr).name();
      }*/
    }
    // Scene
    float GetSceneLightIntensity() { return scene.GetSceneLight().GetIntensity(); };
    // Render Target
    //unsigned int* GetRenderTargetID_ptr() { return renderTarget.GetImageID_ptr(); };

    //Set
    // Render Target
    void SetRenderTarget(const std::shared_ptr<RenderTarget> _renderTarget) { renderTarget = _renderTarget; }
    void SetSuperSampling(int _ss) { superSampling = _ss; }
    // Camera
    void SetCameraType(int _type) {};
    void SetCameraOrigin(const vec3 _org) {
      cam->SetOrigin(_org);
      renderTarget.get()->Init();
    };
    void SetCameraTarget(vec3 _tar) {};
    void SetCameraFov(float _fov) {
      cam->SetFov(_fov);
      renderTarget.get()->Init();
    };
    void SetCameraAperture(const float _aperture) {
      if (GetCameraLensType() == typeid(ThinLens*)) {
        static_cast<ThinLens*>(cam)->SetAperture(_aperture);
        renderTarget.get()->Init();
      }
    }
    void SetCameraForcalDis(const float _dis) {
      if (GetCameraLensType() == typeid(ThinLens*)) {
        static_cast<ThinLens*>(cam)->SetForcalDis(_dis);
        renderTarget.get()->Init();
      }
    }
    void SetCameraApertureType(const type_info& _type) {
      if (GetCameraLensType() == typeid(ThinLens*)) {
        ThinLens* lens = static_cast<ThinLens*>(cam);
        lens->SetApertureType(_type);
        renderTarget.get()->Init();
      }
    }
    // Scene
    void SetSceneLightIntensity(float _li) {};
  };
}
