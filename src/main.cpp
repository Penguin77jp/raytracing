#include "ray.h"
#include "gui.h"
#include "g_data.h"
#include "render.h"
#include "random.h"

void hoge() {
  png::Random rnd(1);
  double ans = 0;
  double max = 0;
  double min = 1;
  constexpr int N = 10;
  //constexpr int N = 99999999999;
  for (int i = 0; i < N; ++i) {
    //std::cout << rnd.next01() << std::endl;
    double sample = rnd.next01();
    std::cout << sample << std::endl;
    ans += sample/ N;
    if (max < sample) {
      max = sample;
    }
    if (min > sample) {
      min = sample;
    }
  }
  std::cout << ans << " , min = " << min << " , max = "<<max;
}

int main(int, char**) {
  hoge();
  //return 0;
  png::GUI gui;
  png::Texture* p_renderTex = new png::Texture{ 25,25 };
  png::Renderer renderer;

  Singleton<png::G_Data>::singleton().renderTex = p_renderTex;
  Singleton<png::G_Data>::singleton().cam = png::Camera{
    0,
    png::vec3{+0.0f,+2.0f,-5.0f},
    png::vec3{+0.0f,+0.0f,+0.0f},
    1.4f,
    png::vec3{0.0f,1.0f,0.0f}
  };


  while (!gui.IsClose()) {
    renderer.Draw((*p_renderTex).image_dataF);
    p_renderTex->Update();
    gui.Update();
  }

  return 0;
}
