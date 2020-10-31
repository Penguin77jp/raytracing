#include "ray.h"
#include "gui.h"
#include "g_data.h"
#include "render.h"
#include "random.h"
#include "texture.h"
#include "sceneLight.h"
#include "sceneData.h"
#include "material.h"
#include <iostream>

void Init_CornellBox(std::vector<std::shared_ptr<png::SceneObject>>& data) {
  data.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{-2.0f,+0.0f,+0.0f},new png::MaterialDiffuse(png::vec3{1.0f,1.0f,1.0f},0.8f,0.0f) }));
  data.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{+2.0f,+0.0f,+0.0f},new png::MaterialDiffuse(png::vec3{1.0f,0.2f,0.2f},0.8f,0.0f) }));
  const float emission = 1.0f;
  data.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3{+0.0f,+3.0f,+0.0f},new png::MaterialDiffuse(png::vec3{1.0f,1.0f,1.0f},0.0f,emission) }));

  //box
  {
    const float side = 55;
    png::MaterialDiffuse* tmp_mat = new png::MaterialDiffuse(png::vec3{ 1.0f,1.0f,1.0f }, 0.5f, 0.0f);
    data.emplace_back(std::make_shared<png::Box>(
      png::Box{ png::vec3{-side,+00.0f,+00.0f}, tmp_mat ,50.0f }));
    data.emplace_back(std::make_shared<png::Box>(
      png::Box{ png::vec3{+side,+00.0f,+00.0f}, tmp_mat ,50.0f }));
    data.emplace_back(std::make_shared<png::Box>(
      png::Box{ png::vec3{+00.0f,-side,+00.0f}, tmp_mat ,50.0f }));
    data.emplace_back(std::make_shared<png::Box>(
      png::Box{ png::vec3{+00.0f,+side,+00.0f}, tmp_mat ,50.0f }));
    data.emplace_back(std::make_shared<png::Box>(
      png::Box{ png::vec3{+00.0f,+00.0f,-side}, tmp_mat ,50.0f }));
    data.emplace_back(std::make_shared<png::Box>(
      png::Box{ png::vec3{+00.0f,+00.0f,+side}, tmp_mat ,50.0f }));
  }
}

int main(int, char**) {
  png::GUI gui;
  constexpr int size = 100;

  //scene setting
  png::Scene& scene = Singleton<png::G_Data>::singleton().scene;
  Init_CornellBox(scene.list);

  png::RenderTarget* p_renderTarget = new png::RenderTarget{ size, size };
  png::Renderer renderer;

  Singleton<png::G_Data>::singleton().renderTex = p_renderTarget;
  Singleton<png::G_Data>::singleton().scene.sceneLight = png::SceneLight(
    png::vec3{ 0.1f,0.1f,0.1f }
    //png::vec3{ 1.0f,1.0f,1.0f }
    //png::Texture("private src/test_texture.jpg")
    //png::Texture("private src/601265265.835475.jpg")
    //png::Texture("private src/601265293.010277.jpg")
  );
  Singleton<png::G_Data>::singleton().cam = png::Camera{
    0,
    png::vec3{+0.0f,+2.0f,-5.0f},
    png::vec3{+0.0f,+0.0f,+0.0f},
    1.4f,
    png::vec3{+0.0f,+1.0f,+0.0f},
  };

  while (!gui.IsClose()) {
    renderer.Draw((*p_renderTarget).image_dataF);
    p_renderTarget->Update();
    gui.Update();
  }

  return 0;
}
