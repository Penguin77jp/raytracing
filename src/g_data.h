#pragma once
#include "Singleton.h"
#include "RenderTarget.h"
#include "camera.h"
#include "sceneData.h"
#include <iostream> 

namespace png {
  class G_Data : public Singleton<G_Data> {
  public:
    png::RenderTarget* renderTex;
    png::Camera cam;
    png::Scene scene;
    bool IsRender;
    void Change() {
      cam.Change();
      renderTex->Init();
    }

  private:
    friend class Singleton<G_Data>;
    G_Data() : renderTex(nullptr), cam(png::Camera{}),IsRender(true) {}
  };
}
