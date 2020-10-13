#pragma once

#include "ray.h"
#include <vector>
#include <array>

namespace png {
  class Triangle {
  private:
  public:
    Triangle(vec3 v1,vec3 v2, vec3 v3, vec3 color, vec3 emissionColor) :v1(v1),v2(v2),v3(v3), color(color), emissionColor(emissionColor) {}
    Triangle(const Triangle& a) {
      this->v1 = a.v1;
      this->v2 = a.v2;
      this->v3 = a.v3;
      this->color = a.color;
      this->emissionColor = a.emissionColor;
    }
    vec3 v1, v2, v3;
    vec3 color;
    vec3 emissionColor;

    void GetLine(std::vector<vec3>& geometryList, std::vector<std::vector<unsigned int>>& polygonIndex, int& polygonCounter) {
      geometryList.push_back(v1);
      geometryList.push_back(v2);
      geometryList.push_back(v3);
      polygonIndex.push_back(std::vector<unsigned int>{0+(unsigned int)polygonCounter,1 + (unsigned int)polygonCounter,2 + (unsigned int)polygonCounter});
      polygonCounter += 3;
    }
  };

  class Box {
  private:
    const std::array<vec3, 8> Kgeometory{
      vec3{-1.0f, +1.0f, -1.0f } ,
      vec3{+1.0f, +1.0f, -1.0f },
      vec3{-1.0f, -1.0f, -1.0f },
      vec3{+1.0f, -1.0f, -1.0f},
      vec3{-1.0f, +1.0f, +1.0f},
      vec3{+1.0f, +1.0f, +1.0f },
      vec3{-1.0f, -1.0f, +1.0f},
      vec3{+1.0f, -1.0f, +1.0f},
    };
    const std::array<std::array<unsigned int, 3>, 12> KpolygonIndex{ {
        { 1,2,0 },
    { 2,1,3 },
  { 0,5,1 },
  { 0,4,5 },
  { 2,7,3 },
  { 2,6,7 },
  { 6,7,5 },
  { 6,5,4 },
  { 3,5,7 },
  { 3,1,5 },
  { 2,6,4 },
  { 0,2,4 },
    } };
  public:
    Box(vec3 offset , vec3 color , vec3 emissionColor, float size = 1.0f) :offset(offset), color(color),emissionColor(emissionColor), size(size) {}
    Box(const Box& a) {
      this->offset = a.offset;
      this->size = a.size;
      this->color = a.color;
      this->emissionColor = a.emissionColor;
    }
    vec3 offset;
    vec3 color;
    vec3 emissionColor;
    float size;

    void GetLine(std::vector<vec3>& geometryList, std::vector<std::vector<unsigned int>>& polygonIndex, int &polygonCounter) {
      for (int i = 0; i < 8; ++i) {
        geometryList.push_back(Kgeometory[i] * size + offset);
      }
      for (int i = 0; i < 12; ++i) {
        polygonIndex.push_back(std::vector<unsigned int>{polygonCounter+ KpolygonIndex[i][0], polygonCounter + KpolygonIndex[i][1], polygonCounter + KpolygonIndex[i][2]});
      }
      polygonCounter += 8;
    }
  };

  class Scene {
  private :
    int polygonCounter;
  public:
    Scene() : polygonCounter(0){}

    std::vector<Box> list;
    //std::vector<Triangle> list;

    void GetLise(std::vector<vec3>& geometryList, std::vector<std::vector<unsigned int>>& polygonIndex) {
      for (int i = 0; i < list.size(); ++i) {
        list[i].GetLine(geometryList, polygonIndex,polygonCounter);
      }
    }

    vec3 GetColor(int index) {
      //return list[(int)(index )].color;
      return list[(int)(index / 12.0f)].color;
    }
    vec3 GetEmissionColor(int index) {
      //return list[(int)index ].emissionColor;
      return list[(int)index / 12.0f].emissionColor;
    }
  };
}
