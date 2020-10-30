#define NOMINMAX
#include "ray.h"
#include "render.h"
#include "g_data.h"
#include "sceneData.h"
#include "random.h"
#include "embree3/rtcore.h"
#include <array>
#include <vector>
#include <numeric>
#include <random>
#include <numbers>
#include <cmath>
#include <memory>
#include <time.h>

constexpr float FLOAT_INFINITY = std::numeric_limits<float>::max();


struct Vertex {
  float x, y, z;
};

struct PolygonIndex {
  unsigned int v0, v1, v2;
};

png::Renderer::Renderer() : scene(Singleton<png::G_Data>::singleton().scene) {
  deviceHandle = rtcNewDevice(nullptr);
  sceneHandle = rtcNewScene(deviceHandle);
  RTCGeometry geometryHandle = rtcNewGeometry(deviceHandle, RTC_GEOMETRY_TYPE_TRIANGLE);

  std::vector<vec3> vec_geometoryList;
  std::vector<std::vector<unsigned int>> vec_polygonIndex;
  scene.GetVertex(vec_geometoryList, vec_polygonIndex);

  std::vector<Vertex> geometryVertices;
  std::vector<PolygonIndex> geometryPolygons;

  for (int i = 0; i < vec_geometoryList.size(); ++i) {
    geometryVertices.push_back({ vec_geometoryList[i].x,vec_geometoryList[i].y,vec_geometoryList[i].z });
  }

  rtcSetSharedGeometryBuffer(geometryHandle,
                             RTC_BUFFER_TYPE_VERTEX,
                             0,
                             RTC_FORMAT_FLOAT3,
                             &geometryVertices[0],
                             0,
                             sizeof(Vertex),
                             geometryVertices.size());




  for (int i = 0; i < vec_polygonIndex.size(); ++i) {
    geometryPolygons.push_back({ vec_polygonIndex[i][0],vec_polygonIndex[i][1],vec_polygonIndex[i][2] });
  }

  rtcSetSharedGeometryBuffer(geometryHandle,
                             RTC_BUFFER_TYPE_INDEX,
                             0,
                             RTC_FORMAT_UINT3,
                             &geometryPolygons[0],
                             0,
                             sizeof(PolygonIndex),
                             geometryPolygons.size());

  rtcCommitGeometry(geometryHandle);
  rtcAttachGeometry(sceneHandle, geometryHandle);
  rtcReleaseGeometry(geometryHandle);
  rtcCommitScene(sceneHandle);

  rtcInitIntersectContext(&context);
}



png::Renderer::~Renderer() {
  rtcReleaseScene(sceneHandle);
  rtcReleaseDevice(deviceHandle);
}


png::vec3 png::Renderer::PathTracing(RTCRayHit& rayhit, int depth, Random& rnd) {
  constexpr int kDepth = 5; // ロシアンルーレットで打ち切らない最大深度
  constexpr int kDepthLimit = 64;

  rtcIntersect1(sceneHandle, &context, &rayhit);

  //no hit
  if (rayhit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
    return Singleton<png::G_Data>::singleton().scene.sceneLight.GetColor(vec3{ rayhit.ray.dir_x,rayhit.ray.dir_y ,rayhit.ray.dir_z }) *1.0f;
  }

  //next
  RTCRayHit newRayhit;
  png::vec3 hitPoint = vec3{ rayhit.ray.org_x,rayhit.ray.org_y,rayhit.ray.org_z } + vec3::Normalize(vec3{ rayhit.ray.dir_x,rayhit.ray.dir_y, rayhit.ray.dir_z }) * rayhit.ray.tfar;
  vec3 normal_n = vec3::Normalize(vec3{ rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z });
  vec3 objColor = scene.GetMaterial(rayhit.hit.primID)->GetColor();
  if (objColor.x > 0.8f && objColor.y < 0.1f || objColor.z < 0.1f) {
    //std::cout << "hoge";
  }
  newRayhit.ray.org_x = hitPoint.x;
  newRayhit.ray.org_y = hitPoint.y;
  newRayhit.ray.org_z = hitPoint.z;

  vec3 dir = scene.GetMaterial(rayhit.hit.primID)->GetDirection(
    vec3::Normalize(vec3{ rayhit.ray.dir_x,rayhit.ray.dir_y, rayhit.ray.dir_z }),
    normal_n,
    rnd
  );

  newRayhit.ray.dir_x = dir.x;
  newRayhit.ray.dir_y = dir.y;
  newRayhit.ray.dir_z = dir.z;
  newRayhit.ray.tnear = 0.0f;
  newRayhit.ray.tfar = FLOAT_INFINITY;
  newRayhit.ray.flags = false;
  newRayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

  double russian_roulette_probability = std::max(objColor.x, std::max(objColor.y, objColor.z));
  if (depth > kDepthLimit) {
    russian_roulette_probability *= std::pow(0.5, depth - kDepthLimit);
  }
  if (depth > kDepth) {
    if (rnd.next01() >= russian_roulette_probability) {
      return scene.GetMaterial(rayhit.hit.primID)->GetEmission();
    }
  } else {
    russian_roulette_probability = 1.0;
  }
  vec3 weight = objColor / russian_roulette_probability;
  vec3 incoming_radiance = PathTracing(newRayhit, depth + 1, rnd);
  return scene.GetMaterial(rayhit.hit.primID)->GetEmission() + weight * incoming_radiance;
}

png::vec3 png::Renderer::PrimalRayTracing(RTCRayHit& rayhit) {
  rtcIntersect1(sceneHandle, &context, &rayhit);

  //no hit
  if (rayhit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
    return png::vec3{ };
  }

  return scene.GetMaterial(rayhit.hit.primID)->GetColor() * std::clamp(vec3::Dot(vec3(rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z), vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z)), 0.0f, 1.0f);
}

png::vec3 png::Renderer::LambertDiffuse(RTCRayHit& rayhit) {
  rtcIntersect1(sceneHandle, &context, &rayhit);

  //no hit
  if (rayhit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
    return Singleton<png::G_Data>::singleton().scene.sceneLight.GetColor(vec3{ rayhit.ray.dir_x,rayhit.ray.dir_y ,rayhit.ray.dir_z });
  }

  return scene.GetMaterial(rayhit.hit.primID)->GetColor() * std::abs(vec3::Dot(vec3::Normalize(vec3(rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z)), vec3::Normalize(vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z))));
}


void png::Renderer::Draw(std::vector<float>& image) {
  const unsigned int superSampling = Singleton<G_Data>::singleton().renderTex->superSampling;
  constexpr float FLOAT_INFINITY = std::numeric_limits<float>::max();
  const int screen_width = Singleton<G_Data>::singleton().renderTex->width;
  const int screen_height = Singleton<G_Data>::singleton().renderTex->height;
  const float fovx = Singleton<G_Data>::singleton().cam.fov;
  const float fovy = Singleton<G_Data>::singleton().cam.fov * screen_height / screen_width;

  if (!Singleton<G_Data>::singleton().IsRender) {
    return;
  }

  #pragma omp parallel for num_threads(6)
  for (int32_t y = 0; y < screen_height; ++y) {
    RTCRayHit rayhit;
    //org
    png::vec3 org = Singleton<G_Data>::singleton().cam.origin;
    rayhit.ray.org_x = org.x;
    rayhit.ray.org_y = org.y;
    rayhit.ray.org_z = org.z;
    rayhit.ray.tnear = 0.0f;
    using singleton = Singleton<png::G_Data>;
    Random rnd{ (unsigned int)((y + 1) * singleton::singleton().renderTex->width * singleton::singleton().renderTex->height * singleton::singleton().renderTex->sampleCounter) };
    for (int32_t x = 0; x < screen_width; ++x) {
      vec3 l_camX = -Singleton<png::G_Data>::singleton().cam.l_camX;
      vec3 l_camY = Singleton<png::G_Data>::singleton().cam.l_camY;
      vec3 l_camZ = Singleton<png::G_Data>::singleton().cam.l_camZ;
      png::vec3 color;
      for (int s = 0; s < superSampling; ++s) {
        vec3 dir = vec3::Normalize(l_camX * fovx * (2.0f * (x + rnd.next01()) / screen_width - 1.0f) + l_camY * fovy * (2.0f * (y + rnd.next01()) / screen_height - 1.0f) + l_camZ);
        rayhit.ray.dir_x = dir.x;
        rayhit.ray.dir_y = dir.y;
        rayhit.ray.dir_z = dir.z;

        rayhit.ray.tfar = FLOAT_INFINITY;
        rayhit.ray.flags = false;
        rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        int camType = Singleton<png::G_Data>::singleton().cam.type;
        if (camType == 0) {
          color += PathTracing(rayhit, 0, rnd) / superSampling;
        } else if (camType == 1) {
          color += PrimalRayTracing(rayhit) / superSampling;
        } else if (camType == 2) {
          color += LambertDiffuse(rayhit) / superSampling;
        }
      }
      const int indexY = screen_height - y - 1;

      const int index = x * 4 + indexY * screen_width * 4;
      image[index] += color.x;
      image[index + 1] += color.y;
      image[index + 2] += color.z;
      image[index + 3] += 1.0f;
    }
  }
}
