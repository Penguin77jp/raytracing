#define NOMINMAX
#include "ray.h"
#include "render.h"
#include "g_data.h"
#include "sceneData.h"
#include <iostream>
#include "embree3/rtcore.h"
#include <array>
#include <vector>
#include <numeric>
#include <random>
#include <numbers>
#include <cmath>
#include <iostream>

struct Vertex {
  float x, y, z;
};

struct PolygonIndex {
  unsigned int v0, v1, v2;
};

png::Renderer::Renderer() {
  deviceHandle = rtcNewDevice(nullptr);
  sceneHandle = rtcNewScene(deviceHandle);
  RTCGeometry geometryHandle = rtcNewGeometry(deviceHandle, RTC_GEOMETRY_TYPE_TRIANGLE);
//  scene.list.push_back(Triangle{ vec3{+1.5f,+2.0f,+1.5f},vec3{-1.0f,+2.0f,+0.0f},vec3{+1.5f,+2.0f,+0.0f},
//  vec3{1.0f,0.3f,0.3f},vec3{1,1,1} });
//  scene.list.push_back(Triangle{ vec3{+1.5f,-2.0f,+1.5f},vec3{+1.5f,-2.0f,+0.0f},vec3{-1.0f,-2.0f,+0.0f},
//vec3{0.3f,1.0f,0.3f},vec3{} });
  scene.list.push_back(Box{ vec3{+1.5f,+0.0f,+0.0f},vec3{1.0f,0.3f,0.3f},vec3{} });
  scene.list.push_back(Box{ vec3{-1.5f,+0.0f,+0.0f},vec3{0.3f,1.0f,0.3f},vec3{} });
  scene.list.push_back(Box{ vec3{-0.0f,+3.0f,+0.0f},vec3{1.0f,1.0f,1.0f},vec3{2.0f,2.0f,2.0f} });

  //box
  {
    const float side = 60;
    float col = 1.0f;
    vec3 color{col,col,col};
    scene.list.push_back(Box{ vec3{-side,+00.0f,+00.0f},color,vec3{},50 });
    scene.list.push_back(Box{ vec3{side ,+00.0f,+00.0f},color,vec3{},50 });
    scene.list.push_back(Box{ vec3{0.0f ,-side ,+00.0f},color,vec3{},50 });
    scene.list.push_back(Box{ vec3{0.0f ,side  ,+00.0f},color,vec3{},50 });
    scene.list.push_back(Box{ vec3{0.0f ,0.0f  ,-side },color,vec3{},50 });
    scene.list.push_back(Box{ vec3{0.0f ,0.0f  ,+side },color,vec3{},50 });
  }


  std::vector<vec3> vec_geometoryList;
  std::vector<std::vector<unsigned int>> vec_polygonIndex;
  scene.GetLise(vec_geometoryList, vec_polygonIndex);

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

inline double random_double() {
  static std::uniform_real_distribution<double> distribution(0.0, 1.0);
  static std::mt19937 generator;
  return distribution(generator);
}

inline double random_double(double min, double max) {
  // Returns a random real in [min,max).
  return min + (max - min) * random_double();
}

png::vec3 png::Renderer::Hit(RTCRayHit& rayhit, int depth) {
  constexpr int kDepth = 5; // ロシアンルーレットで打ち切らない最大深度
  constexpr int kDepthLimit = 64;

  if (depth > 6) {
    return png::vec3{};
  }
  rtcIntersect1(sceneHandle, &context, &rayhit);

  //no hit
  if (rayhit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
    return png::vec3{ };
  }

  //next
  RTCRayHit newRayhit;
  png::vec3 hitPoint = vec3{ rayhit.ray.org_x,rayhit.ray.org_y,rayhit.ray.org_z } + vec3::Normalize(vec3{ rayhit.ray.dir_x,rayhit.ray.dir_y, rayhit.ray.dir_z })*rayhit.ray.tfar;
  newRayhit.ray.org_x = hitPoint.x;
  newRayhit.ray.org_y = hitPoint.y;
  newRayhit.ray.org_z = hitPoint.z;

  vec3 w, u, v;
  w = vec3::Normalize(vec3{ rayhit.hit.Ng_x,rayhit.hit.Ng_y,rayhit.hit.Ng_z });
  constexpr float kEPS = 1e-6;
  if (fabs(w.x) > kEPS) // ベクトルwと直交するベクトルを作る。w.xが0に近い場合とそうでない場合とで使うベクトルを変える。
    u = vec3::Normalize(vec3::Cross(vec3(0.0, 1.0, 0.0), w));
  else
    u = vec3::Normalize(vec3::Cross(vec3(1.0, 0.0, 0.0), w));
  v = vec3::Cross(w, u);
  // コサイン項を使った重点的サンプリング
  const double r1 = 2 * std::numbers::pi * random_double();
  const double r2 = random_double(), r2s = sqrt(r2);
  vec3 dir = vec3::Normalize((
    u * cos(r1) * r2s +
    v * sin(r1) * r2s +
    w * sqrt(1.0 - r2)));

  //hoge
  vec3 hogeDir = vec3::Normalize(vec3{ rayhit.ray.dir_x ,rayhit.ray.dir_y,rayhit.ray.dir_z });
  vec3 hogehoge = hogeDir - w * 2.0 * vec3::Dot(hogeDir,w);

  //dir = vec3::Normalize(dir + hogehoge*2.0f);

  newRayhit.ray.dir_x = dir.x;
  newRayhit.ray.dir_y = dir.y;
  newRayhit.ray.dir_z = dir.z;
  newRayhit.ray.tnear = 0.0f;
  constexpr float FLOAT_INFINITY = std::numeric_limits<float>::max();
  newRayhit.ray.tfar = FLOAT_INFINITY;
  newRayhit.ray.flags = false;
  newRayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

  vec3 incoming_radiance = Hit(newRayhit, depth + 1);
  vec3 normal_n = vec3::Normalize(vec3{ rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z });
  vec3 objColor = scene.GetColor(rayhit.hit.primID);
  double russian_roulette_probability = std::max(objColor.x, std::max(objColor.y, objColor.z));
  vec3 emission = scene.GetEmissionColor(rayhit.hit.primID);
  if (depth > kDepthLimit) {
    russian_roulette_probability *= std::pow(0.5,depth-kDepthLimit);
  }
  if (depth > kDepth) {
    if (random_double() >= russian_roulette_probability) {
      return emission;
    }
  } else {
    russian_roulette_probability = 1.0;
  }
  vec3 weight = objColor / russian_roulette_probability;
  return emission + objColor *incoming_radiance;
}

png::vec3 png::Renderer::PrimalRayTracing(RTCRayHit& rayhit) {
  rtcIntersect1(sceneHandle, &context, &rayhit);

  //no hit
  if (rayhit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
    return png::vec3{ };
  }

  return scene.GetColor(rayhit.hit.primID)  * std::clamp(vec3::Dot(vec3(rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z), vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z)),0.0f,1.0f);
}

png::vec3 png::Renderer::LambertDiffuse(RTCRayHit& rayhit) {
  rtcIntersect1(sceneHandle, &context, &rayhit);

  //no hit
  if (rayhit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
    return png::vec3{ };
  }

  return scene.GetColor(rayhit.hit.primID) * std::abs(vec3::Dot(vec3::Normalize(vec3(rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z)), vec3::Normalize(vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z))));
}


void png::Renderer::Draw(std::vector<float>& image) {
  const int superSampling = 1;
  constexpr float FLOAT_INFINITY = std::numeric_limits<float>::max();
  const int screen_width = Singleton<G_Data>::singleton().renderTex->width;
  const int screen_height = Singleton<G_Data>::singleton().renderTex->height;
  const float fovx = Singleton<G_Data>::singleton().cam.fov;
  const float fovy = Singleton<G_Data>::singleton().cam.fov * screen_height / screen_width;

#pragma omp parallel for num_threads(5)
  for (int32_t y = 0; y < screen_height; ++y) {
    RTCRayHit rayhit;
    //org
    png::vec3 org = Singleton<G_Data>::singleton().cam.origin;
    rayhit.ray.org_x = org.x;
    rayhit.ray.org_y = org.y;
    rayhit.ray.org_z = org.z;
    rayhit.ray.tnear = 0.0f;
    for (int32_t x = 0; x < screen_width; ++x) {
      vec3 l_camX = -Singleton<png::G_Data>::singleton().cam.l_camX;
      vec3 l_camY = Singleton<png::G_Data>::singleton().cam.l_camY;
      vec3 l_camZ = Singleton<png::G_Data>::singleton().cam.l_camZ;
      png::vec3 color;
      for (int s = 0; s < superSampling; ++s) {
        vec3 dir = vec3::Normalize(l_camX * fovx * (2.0f * (x + random_double()) / screen_width - 1.0f) + l_camY * fovy * (2.0f * (y + random_double()) / screen_height - 1.0f) + l_camZ);
        rayhit.ray.dir_x = dir.x;
        rayhit.ray.dir_y = dir.y;
        rayhit.ray.dir_z = dir.z;

        rayhit.ray.tfar = FLOAT_INFINITY;
        rayhit.ray.flags = false;
        rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        int camType = Singleton<png::G_Data>::singleton().cam.type;
        if (camType == 0) {
          color += Hit(rayhit, 0) / superSampling;
        } else if (camType == 1) {
          color += PrimalRayTracing(rayhit) / superSampling;
        } else if (camType == 2) {
          color += LambertDiffuse(rayhit) / superSampling;
        }
      }
      const int indexY = screen_height - y - 1;

      image[x * 4 + indexY * screen_width * 4] += color.x;
      image[x * 4 + indexY * screen_width * 4 + 1] += color.y;
      image[x * 4 + indexY * screen_width * 4 + 2] += color.z;
      image[x * 4 + indexY * screen_width * 4 + 3] += 1.0f;

    }
  }
}
