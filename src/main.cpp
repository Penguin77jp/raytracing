#include "InitOpenGL.h"
#include "ray.h"
#include "gui.h"
#include "random.h"
#include "texture.h"
#include "material.h"
#include "renderData.h"
#include "camera.h"
#include <iostream>

//debug
#include <chrono>
#include <random>

//void Init_CornellBox(std::vector<std::shared_ptr<png::SceneObject>>& data) {
//  data.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3<>{-2.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3<>{1.0f,1.0f,1.0f},0.8f,0.0f) }));
//  data.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3<>{+2.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3<>{1.0f,0.2f,0.2f},0.8f,0.0f) }));
//  const float emission = 1.0f;
//  data.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3<>{+0.0f,+3.0f,+0.0f},new png::MaterialReflect(png::vec3<>{1.0f,1.0f,1.0f},0.0f,emission) }));
//
//  //box
//  {
//    const float side = 55;
//    png::MaterialReflect* tmp_mat = new png::MaterialReflect(png::vec3<>{ 1.0f,1.0f,1.0f }, 0.5f, 0.0f);
//    data.emplace_back(std::make_shared<png::Box>(
//      png::Box{ png::vec3<>{-side,+00.0f,+00.0f}, tmp_mat ,50.0f }));
//    data.emplace_back(std::make_shared<png::Box>(
//      png::Box{ png::vec3<>{+side,+00.0f,+00.0f}, tmp_mat ,50.0f }));
//    data.emplace_back(std::make_shared<png::Box>(
//      png::Box{ png::vec3<>{+00.0f,-side,+00.0f}, tmp_mat ,50.0f }));
//    data.emplace_back(std::make_shared<png::Box>(
//      png::Box{ png::vec3<>{+00.0f,+side,+00.0f}, tmp_mat ,50.0f }));
//    data.emplace_back(std::make_shared<png::Box>(
//      png::Box{ png::vec3<>{+00.0f,+00.0f,-side}, tmp_mat ,50.0f }));
//    data.emplace_back(std::make_shared<png::Box>(
//      png::Box{ png::vec3<>{+00.0f,+00.0f,+side}, tmp_mat ,50.0f }));
//  }
//
//  Singleton<png::G_Data>::singleton().scene.sceneLight = png::SceneLight();
//}

//void Init_SkyBox(std::vector<std::shared_ptr<png::SceneObject>>& data) {
//  data.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3<>{-2.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3<>{1.0f,1.0f,1.0f},0.8f,0.0f) }));
//  data.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3<>{+2.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3<>{0.2f,1.0f,0.2f},0.8f,0.0f) }));
//  data.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3<>{+0.0f,-7.5f,+0.0f},new png::MaterialReflect(png::vec3<>{1.0f,1.0f,1.0f},0.2f,0.0f),6.0f }));
//
//  Singleton<png::G_Data>::singleton().scene.sceneLight = png::SceneLight(
//    new png::Texture("private src/601265265.835475.jpg")
//    //new png::Texture("private src/601265293.010277.jpg")
//  );
//}
//
//void Init_SkyBox2(std::vector<std::shared_ptr<png::SceneObject>>& data) {
//  data.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3<>{-2.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3<>{1.0f,0.2f,0.2f},0.8f,0.0f) }));
//  data.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3<>{+2.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3<>{1.0f,1.0f,1.0f},0.2f,0.0f) }));
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
  list.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3<>{+0.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3<>{0.1f,0.1f,1.0f},0.5f,0.0f),0.0f }));
  scene = png::Scene(list, light);
}

void Init_NiceScene(png::Scene& scene) {
  std::vector<std::shared_ptr<png::SceneObject>> list;
  list.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3<>{+0.0f,+0.0f,+5.0f},new png::MaterialReflect(png::vec3<>{1.0f,0.1f,0.1f},0.7f,0.0f) }));
  list.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3<>{+5.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3<>{0.1f,1.0f,0.1f},0.7f,0.0f) }));
  list.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3<>{-5.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3<>{0.1f,0.1f,1.0f},0.7f,0.0f) }));
  list.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3<>{+0.0f,+0.0f,-5.0f},new png::MaterialReflect(png::vec3<>{1.0f,1.0f,0.1f},0.7f,0.0f) }));

  //box
  {
    png::Material* tmp_mat = new png::MaterialReflect(png::vec3<>{ 1.0f, 1.0f, 1.0f }, 0.05f, 0.0f);
    list.emplace_back(std::make_shared<png::Box>(
      png::Box{ png::vec3<>{+00.0f,-25,+00.0f}, tmp_mat ,25.0f }));
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
  list.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3<>{+0.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3<>{1.0f,0.1f,0.1f},1.0f,1.0f),0.5f }));
  scene.SetSceneList(list);

  png::SceneLight sceneLight = png::SceneLight(
    new png::Texture("private src/601265265.835475.jpg")
  );
  scene.SetSceneLight(sceneLight);
}

void Init_Test(png::Scene& scene) {
  std::vector<std::shared_ptr<png::SceneObject>> list;
  list.emplace_back(std::make_shared<png::Box>(png::Box{ png::vec3<>{+0.0f,+0.0f,+0.0f},new png::MaterialReflect(png::vec3<>{1.0f,0.1f,0.1f},1.0f,1.0f),0.2f }));
  scene.SetSceneList(list);

  png::SceneLight sceneLight = png::SceneLight();
  scene.SetSceneLight(sceneLight);
}

int main(int, char**) {
  /*
  {
    png::AperturePolygonBlueProgress hoge(6);
    constexpr int P = 10;
#pragma omp parallel for
    for (int p = 0; p <= P; p++) {
      hoge.CheckBluePoint(150, 1e3, 1.0 * p / P);
    }
  }

  {
    png::AperturePolygonBlueSplit hoge(3);
    constexpr int P = 10;
    hoge.CheckBluePoint(150, 1e3 );
  }
  */

  /*4
  {
    //texture check
#pragma omp parallel for
    for (int i = 3; i < 10; i += 1) {
      constexpr unsigned int maxP = 5;
      for (double p = 0; p <= maxP; p++) {
        const double calP = 1.0 * p / maxP;
        std::cout << calP << "  ";
        png::AperturePolygonBlue hoge(i);
        hoge.CheckTexture(10, 1000, calP);
      }
    }
  }
  */

  {
    //texture check
    for (int i = 3; i < 5; i += 1) {
      png::Research hoge(i);
//#pragma omp parallel for
      for (int p = 0; p <= 10; p++) {
        hoge.CheckMandrill(1000, 1.0 * p / 10);
      }
    }
  }

  return 0;

  //speed check
  /*
  {
  constexpr double P = 1e7;
  constexpr double N = 1e4;
  for (double p = 1; p < P; p *= 10) {
    if (p == 0) continue;
    png::Random rnd(0);
    std::chrono::system_clock::time_point  cs, ce, s, e;
    cs = std::chrono::system_clock::now();
    png::AperturePolygonBlueSpeedCheck ap(p);
    ce = std::chrono::system_clock::now();
    s = std::chrono::system_clock::now();
    //std::cout << s.tv_nsec << "  ";
    for (int i = 0; i < N; ++i) {
      ap.SpeedCheck(rnd);
    }
    //std::cout << e.tv_nsec << "  ";
    e = std::chrono::system_clock::now();
    double cElapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(ce - cs).count();
    double elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>((e - s)).count() / N;
    std::cout << std::scientific << std::setprecision(2) << p << std::endl << std::fixed << std::setprecision(5) << elapsed << std::endl << std::endl;
    //std::cout << std::scientific << std::setprecision(2) << p << std::endl << std::fixed << std::setprecision() <<cElapsed << std::endl << elapsed << std::endl << std::endl;
  }

  std::cout << " *** class ***" << std::endl;

  for (double p = 1; p < P; p *= 10) {
    std::chrono::system_clock::time_point  cs, ce;
    cs = std::chrono::system_clock::now();
    for (int i = 0; i < N; ++i) {
      png::AperturePolygonBlueSpeedCheck ap(p);
    }
    ce = std::chrono::system_clock::now();
    double cElapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(ce - cs).count() / N;
    std::cout << std::scientific << std::setprecision(2) << p << std::endl << std::fixed << std::setprecision(5) << cElapsed << std::endl << std::endl;
  }

  return 0;
  }
  */

  InitOpenGL_Loader();
  constexpr int size = 100;

  //scene setting
  png::Scene scene;
  //Init_NiceScene(scene);
  Init_Test(scene);
  std::shared_ptr<png::RenderTarget> renderTarget(std::make_shared<png::RenderTarget>(size, size));
  png::ThinLens cam(
    png::vec3<>{ -7.1, +0.8, +1.7 },
    png::vec3<>{ +0.0, +0.0, +0.0 },
    0,
    0.4,
    new png::AperturePolygonBlue(6),
    //new png::AperturePolygonBlueSplit(6),
    0.9f,
    1.9f
  );
  png::Renderer renderer(&cam, renderTarget, scene);

  png::GUI gui(renderer);

  while (!gui.IsClose()) {
    //renderData.renderer->Draw();
    //renderData.renderTex->Update();
    gui.Update(renderTarget);
  }

  return 0;
}
