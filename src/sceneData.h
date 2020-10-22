#pragma once

#include "ray.h"
#include <vector>
#include <array>
#include <memory>

namespace png {
  class SceneObject {
  public:
    vec3 color;
    vec3 emissionColor;

    SceneObject(vec3 color, vec3 emissionColor) :color(color), emissionColor(emissionColor) {}
    SceneObject(SceneObject& obj) {
      this->color = obj.color;
      this->emissionColor = obj.emissionColor;
    }
    SceneObject() : color(vec3{}), emissionColor(vec3{}) {}
    ~SceneObject() = default;
    virtual unsigned int NumberVertex() = 0;
    virtual void AddVertex(std::vector<vec3>& geometryList, std::vector<std::vector<unsigned int>>& polygonIndex) = 0;
  };

  class Triangle : public SceneObject {
  private:
  public:
    vec3 v1, v2, v3;

    Triangle(vec3 v1, vec3 v2, vec3 v3, vec3 color, vec3 emissionColor) :v1(v1), v2(v2), v3(v3), SceneObject(color, emissionColor) {}
    Triangle(const Triangle& a) {
      this->v1 = a.v1;
      this->v2 = a.v2;
      this->v3 = a.v3;
      this->color = a.color;
      this->emissionColor = a.emissionColor;
    }

    unsigned int NumberVertex() override {
      return 3;
    }
    void AddVertex(std::vector<vec3>& geometryList, std::vector<std::vector<unsigned int>>& polygonIndex) override {
      geometryList.push_back(v1);
      geometryList.push_back(v2);
      geometryList.push_back(v3);
      polygonIndex.push_back(std::vector<unsigned int>{0, 1, 2});
    }
  };

  class Box : public SceneObject {
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
    { 0,1,2 }, //front
{ 3,2,1 },
      {4,6,5}, //back
      {5,6,7},
      {5,7,3}, //right
      {3,1,5},
      {0,2,4}, //left
      {2,6,4},
      {5,1,0}, //up
      {5,0,4},
      {2,3,7}, //buttom
      {2,7,6},
  } };
  public:
    Box(vec3 offset, vec3 color, vec3 emissionColor, float size = 1.0f) :offset(offset), SceneObject(color, emissionColor), size(size) {}
    Box(const Box& a) {
      this->offset = a.offset;
      this->size = a.size;
      this->color = a.color;
      this->emissionColor = a.emissionColor;
    }
    vec3 offset;
    float size;

    unsigned int NumberVertex() {
      return 12;
    }
    void AddVertex(std::vector<vec3>& geometryList, std::vector<std::vector<unsigned int>>& polygonIndex) {
      for (int i = 0; i < 8; ++i) {
        geometryList.push_back(Kgeometory[i] * size + offset);
      }
      for (int i = 0; i < 12; ++i) {
        polygonIndex.push_back(std::vector<unsigned int>{KpolygonIndex[i][0], KpolygonIndex[i][1], KpolygonIndex[i][2]});
      }
    }
  };

  class Scene {
  private:
    int polygonCounter;
  public:
    Scene() : polygonCounter(0) {}

    std::vector<std::unique_ptr<SceneObject>> list;

    void GetVertex(std::vector<vec3>& geometryList, std::vector<std::vector<unsigned int>>& polygonIndex) {
      int counterIndex = 0;
      for (int i = 0; i < list.size(); ++i) {
        std::vector<std::vector<unsigned int>> tmp_polygonIndex;
        list[i]->AddVertex(geometryList, tmp_polygonIndex);
        for (int l = 0; l < tmp_polygonIndex.size(); ++l) {
          polygonIndex.push_back(std::vector<unsigned int>{ tmp_polygonIndex[l][0] + counterIndex, tmp_polygonIndex[l][1] + counterIndex, tmp_polygonIndex[l][2] + counterIndex});
        }
        counterIndex += list[i]->NumberVertex();
      }
    }

    //primitive IDからオブジェクトIDに変更します
    int GetP2O(unsigned int index) {
      unsigned int tmp_counter = 0;
      for (int i = 0; i < list.size(); ++i) {
        if (tmp_counter >= index) {
          return i;
        }
        tmp_counter += list[i]->NumberVertex();
      }
      return -1;
    }
    vec3 GetColor(int index) {
      return list[GetP2O(index)]->color;
    }
    vec3 GetEmissionColor(int index) {
      return list[GetP2O(index)]->emissionColor;
    }
  };
}
