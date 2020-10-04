#define NOMINMAX
#include "ray.h"
#include "render.h"
#include "g_data.h"
#include <iostream>
#include "embree3/rtcore.h"
#include <array>
#include <vector>
#include <numeric>

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

  constexpr size_t numVertices = 8;
  constexpr size_t numPolygons = 12;

  std::array<Vertex, numVertices> geometryVertices;
  std::array<PolygonIndex, numPolygons> geometryPolygons;

  rtcSetSharedGeometryBuffer(geometryHandle,
                             RTC_BUFFER_TYPE_VERTEX,
                             0,
                             RTC_FORMAT_FLOAT3,
                             &geometryVertices,
                             0,
                             sizeof(Vertex),
                             geometryVertices.size());

  geometryVertices[0] = { -0.5f, +0.5f, -0.5f };
  geometryVertices[1] = { +0.5f, +0.5f, -0.5f };
  geometryVertices[2] = { -0.5f, -0.5f, -0.5f };
  geometryVertices[3] = { +0.5f, -0.5f, -0.5f };
  geometryVertices[4] = { -0.5f, +0.5f, +0.5f };
  geometryVertices[5] = { +0.5f, +0.5f, +0.5f };
  geometryVertices[6] = { -0.5f, -0.5f, +0.5f };
  geometryVertices[7] = { +0.5f, -0.5f, +0.5f };

  rtcSetSharedGeometryBuffer(geometryHandle,
                             RTC_BUFFER_TYPE_INDEX,
                             0,
                             RTC_FORMAT_UINT3,
                             &geometryPolygons,
                             0,
                             sizeof(PolygonIndex),
                             geometryPolygons.size());

  geometryPolygons[0] = { 1,0,2 };
  geometryPolygons[1] = { 2,3,1 };
  geometryPolygons[2] = { 0,1,5 };
  geometryPolygons[3] = { 0,5,4 };
  geometryPolygons[4] = { 2,3,7 };
  geometryPolygons[5] = { 2,7,6 };
  geometryPolygons[6] = { 6,7,5 };
  geometryPolygons[7] = { 6,5,4 };
  geometryPolygons[8] = { 3,7,5 };
  geometryPolygons[9] = { 3,5,1 };
  geometryPolygons[10] = { 2,6,4 };
  geometryPolygons[11] = { 2,4,0 };

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

void png::Renderer::Draw(std::vector<unsigned char>& image, int screen_width, int screen_height) {
  constexpr float FLOAT_INFINITY = std::numeric_limits<float>::max();

  RTCRayHit rayhit;
  //org
  png::vec3 org = Singleton<G_Data>::singleton().cam.origin;
  rayhit.ray.org_x = org.x;
  rayhit.ray.org_y = org.y;
  rayhit.ray.org_z = org.z;

  rayhit.ray.tnear = 0.0f;

  const float fov = Singleton<G_Data>::singleton().cam.fov;
  for (int32_t y = 0; y < screen_height; ++y) {
    for (int32_t x = 0; x < screen_width; ++x) {
      vec3 l_camX = -Singleton<png::G_Data>::singleton().cam.l_camX;
      vec3 l_camY = Singleton<png::G_Data>::singleton().cam.l_camY;
      vec3 l_camZ = Singleton<png::G_Data>::singleton().cam.l_camZ;
      vec3 dir = vec3::Normalize(l_camX * fov * (2.0f * x / screen_width - 1.0f) + l_camY * fov * (2.0f * y / screen_height - 1.0f) + l_camZ);
      rayhit.ray.dir_x = dir.x;
      rayhit.ray.dir_y = dir.y;
      rayhit.ray.dir_z = dir.z;
      rayhit.ray.tfar = FLOAT_INFINITY;
      rayhit.ray.flags = false;
      rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
      rtcIntersect1(sceneHandle, &context, &rayhit);
      const int indexY = screen_height-y-1;
      if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
        image[x * 4 + indexY * screen_width * 4] = 255 * vec3::Dot(vec3(rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z), vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z));
        image[x * 4 + indexY * screen_width * 4 + 1] = 0;
        image[x * 4 + indexY * screen_width * 4 + 2] = 0;
        image[x * 4 + indexY * screen_width * 4 + 3] = 255;
      } else {
        image[x * 4 + indexY * screen_width * 4] = 0;
        image[x * 4 + indexY * screen_width * 4 + 1] = 0;
        image[x * 4 + indexY * screen_width * 4 + 2] = 0;
        image[x * 4 + indexY * screen_width * 4 + 3] = 255;
      }
    }
  }
}
