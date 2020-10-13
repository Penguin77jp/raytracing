#include "ray.h"
#include "gui.h"
#include "g_data.h"
#include "render.h"

int main(int, char**) {
  png::GUI gui;
  png::Texture* p_renderTex = new png::Texture{ 100,100 };
  png::Renderer renderer;

  Singleton<png::G_Data>::singleton().renderTex = p_renderTex;
  Singleton<png::G_Data>::singleton().cam = png::Camera{
    0,
    png::vec3{+0.0f,+5.0f,+5.0f},
    png::vec3{+0.0f,-2.5f,+0.0f},
    1.6f,
    png::vec3{0.0f,1.0f,0.0f}
  };


  while (!gui.IsClose()) {
    renderer.Draw((*p_renderTex).image_dataF);
    p_renderTex->Update();
    gui.Update();
  }

  return 0;
}
