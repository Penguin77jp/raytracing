#pragma once

#include <vector>
#include <array>
#include <memory>
#include "ray.h"
#include "sceneLight.h"
#include "material.h"

namespace png {
  class SceneObject {
  public:
    Material* material;

    SceneObject(Material* mat) :material(mat) {}
    SceneObject(SceneObject& obj) {
      this->material = obj.material;
    }
    SceneObject() : material(nullptr) {}
    ~SceneObject() = default;
    virtual unsigned int vertexNum() = 0;
    virtual unsigned int primitiveNum() = 0;
    virtual void AddVertex(std::vector<vec3>& geometryList, std::vector<std::vector<unsigned int>>& polygonIndex) = 0;
  };

  class Triangle : public SceneObject {
  private:
  public:
    vec3 v1, v2, v3;

    Triangle(vec3 v1, vec3 v2, vec3 v3, Material* mat) :v1(v1), v2(v2), v3(v3), SceneObject(mat) {}
    Triangle(const Triangle& a) {
      this->v1 = a.v1;
      this->v2 = a.v2;
      this->v3 = a.v3;
      this->material = material;
    }

    unsigned int vertexNum() override {
      return 2;
    }
    unsigned int primitiveNum() override {
      return 1;
    }

    void AddVertex(std::vector<vec3>& geometryList, std::vector<std::vector<unsigned int>>& polygonIndex) override {
      geometryList.push_back(v1);
      geometryList.push_back(v2);
      geometryList.push_back(v3);
      polygonIndex.push_back(std::vector<unsigned int>{0, 1, 2});
    }
  };

  class Plane : public SceneObject {
  private:
  public:
    vec3 v1, v2, v3, v4;

    Plane(vec3 v1, vec3 v2, vec3 v3, vec3 v4, Material* mat) :v1(v1), v2(v2), v3(v3), v4(v4), SceneObject(mat) {}
    Plane(vec3 pos, Material* mat, float size = 1.0f) : SceneObject(mat) {
      v1 = pos + vec3{ -size,+size,0 };
      v2 = pos + vec3{ +size,+size,0 };
      v3 = pos + vec3{ -size,-size,0 };
      v4 = pos + vec3{ +size,-size,0 };
    }
    Plane(const Plane& a) {
      this->v1 = a.v1;
      this->v2 = a.v2;
      this->v3 = a.v3;
      this->v4 = a.v4;
      this->material = a.material;
    }

    unsigned int vertexNum() override {
      return 4;
    }
    unsigned int primitiveNum() override {
      return 2;
    }
    void AddVertex(std::vector<vec3>& geometryList, std::vector<std::vector<unsigned int>>& polygonIndex) override {
      geometryList.push_back(v1);
      geometryList.push_back(v2);
      geometryList.push_back(v3);
      geometryList.push_back(v4);
      polygonIndex.push_back(std::vector<unsigned int>{0, 1, 2});
      polygonIndex.push_back(std::vector<unsigned int>{1, 2, 3});
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
    Box(vec3 offset, Material* mat, float size = 1.0f) :offset(offset), SceneObject(mat), size(size) {}
    Box(const Box& a) {
      this->offset = a.offset;
      this->size = a.size;
      this->material = a.material;
    }
    vec3 offset;
    float size;

    unsigned int vertexNum() override {
      return 8;
    }
    unsigned int primitiveNum() override {
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
    std::vector<std::shared_ptr<SceneObject>> list;
    SceneLight sceneLight;
    Scene() : polygonCounter(0) {}

    void GetVertex(std::vector<vec3>& geometryList, std::vector<std::vector<unsigned int>>& polygonIndex) {
      int counterIndex = 0;
      for (int i = 0; i < list.size(); ++i) {
        std::vector<std::vector<unsigned int>> tmp_polygonIndex;
        list[i]->AddVertex(geometryList, tmp_polygonIndex);
        for (int l = 0; l < tmp_polygonIndex.size(); ++l) {
          polygonIndex.push_back(std::vector<unsigned int>{ tmp_polygonIndex[l][0] + counterIndex, tmp_polygonIndex[l][1] + counterIndex, tmp_polygonIndex[l][2] + counterIndex});
        }
        counterIndex += list[i]->vertexNum();
      }
    }

    //primitive IDからオブジェクトIDに変更します
    int GetP2O(unsigned int index) {
      unsigned int tmp_counter = 0;
      for (int i = 0; i < list.size(); ++i) {
        tmp_counter += list[i]->primitiveNum();
        if (tmp_counter > index) {
          return i;
        }
      }
      return -1;
    }
    Material* GetMaterial(int index) {
      return list[GetP2O(index)]->material;
    }
  };
}
