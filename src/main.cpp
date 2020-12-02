#include "InitOpenGL.h"
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
  png::SceneLight light = png::SceneLight(
    //new png::Texture("private src/601265265.835475.jpg")
    new png::Texture("private src/601265442.217750.jpg")
    //new png::Texture("private src/test.jpg")
    //new png::Texture("private src/601265293.010277.jpg")
  );
  std::vector<std::shared_ptr<png::SceneObject>> list;
  list.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{+0.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3{0.1f,0.1f,1.0f},0.5f,0.0f),0.0f }));
  scene = png::Scene(list, light);
}

void Init_NiceScene(png::Scene& scene) {
  std::vector<std::shared_ptr<png::SceneObject>> list;
  list.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{+0.0f,+0.0f,+5.0f},new png::MaterialReflect(png::vec3{1.0f,0.1f,0.1f},0.7f,0.0f) }));
  list.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{+5.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3{0.1f,1.0f,0.1f},0.7f,0.0f) }));
  list.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{-5.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3{0.1f,0.1f,1.0f},0.7f,0.0f) }));
  list.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{+0.0f,+0.0f,-5.0f},new png::MaterialReflect(png::vec3{1.0f,1.0f,0.1f},0.7f,0.0f) }));

  //box
  {
    png::Material* tmp_mat = new png::MaterialReflect(png::vec3{ 1.0f,1.0f,1.0f }, 0.05f, 0.0f);
    list.emplace_back(std::make_shared<png::Box>(
      png::Box{ png::vec3{+00.0f,-25,+00.0f}, tmp_mat ,25.0f }));
  }
  scene.SetSceneList(list);

  png::SceneLight sceneLight = png::SceneLight(
    new png::Texture("private src/601265265.835475.jpg")
    //new png::Texture("private src/601265293.010277.jpg")
  );
  scene.SetSceneLight(sceneLight);
}

void Init_DOF(png::Scene& scene) {
  std::vector<std::shared_ptr<png::SceneObject>> list;
  list.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{+0.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3{1.0f,1.0f,1.0f},0.0f,1.0f),0.3 }));
  scene.SetSceneList(list);

  png::SceneLight sceneLight = png::SceneLight();
  scene.SetSceneLight(sceneLight);
}

int main(int, char**) {
  InitOpenGL_Loader();
  constexpr int size = 100;

  //scene setting
  png::Scene scene;
  Init_NiceScene(scene);
  //Init_DOF(scene);
  std::shared_ptr<png::RenderTarget> renderTarget(std::make_shared<png::RenderTarget>(size, size));
  png::ThinLens cam(
    png::vec3{ -7.1f,+0.8f,+1.7f },
    png::vec3{ +0.0f,+0.0f,+0.0f },
    0,
    0.4,
    new png::AperturePolygonBlue(),
    0.1f,
    1.9f
  );
  png::Renderer renderer(&cam,renderTarget, scene);

  png::GUI gui(renderer);

  while (!gui.IsClose()) {
    //renderData.renderer->Draw();
    //renderData.renderTex->Update();
    gui.Update(renderTarget);
  }

  return 0;
}
