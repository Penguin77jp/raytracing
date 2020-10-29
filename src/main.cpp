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

int main(int, char**) {
  png::GUI gui;
  constexpr int size = 100;

  //scene setting
  {
    png::Scene& scene = Singleton<png::G_Data>::singleton().scene;

    //scene.list.emplace_back(std::make_unique<png::Triangle>(png::Triangle{ png::vec3{+1.0f,+0.0f,+0.0f},png::vec3{-1.0f,+0.0f,+0.0f},png::vec3{+0.0f,+1.0f,+0.0f},png::vec3{1.0f,0.3f,0.3f},png::vec3{} }));
    //scene.list.emplace_back(std::make_unique<png::Triangle>(png::Triangle{ png::vec3{+5.0f,+0.0f,+0.0f},png::vec3{+3.0f,+0.0f,+0.0f},png::vec3{+4.0f,+1.0f,+0.0f},png::vec3{0.3f,1.0f,0.3f},png::vec3{} }));



    //std::unique_ptr<png::vec3> hoge = std::make_unique<png::vec3>(1, 2, 3);
    std::unique_ptr<png::vec3> hoge(new png::vec3(0, 1, 2));


    //scene.list.emplace_back(new png::Box{ png::vec3{+1.5f,+0.0f,+0.0f},png::vec3{1.0f,0.3f,0.3f},png::vec3{} });
    scene.list.emplace_back(new png::Box{ png::vec3{+1.5f,+0.0f,+0.0f},new png::MaterialDiffuse(png::vec3{1.0f,0.0f,0.0f},0.5,0.0f) });
    scene.list.emplace_back(new png::Box{ png::vec3{-1.5f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3{1.0f,1.0f,1.0f},0.8f,0.0f) });
    scene.list.emplace_back(new png::Box{ png::vec3{+0.0f,+0.0f,-3.0f},new png::MaterialReflect(png::vec3{0.1f,1.0f,0.1f},0.8f,0.0f) });
    //scene.list.emplace_back(new png::Box{ png::vec3{-1.5f,+0.0f,+0.0f},png::vec3{0.3f,1.0f,0.3f},png::vec3{} });
    //const float emission = 2.0f;
    //scene.list.emplace_back(new png::Box{ png::vec3{-0.0f,+3.0f,+0.0f},png::vec3{1.0f,1.0f,1.0f},png::vec3{emission,emission,emission} });

    //box
    /*{
      const float side = 55;
      float col = 0.7f;
      png::vec3 color{ col,col,col };
      scene.list.emplace_back(std::make_unique<png::Box>(
        png::Box{ png::vec3{-side,+00.0f,+00.0f},color,png::vec3{},50 }));
      scene.list.emplace_back(std::make_unique<png::Box>(
        png::Box{ png::vec3{+side,+00.0f,+00.0f},color,png::vec3{},50 }));
      scene.list.emplace_back(std::make_unique<png::Box>(
        png::Box{ png::vec3{+00.0f,-side,+00.0f},color,png::vec3{},50 }));
      scene.list.emplace_back(std::make_unique<png::Box>(
        png::Box{ png::vec3{+00.0f,+side,+00.0f},color,png::vec3{},50 }));
      scene.list.emplace_back(std::make_unique<png::Box>(
        png::Box{ png::vec3{+00.0f,+00.0f,-side},color,png::vec3{},50 }));
      scene.list.emplace_back(std::make_unique<png::Box>(
        png::Box{ png::vec3{+00.0f,+00.0f,+side},color,png::vec3{},50 }));
    }*/

  }
  png::RenderTarget* p_renderTarget = new png::RenderTarget{ size, size };
  png::Renderer renderer;

  Singleton<png::G_Data>::singleton().renderTex = p_renderTarget;
  Singleton<png::G_Data>::singleton().scene.sceneLight = png::SceneLight(
    //png::vec3{1.0f,1.0f,1.0f}
    //png::Texture("private src/test_texture.jpg")
    //png::Texture("private src/601265265.835475.jpg")
    png::Texture("private src/601265293.010277.jpg")
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
