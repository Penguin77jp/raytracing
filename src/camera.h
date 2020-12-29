#pragma once

#include "ray.h"
#include "renderTarget.h"
#include "random.h"
#include "aperture.h"

#include <iostream>
#include <memory>

namespace png {
  class Camera {
  protected:
    //RenderTarget& renderTarget;
    vec3 origin;
    vec3 target;
    int type;
    vec3 direction;
    vec3 upVec;
    vec3 l_camX;
    vec3 l_camY;
    vec3 l_camZ;
    float fov;
  public:
    Camera(vec3 _org, vec3 _tar, int _type, float _fov = 1.0f);
    //Camera(const Camera& _cam) {}

    void Init();
    Ray virtual GenerateRay(const std::shared_ptr<RenderTarget> _renderTarget, const int x, const int y, png::Random& rnd) const = 0;

    //Get
    float GetFov() const { return fov; }
    int GetType() const { return type; }
    vec3 GetOrigin() const { return origin; }
    vec3 GetTarget() const { return target; }

    //Set
    void SetOrigin(const vec3 _org) {
      origin = _org;
      Init();
    }
    void SetFov(const float _fov) {
      fov = _fov;
      Init();
    }
  };

  class PinHole : public Camera {
  public:
    PinHole(vec3 _org, vec3 _tar, int _type, float _fov = 1.0f);
    Ray GenerateRay(
      const std::shared_ptr<RenderTarget> _renderTarget,
      const int x, const int y,
      png::Random& rnd
    )const;
  };

  class ThinLens : public Camera {
  private:
    float apertureDiameter;
    float forcalDis;
    Aperture* aperture;
  public:
    ThinLens(vec3 _org, vec3 _tar, int _type, float _fov, Aperture* _aperture, const float _apertureDiameter = 0.0f, const float _forcalDis = 1.0f);
    Ray GenerateRay(
      const std::shared_ptr<RenderTarget> _renderTarget,
      const int x, const int y,
      png::Random& rnd
    ) const;
    // Get

    // Set
    void SetAperture(const float _val) {
      apertureDiameter = _val;
    }
    void SetForcalDis(const float _dis) {
      forcalDis = _dis;
    }
    void SetApertureType(const type_info& _type) {
      if (_type == typeid(ApertureDisk*)) {
        delete aperture;
        aperture = new ApertureDisk();
      } else if (_type == typeid(AperturePolygonRejection*)) {
        delete aperture;
        aperture = new AperturePolygonRejection();
      } else if (_type == typeid(ApertureRectangle*)) {
        delete aperture;
        aperture = new ApertureRectangle();
      } else {
        std::cerr << "Not Found class " << typeid(aperture).name() << std::endl;
        exit(1);
      }
    }
  };
}
