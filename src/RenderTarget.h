#pragma once

#include <vector>

namespace png {
  class RenderTarget {
  private:
    uint16_t width, height;
    unsigned int sampleCounter;
    unsigned int* image_id; // GLuint
    std::vector<float> image_dataF;
    std::vector<unsigned char> image_data;

  public:
    RenderTarget(const uint16_t w, const uint16_t h);
    RenderTarget(const RenderTarget& _renderTarget);
    void Update();
    void Init();
    void WriteImage(const char* fileName);

    //Get
    uint16_t GetWidth() const { return width; }
    uint16_t GetHeight() const { return height; }
    unsigned int GetSampleCounter() const { return sampleCounter; }
    unsigned int* GetImageID_ptr() const { return image_id; }
    size_t GetImageDataSize() const { return image_dataF.size(); }
    std::vector<float>& ImageDataF() { return image_dataF; }

    //Set
    void SetResolution(const uint16_t _size) {
      *this = RenderTarget(_size, _size);
    }
  };
}
