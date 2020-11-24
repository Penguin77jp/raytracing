#include "ray.h"
#include "gui.h"
#include "random.h"
#include "texture.h"
#include "material.h"
#include "renderData.h"
#include "camera.h"
#include <iostream>

//void Init_CornellBox(std::vector<std::shared_ptr<png::SceneObject>>& data) {
//  data.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{-2.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3{1.0f,1.0f,1.0f},0.8f,0.0f) }));
//  data.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{+2.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3{1.0f,0.2f,0.2f},0.8f,0.0f) }));
//  const float emission = 1.0f;
//  data.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{+0.0f,+3.0f,+0.0f},new png::MaterialReflect(png::vec3{1.0f,1.0f,1.0f},0.0f,emission) }));
//
//  //box
//  {
//    const float side = 55;
//    png::MaterialReflect* tmp_mat = new png::MaterialReflect(png::vec3{ 1.0f,1.0f,1.0f }, 0.5f, 0.0f);
//    data.emplace_back(std::make_shared<png::Box>(
//      png::Box{ png::vec3{-side,+00.0f,+00.0f}, tmp_mat ,50.0f }));
//    data.emplace_back(std::make_shared<png::Box>(
//      png::Box{ png::vec3{+side,+00.0f,+00.0f}, tmp_mat ,50.0f }));
//    data.emplace_back(std::make_shared<png::Box>(
//      png::Box{ png::vec3{+00.0f,-side,+00.0f}, tmp_mat ,50.0f }));
//    data.emplace_back(std::make_shared<png::Box>(
//      png::Box{ png::vec3{+00.0f,+side,+00.0f}, tmp_mat ,50.0f }));
//    data.emplace_back(std::make_shared<png::Box>(
//      png::Box{ png::vec3{+00.0f,+00.0f,-side}, tmp_mat ,50.0f }));
//    data.emplace_back(std::make_shared<png::Box>(
//      png::Box{ png::vec3{+00.0f,+00.0f,+side}, tmp_mat ,50.0f }));
//  }
//
//  Singleton<png::G_Data>::singleton().scene.sceneLight = png::SceneLight();
//}

//void Init_SkyBox(std::vector<std::shared_ptr<png::SceneObject>>& data) {
//  data.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{-2.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3{1.0f,1.0f,1.0f},0.8f,0.0f) }));
//  data.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{+2.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3{0.2f,1.0f,0.2f},0.8f,0.0f) }));
//  data.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{+0.0f,-7.5f,+0.0f},new png::MaterialReflect(png::vec3{1.0f,1.0f,1.0f},0.2f,0.0f),6.0f }));
//
//  Singleton<png::G_Data>::singleton().scene.sceneLight = png::SceneLight(
//    new png::Texture("private src/601265265.835475.jpg")
//    //new png::Texture("private src/601265293.010277.jpg")
//  );
//}
//
//void Init_SkyBox2(std::vector<std::shared_ptr<png::SceneObject>>& data) {
//  data.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{-2.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3{1.0f,0.2f,0.2f},0.8f,0.0f) }));
//  data.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{+2.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3{1.0f,1.0f,1.0f},0.2f,0.0f) }));
//
//  Singleton<png::G_Data>::singleton().scene.sceneLight = png::SceneLight(
//    //new png::Texture("private src/601265265.835475.jpg")
//    new png::Texture("private src/601265293.010277.jpg")
//  );
//}

void Init_SkyBoxOnly(png::Scene& scene) {
  scene.list.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{+0.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3{0.1f,0.1f,1.0f},0.5f,0.0f) }));
  scene.sceneLight = png::SceneLight(
    new png::Texture("private src/601265265.835475.jpg")
    //new png::Texture("private src/601265293.010277.jpg")
  );
}

void Init_NiceScene(png::Scene& scene) {
  scene.list.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{+0.0f,+0.0f,+5.0f},new png::MaterialReflect(png::vec3{1.0f,0.1f,0.1f},0.7f,0.0f) }));
  scene.list.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{+5.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3{0.1f,1.0f,0.1f},0.7f,0.0f) }));
  scene.list.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{-5.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3{0.1f,0.1f,1.0f},0.7f,0.0f) }));
  scene.list.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{+0.0f,+0.0f,-5.0f},new png::MaterialReflect(png::vec3{1.0f,1.0f,0.1f},0.7f,0.0f) }));

  //box
  {
    png::Material* tmp_mat = new png::MaterialReflect(png::vec3{ 1.0f,1.0f,1.0f }, 0.05f, 0.0f);
    scene.list.emplace_back(std::make_shared<png::Box>(
      png::Box{ png::vec3{+00.0f,-25,+00.0f}, tmp_mat ,25.0f }));
  }

  scene.sceneLight = png::SceneLight(
    new png::Texture("private src/601265265.835475.jpg")
    //new png::Texture("private src/601265293.010277.jpg")
  );
}

int main(int, char**) {
  constexpr int size = 100;

  //scene setting
  png::Scene scene;
  Init_NiceScene(scene);
  png::CameraLens* camLens = new png::PinHole();
  png::RenderData renderData(
    size,
    png::Camera{
      size,size,
      png::vec3{ -2.6f,+0.2f,+2.6f },
      png::vec3{ +0.0f,+0.0f,+0.0f },
      0,
      //camLens,
      0.726f,
    },
    scene
    );

  png::GUI gui(renderData);

  while (!gui.IsClose()) {
    renderData.renderer->Draw();
    renderData.renderTex->Update();
    gui.Update();
  }

  return 0;
}
