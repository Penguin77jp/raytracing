#include "ray.h"
#include "gui.h"
#include "g_data.h"
#include "render.h"

int main(int, char**) {
  png::GUI gui;
  png::Texture* p_renderTex = new png::Texture{ 500,500 };
  png::Renderer renderer;

  Singleton<png::G_Data>::singleton().renderTex = p_renderTex;
  Singleton<png::G_Data>::singleton().cam = png::Camera{
    png::vec3{0.0f,+10.0f,-5.0f},
    png::vec3{0.0f,0.0f,0.0f},
    0.2f,
    png::vec3{0.0f,1.0f,0.0f}
  };

  while (!gui.IsClose()) {
    renderer.Draw((*p_renderTex).image_data, (*p_renderTex).width, (*p_renderTex).height);
    p_renderTex->Update();
    gui.Update();
  }

  return 0;
}
