#include "camera.h"
#include "random.h"
#include <cmath>

png::Camera::Camera(vec3 _org, vec3 _tar, int _type, float _fov)
  : origin(_org)
  , target(_tar)
  , type(_type)
  , upVec(vec3{ +0.0f,+1.0f,+0.0f })
  , fov(_fov) {
  Init();
}

void png::Camera::Init() {
  direction = vec3::Normalize(target - origin);
  l_camX = vec3::Normalize(vec3::Cross(direction, upVec));
  l_camY = vec3::Cross(l_camX, direction);
  l_camZ = direction;
}

//png::Camera::Camera(const Camera& _cam) {
//  
//}


//png::Ray png::Camera::GenerateRay(const std::shared_ptr<RenderTarget> _renderTarget, const int x, const int y, png::Random& rnd) const {
//  constexpr float FLOAT_INFINITY = std::numeric_limits<float>::max();
//  const float fovx = fov;
//  const float fovy = fov * _renderTarget.get()->GetWidth() / _renderTarget.get()->GetHeight();
//
//  //Random rnd{ (unsigned int)((y + 1) * renderTarget.GetWidth() * renderTarget.GetHeight()) };
//  png::Ray cal;
//  cal.org = origin;
//  png::vec3 color;
//  vec3 dir = vec3::Normalize(
//    -l_camX * fovx * (2.0f * (x + rnd.next01()) / _renderTarget.get()->GetWidth() - 1.0f)
//    + l_camY * fovy * (2.0f * (y + rnd.next01()) / _renderTarget.get()->GetHeight() - 1.0f)
//    + l_camZ);
//  cal.dir = dir;
//
//  if (camLensType == 0) {
//  } else if (camLensType == 1) {
//
//  }
//  return cal;
//}

png::PinHole::PinHole(vec3 _org, vec3 _tar, int _type, float _fov)
  : Camera(_org, _tar, _type, _fov) {
}

png::Ray png::PinHole::GenerateRay(
  const std::shared_ptr<RenderTarget> _renderTarget,
  const int x, const int y,
  png::Random& rnd
) const {
  constexpr float FLOAT_INFINITY = std::numeric_limits<float>::max();
  const float fovx = fov;
  const float fovy = fov * _renderTarget.get()->GetWidth() / _renderTarget.get()->GetHeight();

  png::Ray cal;
  cal.org = origin;
  png::vec3 color;
  vec3 dir = vec3::Normalize(
    -l_camX * fovx * (2.0f * (x + rnd.next01()) / _renderTarget.get()->GetWidth() - 1.0f)
    + l_camY * fovy * (2.0f * (y + rnd.next01()) / _renderTarget.get()->GetHeight() - 1.0f)
    + l_camZ);
  cal.dir = dir;
  return cal;
}

png::ThinLens::ThinLens(vec3 _org, vec3 _tar, int _type, float _fov, Aperture* _aperture, const float _apertureDiameter, const float _forcalDis)
  : Camera(_org, _tar, _type, _fov)
  , aperture(_aperture)
  , apertureDiameter(_apertureDiameter)
  , forcalDis(_forcalDis) {
}

png::Ray png::ThinLens::GenerateRay(
  const std::shared_ptr<RenderTarget> _renderTarget,
  const int x, const int y,
  png::Random& rnd
) const {
  constexpr float FLOAT_INFINITY = std::numeric_limits<float>::max();
  const float fovx = fov;
  const float fovy = fov * _renderTarget.get()->GetWidth() / _renderTarget.get()->GetHeight();
  png::Ray cal;
  cal.org = origin;
  png::vec3 color;
  vec3 dir = vec3::Normalize(
    -l_camX * fovx * (2.0f * (x + rnd.next01()) / _renderTarget.get()->GetWidth() - 1.0f)
    + l_camY * fovy * (2.0f * (y + rnd.next01()) / _renderTarget.get()->GetHeight() - 1.0f)
    + l_camZ);
  cal.dir = dir;

  //debug
  /*if (static_cast<ApertureDisk*>(aperture) != nullptr) {
    std::cout << "aperture disk" << std::endl;
  } else if (static_cast<ApertureRectangle*>(aperture) != nullptr) {
    std::cout << "aperture rectangle" << std::endl;
  } else if (static_cast<AperturePolygonRejection*>(aperture) != nullptr) {
    std::cout << "aperture polygon rejection" << std::endl;
  }*/

  png::vec3 rd = aperture->Sample(rnd) * apertureDiameter;
  png::vec3 offset = l_camX * rd.x + l_camY * rd.y;
  cal.org = cal.org + offset;
  cal.dir = vec3::Normalize(cal.dir - offset / forcalDis);
  return cal;
}
