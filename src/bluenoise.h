
#include "texture.h"

namespace png {
  class BlueNoiseSampler {
  private:
    Texture blueNoise;
  public:
    BlueNoiseSampler(Texture _tex);
    /// <summary>
    /// ブルーノイズパターンを使って、オフセットした座標を取得
    /// </summary>
    /// <param name="x">0 ~ [Width]</param>
    /// <param name="y">0 ~ [Height]</param>
    /// <returns>0 ~  [TextureSize]</returns>
    vec3 Sample(double x, double y);

    // Get
    size_t GetTextureSize() const { return blueNoise.width; }
  };
}
