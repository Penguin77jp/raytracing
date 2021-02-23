
#include "random.h"
#include "ray.h"
#include "bluenoise.h"

#include <cmath>
#include <numbers>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace png {
  class Aperture {
  public:
    vec3<> virtual Sample(Random& _rand) = 0;
  };

  class ApertureDisk : public Aperture {
  public:
    vec3<> Sample(Random& _rand) override {
      while (true) {
        png::vec3<> p = png::vec3<>(_rand.next01() * 2.0 - 1.0, _rand.next01() * 2.0 - 1.0, 0);
        if (vec3<>::Magnitude(p) >= 1) continue;
        return p;
      }
    }
  };

  class ApertureRectangle : public Aperture {
  public:
    vec3<> Sample(Random& _rand) override {
      return vec3<>(2.0f * _rand.next01() - 1.0f, 2.0f * _rand.next01() - 1.0f, 0.0f);
    }
  };

  class AperturePolygonBlueSplit : public Aperture {
  private:
    BlueNoiseSampler blue;
    std::vector<vec3<>> coner;
    unsigned int polygon;
    int GetIndex(int index, int size) {
      if (index >= size) {
        index -= size;
      }
      return index;
    }
    vec3<> Square2triangle(vec3<> _input) {
      vec3<> cal = _input;
      //std::cout << "Input :" << std::string(_input) << std::endl;
      if (_input.y > _input.x) {
        cal.x *= 0.5f;
        cal.y -= cal.x;
      } else {
        cal.y *= 0.5f;
        cal.x -= cal.y;
      }
      return cal;
    }
  public:
    AperturePolygonBlueSplit(unsigned int _polygon)
      : blue(Texture("blue.png"))
      , polygon(_polygon) {
      for (int i = 0; i < polygon; ++i) {
        double deg = 2.0 * std::numbers::pi * i / polygon;
        vec3<> cal = vec3<>(std::cos(deg), std::sin(deg), 0);
        coner.push_back(cal);
      }
    }

    vec3<> Sample(Random& _rand) override {
      const double randX = _rand.next01();
      const double blueX = randX * blue.GetTextureSize();
      const unsigned int polygonIndex = (unsigned int)(randX * polygon);
      const double randY = _rand.next01();
      const double blueY = randY * blue.GetTextureSize() / polygon;
      const vec3<> offseted = blue.Sample(blueX - ((double)(int)(blueX * polygon) / polygon), blueY);

      vec3<>& vec_a = coner[GetIndex(polygonIndex, coner.size())];
      vec3<>& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

      vec3<> triVec = Square2triangle(vec3<>(blueX / polygon - (int)(blueX / polygon), offseted.y * polygon / blue.GetTextureSize(), 0));

      return vec_a * triVec.x + vec_b * triVec.y;
    }

    void CheckBluePoint(unsigned int _size, int _pointNum) {
      Random _rand(0);
      constexpr unsigned int bpp = 3;
      std::vector<unsigned char> data(_size * _size * bpp, 255);
      for (int i = 0; i < _pointNum; ++i) {
        const double randX = _rand.next01();
        const double blueX = randX * blue.GetTextureSize();
        const unsigned int polygonIndex = (unsigned int)(randX * polygon);
        const double randY = _rand.next01();
        const double blueY = randY * blue.GetTextureSize() / polygon;
        const vec3<> offseted = blue.Sample(blueX - ((double)(int)(blueX * polygon) / polygon), blueY);

        vec3<>& vec_a = coner[GetIndex(polygonIndex, coner.size())];
        vec3<>& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

        vec3<> triVec = Square2triangle(vec3<>(blueX / polygon - (int)(blueX / polygon), offseted.y * polygon / blue.GetTextureSize(), 0));
        vec3<> point = vec_a * triVec.x + vec_b * triVec.y;
        const int indexX = (point.x * 0.5 + 0.5) * (_size - 1);
        const int indexY = (point.y * 0.5 + 0.5) * (_size - 1);
        data[indexX * bpp + indexY * _size * bpp] = 0;
        data[indexX * bpp + indexY * _size * bpp + 1] = 0;
        data[indexX * bpp + indexY * _size * bpp + 2] = 0;
      }
      Texture hoge(data, _size, _size);
      hoge.WriteImage("CheckBluePointSplit.png");
    }
  };

  class AperturePolygonRejection : public Aperture {
  private:
    unsigned int polygon;
    int GetIndex(int index, int size) {
      if (index >= size) {
        index -= size;
      }
      return index;
    }

    float sign(vec3<> p1, vec3<> p2, vec3<> p3) {
      return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    }

    bool PointInTriangle(vec3<> pt, vec3<> v1, vec3<> v2, vec3<> v3) {
      bool b1, b2, b3;

      b1 = sign(pt, v1, v2) < 0.0f;
      b2 = sign(pt, v2, v3) < 0.0f;
      b3 = sign(pt, v3, v1) < 0.0f;

      return ((b1 == b2) && (b2 == b3));
    }

  public:
    AperturePolygonRejection(unsigned int _polygon) : polygon(_polygon) {
    }

    vec3<> Sample(Random& _rand) override {
      std::vector<vec3<>> coner;
      for (int i = 0; i < polygon; ++i) {
        double deg = 2.0 * std::numbers::pi * i / polygon;
        vec3<> cal = vec3<>(std::cos(deg), std::sin(deg), 0);
        coner.push_back(cal);
      }

      while (true) {
        double x = _rand.next01() * 2.0 - 1.0;
        double y = _rand.next01() * 2.0 - 1.0;
        for (int i = 0; i < polygon; ++i) {
          vec3<> tmp = vec3<>(x, y, 0);
          if (PointInTriangle(
            tmp,
            vec3<>(),
            coner[GetIndex(i, polygon)],
            coner[GetIndex(i + 1, polygon)])) {
            return tmp;
          }
        }
      }

      std::cerr << "Out of Area" << std::endl;
      exit(1);
      return coner[0];
    }
  };

  class AperturePolygonBlue : public Aperture {
  private:
    BlueNoiseSampler blue;
    std::vector<vec3<>> coner;
    unsigned int polygon;
    int GetIndex(int index, int size) {
      if (index >= size) {
        index -= size;
      }
      return index;
    }
    vec3<double> Square2triangle(vec3<double> _input) {
      auto cal = _input;
      //std::cout << "Input :" << std::string(_input) << std::endl;
      if (_input.y > _input.x) {
        cal.x *= 0.5;
        cal.y -= cal.x;
      } else {
        cal.y *= 0.5;
        cal.x -= cal.y;
      }
      return cal;
    }
  public:
    AperturePolygonBlue(unsigned int _polygon)
      : blue(Texture("blue.png"))
      , polygon(_polygon) {
      for (int i = 0; i < polygon; ++i) {
        double deg = 2.0 * std::numbers::pi * i / polygon;
        vec3<> cal = vec3<>(std::cos(deg), std::sin(deg), 0);
        coner.push_back(cal);
      }
    }

    vec3<> Sample(Random& _rand) override {
      const double randX = _rand.next01();
      const double blue_x = randX * blue.GetTextureSize();
      const unsigned int polygonIndex = (unsigned int)(randX * polygon);
      const double randY = _rand.next01();
      const double blue_y = randY * blue.GetTextureSize() / polygon;
      const vec3<> offseted = blue.Sample(blue_x, blue_y);

      vec3<>& vec_a = coner[GetIndex(polygonIndex, coner.size())];
      vec3<>& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

      auto triVec = Square2triangle(vec3<double>(blue_x / polygon - (int)(blue_x / polygon), offseted.y * polygon / blue.GetTextureSize(), 0));

      return vec_a * triVec.x + vec_b * triVec.y;
    }

    void CheckBluePoint(unsigned int _size, int _pointNum) {
      Random _rand(0);
      constexpr unsigned int bpp = 3;
      std::vector<unsigned char> data(_size * _size * bpp, 255);
      for (int i = 0; i < _pointNum; ++i) {
        const double randX = _rand.next01();
        const double blue_x = randX * blue.GetTextureSize();
        const unsigned int polygonIndex = (unsigned int)(randX * polygon);
        const double randY = _rand.next01();
        const double blue_y = randY * blue.GetTextureSize() / polygon;
        const vec3<> offseted = blue.Sample(blue_x, blue_y);

        vec3<>& vec_a = coner[GetIndex(polygonIndex, coner.size())];
        vec3<>& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

        auto triVec = Square2triangle(vec3<>(blue_x / polygon - (int)(blue_x / polygon), offseted.y * polygon / blue.GetTextureSize(), 0));
        auto point = vec_a * triVec.x + vec_b * triVec.y;
        const int indexX = (point.x * 0.5 + 0.5) * (_size - 1);
        const int indexY = (point.y * 0.5 + 0.5) * (_size - 1);
        data[indexX * bpp + indexY * _size * bpp] = 0;
        data[indexX * bpp + indexY * _size * bpp + 1] = 0;
        data[indexX * bpp + indexY * _size * bpp + 2] = 0;
      }

      Texture hoge(data, _size, _size);
      std::stringstream ss;
      ss << "./Image/CheckTexture" << polygon << ".png";
      hoge.WriteImage(ss.str().c_str());
    }

    vec3<> Lerp(vec3<> s, vec3<> e, float _progress) {
      return   s * (1.0 - _progress) + e * _progress;
    }

    void CheckTexture(int cell, unsigned int _size, float _prog) {
      constexpr unsigned int bpp = 3;
      std::vector<unsigned char> data(_size * _size * bpp, 255);
      const double pointN = 1e4;
      for (int y = 0; y < pointN; ++y) {
        for (int x = 0; x < pointN; ++x) {
          const double randX = 1.0 * x / pointN;
          const unsigned int polygonIndex = (unsigned int)(randX * polygon);
          const double randY = 1.0 * y / pointN;
          //const vec3<> offseted = blue.Sample(blueX - ((double)(int)(blueX * polygon) / polygon), blueY);

          vec3<>& vec_a = coner[GetIndex(polygonIndex, coner.size())];
          vec3<>& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

          double cellX = randX * polygon - (int)(randX * polygon);
          auto triVec = Square2triangle(vec3<>(cellX, randY, 0));
          auto cellPoint = vec3<>(randX * 2.0 - 1.0, 1.0 - 2 * randY / polygon, 0);
          //auto mappedPoint = vec_a * triVec.x + vec_b * triVec.y;
          auto mappedPoint = vec_b * triVec.x
            + vec3<>(0, 0, 0) * triVec.y
            + vec_a * (1.0 - triVec.x - triVec.y);
          auto point = Lerp(cellPoint, mappedPoint, _prog);
          const int indexX = (point.x * 0.5 + 0.5) * (_size - 1);
          const int indexY = (point.y * 0.5 + 0.5) * (_size - 1);

          int sequence = ((int)(cellX * cell) + (int)(randY * cell));

          if (false && (int)(randX * polygon * cell) == 2 && (int)(randY * cell) == 2) {
            data[indexX * bpp + indexY * _size * bpp] = 255;
            data[indexX * bpp + indexY * _size * bpp + 1] = 0;
            data[indexX * bpp + indexY * _size * bpp + 2] = 0;
          } else if (sequence % 2 == 0) {
            constexpr int cal = 0;
            data[indexX * bpp + indexY * _size * bpp] = cal;
            data[indexX * bpp + indexY * _size * bpp + 1] = cal;
            data[indexX * bpp + indexY * _size * bpp + 2] = cal;
          } else {
            constexpr int cal = 127;
            data[indexX * bpp + indexY * _size * bpp] = cal;
            data[indexX * bpp + indexY * _size * bpp + 1] = cal;
            data[indexX * bpp + indexY * _size * bpp + 2] = cal;
          }

          data[indexX * bpp + indexY * _size * bpp] = 255 * randX;
        }
      }
      Texture hoge(data, _size, _size);
      std::stringstream ss;
      ss << "./Image/CheckTexture" << polygon << "p" << std::fixed << std::setprecision(3) << _prog << ".png";
      hoge.WriteImage(ss.str().c_str());
    }

    void CheckMandrill(unsigned int _size, float _prog) {
      Texture mandrillTex("private src/Mandrill.bmp");

      constexpr unsigned int bpp = 3;
      std::vector<unsigned char> data(_size * _size * bpp, 255);
      const double pointN = 1e4;
      for (int y = 0; y < pointN; ++y) {
        for (int x = 0; x < pointN; ++x) {
          const double randX = 1.0 * x / pointN;
          const unsigned int polygonIndex = (unsigned int)(randX * polygon);
          const double randY = 1.0 * y / pointN;
          //const vec3<> offseted = blue.Sample(blueX - ((double)(int)(blueX * polygon) / polygon), blueY);

          vec3<>& vec_a = coner[GetIndex(polygonIndex, coner.size())];
          vec3<>& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

          double cellX = randX * polygon - (int)(randX * polygon);
          auto triVec = Square2triangle(vec3<>(cellX, randY, 0));
          auto cellPoint = vec3<>(randX * 2.0 - 1.0, 1.0 - 2 * randY / polygon, 0);
          auto mappedPoint = vec_b * triVec.x
            + vec3<>(0, 0, 0) * triVec.y
            + vec_a * (1.0 - triVec.x - triVec.y);

          auto point = Lerp(cellPoint, mappedPoint, _prog);
          const int indexX = (point.x * 0.5 + 0.5) * (_size - 1);
          const int indexY = (point.y * 0.5 + 0.5) * (_size - 1);


          auto col = mandrillTex.GetColor(randX * mandrillTex.width, (1 - randY) * mandrillTex.height / polygon, LoadTextureType::Normal);
          data[indexX * bpp + indexY * _size * bpp] = 255 * col.x;
          data[indexX * bpp + indexY * _size * bpp + 1] = 255 * col.y;
          data[indexX * bpp + indexY * _size * bpp + 2] = 255 * col.z;
        }
      }
      Texture hoge(data, _size, _size);
      std::stringstream ss;
      ss << "./Image/CheckMandrillTexture" << polygon << "p" << std::fixed << std::setprecision(3) << _prog << ".png";
      hoge.WriteImage(ss.str().c_str());
    }

    void SpeedCheck(Random& _rnd) {
      const double randX = _rnd.next01();
      const unsigned int polygonIndex = (unsigned int)(randX * polygon);
      const double randY = _rnd.next01();
      //const vec3<> offseted = blue.Sample(blueX - ((double)(int)(blueX * polygon) / polygon), blueY);

      vec3<>& vec_a = coner[GetIndex(polygonIndex, coner.size())];
      vec3<>& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

      double cellX = randX * polygon - (int)(randX * polygon);
      auto triVec = Square2triangle(vec3<>(cellX, randY, 0));
      auto point = vec_a * triVec.x + vec_b * triVec.y;
    }
  };

  class AperturePolygonBlueSpeedCheck : public Aperture {
  private:
    std::vector<vec3<>> coner;
    unsigned int polygon;
    int GetIndex(int index, int size) {
      if (index >= size) {
        index -= size;
      }
      return index;
    }
    vec3<double> Square2triangle(vec3<double> _input) {
      auto cal = _input;
      //std::cout << "Input :" << std::string(_input) << std::endl;
      if (_input.y > _input.x) {
        cal.x *= 0.5;
        cal.y -= cal.x;
      } else {
        cal.y *= 0.5;
        cal.x -= cal.y;
      }
      return cal;
    }
  public:
    AperturePolygonBlueSpeedCheck(unsigned int _polygon)
      : polygon(_polygon) {
      for (int i = 0; i < polygon; ++i) {
        double deg = 2.0 * std::numbers::pi * i / polygon;
        vec3<> cal = vec3<>(std::cos(deg), std::sin(deg), 0);
        coner.push_back(cal);
      }
    }

    vec3<> Sample(Random& _rand) override {
      return vec3<>();
    }

    void CheckBluePoint(unsigned int _size, int _pointNum) {
    }

    vec3<> Lerp(vec3<> s, vec3<> e, float _progress) {
      return   s * (1.0 - _progress) + e * _progress;
    }

    void CheckTexture(int cell, unsigned int _size, float _prog) {
      constexpr unsigned int bpp = 3;
      std::vector<unsigned char> data(_size * _size * bpp, 255);
      const double pointN = 1e4;
      for (int y = 0; y < pointN; ++y) {
        for (int x = 0; x < pointN; ++x) {
          const double randX = 1.0 * x / pointN;
          const unsigned int polygonIndex = (unsigned int)(randX * polygon);
          const double randY = 1.0 * y / pointN;
          //const vec3<> offseted = blue.Sample(blueX - ((double)(int)(blueX * polygon) / polygon), blueY);

          vec3<>& vec_a = coner[GetIndex(polygonIndex, coner.size())];
          vec3<>& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

          double cellX = randX * polygon - (int)(randX * polygon);
          auto triVec = Square2triangle(vec3<>(cellX, randY, 0));
          auto cellPoint = vec3<>(randX * 2.0 - 1.0, randY * 2.0 - 1.0, 0);
          auto mappedPoint = vec_a * triVec.x + vec_b * triVec.y;
          auto point = Lerp(cellPoint, mappedPoint, _prog);
          const int indexX = (point.x * 0.5 + 0.5) * (_size - 1);
          const int indexY = (point.y * 0.5 + 0.5) * (_size - 1);

          bool black = ((int)(cellX * cell) + (int)(randY * cell)) % 2 == 0;

          if (black) {
            constexpr int cal = 0;
            data[indexX * bpp + indexY * _size * bpp] = cal;
            data[indexX * bpp + indexY * _size * bpp + 1] = cal;
            data[indexX * bpp + indexY * _size * bpp + 2] = cal;
          } else {
            constexpr int cal = 127;
            data[indexX * bpp + indexY * _size * bpp] = cal;
            data[indexX * bpp + indexY * _size * bpp + 1] = cal;
            data[indexX * bpp + indexY * _size * bpp + 2] = cal;
          }
        }
      }
      Texture hoge(data, _size, _size);
      std::stringstream ss;
      ss << "./Image/CheckTexture" << polygon << "p" << std::fixed << std::setprecision(3) << _prog << ".png";
      hoge.WriteImage(ss.str().c_str());
    }

    void SpeedCheck(Random& _rnd) {
      const double randX = _rnd.next01();
      const unsigned int polygonIndex = (unsigned int)(randX * polygon);
      const double randY = _rnd.next01();
      //const vec3<> offseted = blue.Sample(blueX - ((double)(int)(blueX * polygon) / polygon), blueY);

      vec3<>& vec_a = coner[GetIndex(polygonIndex, coner.size())];
      vec3<>& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

      double cellX = randX * polygon - (int)(randX * polygon);
      auto triVec = Square2triangle(vec3<>(cellX, randY, 0));
      auto point = vec_a * triVec.x + vec_b * triVec.y;
    }
  };

  class AperturePolygonBlueProgress : public Aperture {
  private:
    BlueNoiseSampler blue;
    std::vector<vec3<>> coner;
    unsigned int polygon;
    int GetIndex(int index, int size) {
      if (index >= size) {
        index -= size;
      }
      return index;
    }
    vec3<double> Square2triangle(vec3<double> _input) {
      auto cal = _input;
      //std::cout << "Input :" << std::string(_input) << std::endl;
      if (_input.y > _input.x) {
        cal.x *= 0.5;
        cal.y -= cal.x;
      } else {
        cal.y *= 0.5;
        cal.x -= cal.y;
      }
      return cal;
    }
  public:
    AperturePolygonBlueProgress(unsigned int _polygon)
      : blue(Texture("blue.png"))
      , polygon(_polygon) {
      if (_polygon == 2) {
        coner.push_back(vec3<>(1, 0, 0));
        coner.push_back(vec3<>(0, 1, 0));
        polygon = 2;
      } else {
        for (int i = 0; i < polygon; ++i) {
          double deg = 2.0 * std::numbers::pi * i / polygon;
          vec3<> cal = vec3<>(std::cos(deg), std::sin(deg), 0);
          coner.push_back(cal);
        }
      }
    }
    vec3<> Sample(Random& _rand) override {
      double blue_x = _rand.next01();
      const unsigned int polygonIndex = (unsigned int)(blue_x * polygon);
      double x = blue_x * polygon - polygonIndex;
      double y = _rand.next01();
      double blue_y = y / polygon;
      const vec3<> offseted = blue.Sample(blue_x, blue_y);

      vec3<>& vec_a = coner[GetIndex(polygonIndex, coner.size())];
      vec3<>& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

      vec3<> triVec = Square2triangle(vec3<>(x, y, 0));

      return vec_a * triVec.x + vec_b * triVec.y;
    }

    vec3<> Lerp(vec3<> s, vec3<> e, float _progress) {
      return   s * (1.0 - _progress) + e * _progress;
    }
    void CheckBluePoint(const unsigned int _size, const int _pointNum, const float _progress) {
      Random _rand(0);
      constexpr unsigned int bpp = 3;
      std::vector<unsigned char> data(_size * _size * bpp, 255);
      for (int i = 0; i < _pointNum; ++i) {
        const double randX = _rand.next01();
        const double blue_x = randX * blue.GetTextureSize();
        const unsigned int polygonIndex = (unsigned int)(randX * polygon);
        const double randY = _rand.next01();
        const double blue_y = randY * blue.GetTextureSize() / polygon;
        const vec3<> offseted = blue.Sample(blue_x, blue_y);

        vec3<>& vec_a = coner[GetIndex(polygonIndex, coner.size())];
        vec3<>& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

        vec3<> triVec = Square2triangle(vec3<>(blue_x / polygon - (int)(blue_x / polygon), offseted.y * polygon / blue.GetTextureSize(), 0));
        auto cellPoint = vec3<>(randX * 2.0 - 1.0, randY * 2.0 / polygon - 1.0, 0);
        auto mappedPoint = vec_a * triVec.x + vec_b * triVec.y;
        auto point = Lerp(cellPoint, mappedPoint, _progress);

        const int indexX = (point.x * 0.5 + 0.5) * (_size - 1);
        const int indexY = (point.y * 0.5 + 0.5) * (_size - 1);
        data[indexX * bpp + indexY * _size * bpp] = 0;
        data[indexX * bpp + indexY * _size * bpp + 1] = 0;
        data[indexX * bpp + indexY * _size * bpp + 2] = 0;
      }
      Texture hoge(data, _size, _size);
      std::string fileName = std::string("CheckBluePoint") + std::to_string(polygon) + std::string(" , ") + std::to_string(_progress) + std::string(".png");
      hoge.WriteImage(fileName.c_str());
    }
  };

  class ApertureBoxBlue : public Aperture {
  private:
    BlueNoiseSampler blue;
    int GetIndex(int index, int size) {
      if (index >= size) {
        index -= size;
      }
      return index;
    }
    vec3<> Square2triangle(vec3<> _input) {
      vec3<> cal = _input;
      //std::cout << "Input :" << std::string(_input) << std::endl;
      if (_input.y > _input.x) {
        cal.x *= 0.5f;
        cal.y -= cal.x;
      } else {
        cal.y *= 0.5f;
        cal.x -= cal.y;
      }
      return cal;
    }
  public:
    ApertureBoxBlue()
      : blue(Texture("blue.png")) {
    }
    vec3<> Sample(Random& _rand) override {
      double blue_x = _rand.next01();
      const unsigned int polygonIndex = (unsigned int)(blue_x);
      double x = blue_x - polygonIndex;
      double y = _rand.next01();
      double blue_y = y;
      const vec3<> offseted = blue.Sample(blue_x, blue_y);

      vec3<> vec_a = vec3<>(1, 1, 0);
      vec3<> vec_b = vec3<>(1, 0, 0);

      vec3<> triVec = Square2triangle(vec3<>(x, y, 0));

      return vec_a * triVec.x + vec_b * triVec.y;
    }
    void CheckBluePoint(unsigned int _size, int _pointNum, float _progress) {
      Random _rand(0);
      constexpr unsigned int bpp = 3;
      std::vector<unsigned char> data(_size * _size * bpp, 255);
      for (int i = 0; i < _pointNum; ++i) {
        const double randX = _rand.next01() / 3;
        const double blue_x = randX * blue.GetTextureSize();
        const unsigned int polygonIndex = (unsigned int)(randX);
        const double randY = _rand.next01();
        const double blue_y = randY * blue.GetTextureSize();
        const vec3<> offseted = blue.Sample(blue_x, blue_y);

        vec3<> vec_a = vec3<>(1, 1, 0);
        vec3<> vec_b = vec3<>(1, 0, 0);

        vec3<> triVec = Square2triangle(vec3<>(blue_x - (int)(blue_x), offseted.y / blue.GetTextureSize(), 0));

        vec3<> point = vec_a * triVec.x + vec_b * triVec.y;
        const int indexX = point.x * (_size - 1);
        const int indexY = point.y * (_size - 1);
        data[indexX * bpp + indexY * _size * bpp] = 0;
        data[indexX * bpp + indexY * _size * bpp + 1] = 0;
        data[indexX * bpp + indexY * _size * bpp + 2] = 0;
      }
      Texture hoge(data, _size, _size);
      hoge.WriteImage("CheckBluePoint.png");
    }
  };

  class AperturePolygonSquare : public Aperture {
  private:
    BlueNoiseSampler blue;
    std::vector<vec3<>> coner;
    unsigned int polygon;
    int GetIndex(int index, int size) {
      if (index >= size) {
        index -= size;
      }
      return index;
    }
  public:
    AperturePolygonSquare(unsigned int _polygon)
      : blue(Texture("blue.png"))
      , polygon(_polygon) {
      for (int i = 0; i < polygon; ++i) {
        double deg = 2.0 * std::numbers::pi * i / polygon;
        vec3<> cal = vec3<>(std::cos(deg), std::sin(deg), 0);
        coner.push_back(cal);
      }
    }
    vec3<> Sample(Random& _rand) override {
      double blue_x = _rand.next01();
      const unsigned int polygonIndex = (unsigned int)(blue_x * polygon);
      double x = blue_x * polygon - polygonIndex;
      double y = _rand.next01();
      double blue_y = y / polygonIndex;
      const vec3<> offset = blue.Sample(blue_x, blue_y);

      vec3<>& vec_a = coner[GetIndex(polygonIndex, coner.size())];
      vec3<>& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

      //vec3<> P = vec3<>(std::sqrt(s1) * (s2 * V2 + (1 - s2) * V3);

      //return vec_a * triVec.x + vec_b * triVec.y;
      return vec3<>();
    }
  };

  class AperturePolygonBlueTest : public Aperture {
  private:
    BlueNoiseSampler blue;
    std::vector<vec3<>> coner;
    unsigned int polygon;
    int GetIndex(int index, int size) {
      if (index >= size) {
        index -= size;
      }
      return index;
    }
    vec3<> Square2triangle(vec3<> _input) {
      vec3<> cal = _input;
      if (_input.y > _input.x) {
        cal.x *= 0.5f;
        cal.y -= cal.x;
      } else {
        cal.y *= 0.5f;
        cal.x -= cal.y;
      }
      return cal;
    }
  public:
    AperturePolygonBlueTest(unsigned int _polygon)
      : blue(Texture("blue.png"))
      //: blue(Texture("LDR_RG01_23.png"))
      , polygon(_polygon) {
      for (int i = 0; i < polygon; ++i) {
        double deg = 2.0 * std::numbers::pi * i / polygon;
        vec3<> cal = vec3<>(std::cos(deg), std::sin(deg), 0);
        coner.push_back(cal);
      }
    }
    vec3<> Sample(Random& _rand) override {
      double blue_x = _rand.next01();
      const unsigned int polygonIndex = (unsigned int)(blue_x * polygon);
      double x = blue_x * polygon - polygonIndex;
      double y = _rand.next01();
      double blue_y = y / polygon;
      const vec3<> offseted = blue.Sample(blue_x, blue_y);

      vec3<>& vec_a = coner[GetIndex(polygonIndex, coner.size())];
      vec3<>& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

      vec3<> triVec = Square2triangle(vec3<>(x, y, 0));

      return vec_a * triVec.x + vec_b * triVec.y;
    }
    void CheckBluePoint(int _texSize, int _pointNum) {
      Texture tmp_blue = Texture("blue.png");
      const unsigned int width = _texSize * tmp_blue.width;
      constexpr unsigned int bpp = 3;
      std::vector<unsigned char>data(bpp * width * width, 255);
      Random rand(0);
      for (int y = 0; y < width; ++y) {
        for (int x = 0; x < width; ++x) {
          vec3<> hoge = blue.Sample((double)x / _texSize, (double)y / _texSize);
          const float blue_x01 = hoge.x - (int)(hoge.x);
          const float blue_y01 = hoge.y - (int)(hoge.y);
          const vec3<> point = vec3<>(
            (double)x / _texSize / tmp_blue.width, (double)y / _texSize / tmp_blue.height, 0
            );
          const vec3<> pointInCell = vec3<>(
            (double)x / _texSize - (int)((double)x / _texSize)
            , (double)y / _texSize - (int)((double)y / _texSize)
            , 0);
          const vec3<> pointNextInCell = vec3<>(
            (double)(x + 1) / _texSize - (int)((double)(x + 1) / _texSize)
            , (double)(y + 1) / _texSize - (int)((double)(y + 1) / _texSize)
            , 0);
          if (pointInCell.x <= blue_x01 && blue_x01 < pointNextInCell.x) {
            if (pointInCell.y <= blue_y01 && blue_y01 < pointNextInCell.y) {
              data[x * bpp + y * width * bpp] = 0;
              data[x * bpp + y * width * bpp + 1] = 0;
              data[x * bpp + y * width * bpp + 2] = 0;
            }
          }
        }
      }
      Texture hoge(data, _texSize * blue.GetTextureSize(), _texSize * blue.GetTextureSize());
      hoge.WriteImage("CheckBluePoint.png");
    }
  };

  class Research : public Aperture {
  private:
    BlueNoiseSampler blue;
    std::vector<vec3<>> coner;
    unsigned int polygon;
    int GetIndex(int index, int size) {
      if (index >= size) {
        index -= size;
      }
      return index;
    }
    vec3<double> Square2triangle(vec3<double> _input) {
      auto cal = _input;
      //std::cout << "Input :" << std::string(_input) << std::endl;
      if (_input.y > _input.x) {
        cal.x *= 0.5;
        cal.y -= cal.x;
      } else {
        cal.y *= 0.5;
        cal.x -= cal.y;
      }
      return cal;
    }

    vec3<> Square2triangleInv(vec3<> _input) {
      double t1 = 0.5 * (1.0 - _input.x);
      double t2 = 0.5 * _input.y;
      double offset = t2 - t1;
      if (offset > 0) {
        t2 += offset;
      } else {
        t1 -= offset;
      }
      return vec3<>(t1, t2, 0);
    }
  public:
    Research(unsigned int _polygon)
      : blue(Texture("blue.png"))
      , polygon(_polygon) {
      for (int i = 0; i < polygon; ++i) {
        double deg = 2.0 * std::numbers::pi * i / polygon;
        vec3<> cal = vec3<>(std::cos(deg), std::sin(deg), 0);
        coner.push_back(cal);
      }
    }

    vec3<> Sample(Random& _rand) override {
      const double randX = _rand.next01();
      const double blue_x = randX * blue.GetTextureSize();
      const unsigned int polygonIndex = (unsigned int)(randX * polygon);
      const double randY = _rand.next01();
      const double blue_y = randY * blue.GetTextureSize() / polygon;
      const vec3<> offseted = blue.Sample(blue_x, blue_y);

      vec3<>& vec_a = coner[GetIndex(polygonIndex, coner.size())];
      vec3<>& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

      auto triVec = Square2triangle(vec3<double>(blue_x / polygon - (int)(blue_x / polygon), offseted.y * polygon / blue.GetTextureSize(), 0));

      return vec_a * triVec.x + vec_b * triVec.y;
    }

    void CheckBluePoint(unsigned int _size, int _pointNum) {
      Random _rand(0);
      constexpr unsigned int bpp = 3;
      std::vector<unsigned char> data(_size * _size * bpp, 255);
      for (int i = 0; i < _pointNum; ++i) {
        const double randX = _rand.next01();
        const double blue_x = randX * blue.GetTextureSize();
        const unsigned int polygonIndex = (unsigned int)(randX * polygon);
        const double randY = _rand.next01();
        const double blue_y = randY * blue.GetTextureSize() / polygon;
        const vec3<> offseted = blue.Sample(blue_x, blue_y);

        vec3<>& vec_a = coner[GetIndex(polygonIndex, coner.size())];
        vec3<>& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

        auto triVec = Square2triangle(vec3<>(blue_x / polygon - (int)(blue_x / polygon), offseted.y * polygon / blue.GetTextureSize(), 0));
        auto point = vec_a * triVec.x + vec_b * triVec.y;
        const int indexX = (point.x * 0.5 + 0.5) * (_size - 1);
        const int indexY = (point.y * 0.5 + 0.5) * (_size - 1);
        data[indexX * bpp + indexY * _size * bpp] = 0;
        data[indexX * bpp + indexY * _size * bpp + 1] = 0;
        data[indexX * bpp + indexY * _size * bpp + 2] = 0;
      }

      Texture hoge(data, _size, _size);
      std::stringstream ss;
      ss << "./Image/CheckTexture" << polygon << ".png";
      hoge.WriteImage(ss.str().c_str());
    }

    vec3<> Lerp(vec3<> s, vec3<> e, float _progress) {
      return   s * (1.0 - _progress) + e * _progress;
    }

    vec3<> ComputeCenterPoint(const vec3<>& a, const vec3<>& b) {
      return (a + b) * 0.5;
    }
    vec3<> ComputeCenterPoint(const std::vector<vec3<>>& vec, int a, int b) {
      return ComputeCenterPoint(vec[GetIndex(a, vec.size())], vec[GetIndex(b, vec.size())]);
    }

    void CheckTexture(int cell, unsigned int _size, float _prog) {
      constexpr unsigned int bpp = 3;
      std::vector<unsigned char> data(_size * _size * bpp, 255);
      const double pointN = 1e4;
      for (int y = 0; y < pointN; ++y) {
        for (int x = 0; x < pointN; ++x) {
          const double randX = 1.0 * x / pointN;
          const unsigned int polygonIndex = (unsigned int)(randX * polygon);
          const double randY = 1.0 * y / pointN;
          //const vec3<> offseted = blue.Sample(blueX - ((double)(int)(blueX * polygon) / polygon), blueY);

          vec3<>& vec_a = coner[GetIndex(polygonIndex, coner.size())];
          vec3<>& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

          double cellX = randX * polygon - (int)(randX * polygon);
          auto triVec = Square2triangle(vec3<>(cellX, randY, 0));
          auto cellPoint = vec3<>(randX * 2.0 - 1.0, 1.0 - 2 * randY / polygon, 0);
          //auto mappedPoint = vec_a * triVec.x + vec_b * triVec.y;
          auto mappedPoint = vec_b * triVec.x
            + vec3<>(0, 0, 0) * triVec.y
            + vec_a * (1.0 - triVec.x - triVec.y);
          auto point = Lerp(cellPoint, mappedPoint, _prog);
          const int indexX = (point.x * 0.5 + 0.5) * (_size - 1);
          const int indexY = (point.y * 0.5 + 0.5) * (_size - 1);

          int sequence = ((int)(cellX * cell) + (int)(randY * cell));

          if (false && (int)(randX * polygon * cell) == 2 && (int)(randY * cell) == 2) {
            data[indexX * bpp + indexY * _size * bpp] = 255;
            data[indexX * bpp + indexY * _size * bpp + 1] = 0;
            data[indexX * bpp + indexY * _size * bpp + 2] = 0;
          } else if (sequence % 2 == 0) {
            constexpr int cal = 0;
            data[indexX * bpp + indexY * _size * bpp] = cal;
            data[indexX * bpp + indexY * _size * bpp + 1] = cal;
            data[indexX * bpp + indexY * _size * bpp + 2] = cal;
          } else {
            constexpr int cal = 127;
            data[indexX * bpp + indexY * _size * bpp] = cal;
            data[indexX * bpp + indexY * _size * bpp + 1] = cal;
            data[indexX * bpp + indexY * _size * bpp + 2] = cal;
          }

          data[indexX * bpp + indexY * _size * bpp] = 255 * randX;
        }
      }
      Texture hoge(data, _size, _size);
      std::stringstream ss;
      ss << "./Image/CheckTexture" << polygon << "p" << std::fixed << std::setprecision(3) << _prog << ".png";
      hoge.WriteImage(ss.str().c_str());
    }

    void CheckMandrill(unsigned int _size, float _prog) {
      Texture mandrillTex("private src/Mandrill.bmp");

      constexpr unsigned int bpp = 3;
      std::vector<unsigned char> data(_size * _size * bpp, 255);
      const double pointN = _size * polygon;
      for (int y = 0; y < pointN; ++y) {
        for (int x = 0; x < pointN; ++x) {
          const double randX = 1.0 * x / pointN;
          const unsigned int polygonIndex = (unsigned int)(randX * polygon);
          const double randY = 1.0 * y / pointN;
          //const vec3<> offseted = blue.Sample(blueX - ((double)(int)(blueX * polygon) / polygon), blueY);

          double cellX = randX * polygon - (int)(randX * polygon);
          vec3<> triVec;
          auto cellPoint = vec3<>(randX * 2.0 - 1.0, 1.0 - 2 * randY / polygon, 0);
          vec3<> base, vecX, vecY;
          if (cellX <= 0.5) {
            triVec = Square2triangleInv(vec3<>(cellX * 2, randY, 0));
            base = ComputeCenterPoint(coner, polygonIndex, polygonIndex + 1);
            vecY = vec3<>(0, 0, 0);
            vecX = coner[polygonIndex];
            //base = vec3<>(-1.0 + 2.0 * (polygonIndex + 0.5) / polygon, 1, 0);
            //vecY = vec3<>(-1.0 + 2.0 * (polygonIndex + 0.5) / polygon, 1.0 - 2.0 / polygon, 0);
            //vecX = vec3<>(-1.0 + 2.0 * (polygonIndex) / polygon, 1.0, 0);
          } else {
            triVec = Square2triangle(vec3<>(cellX * 2 - 1.0, randY, 0));
            base = ComputeCenterPoint(coner, polygonIndex, polygonIndex + 1);
            vecX = coner[GetIndex(polygonIndex + 1, coner.size())];
            vecY = vec3<>(0, 0, 0);
            //base = vec3<>(-1.0 + 2.0 * (polygonIndex + 0.5) / polygon, 1, 0);
            //vecY = vec3<>(-1.0 + 2.0 * (polygonIndex + 0.5) / polygon, 1.0 - 2.0 / polygon, 0);
            //vecX = vec3<>(-1.0 + 2.0 * (polygonIndex + 1.0) / polygon, 1.0, 0);
          }

          auto mappedPoint = vecX * triVec.x
            + vecY * triVec.y
            + base * (1.0 - triVec.x - triVec.y);
          auto point = Lerp(cellPoint, mappedPoint, _prog);

          const int indexX = (point.x * 0.5 + 0.5) * (_size - 1);
          const int indexY = (point.y * 0.5 + 0.5) * (_size - 1);

          auto col = mandrillTex.GetColor(randX * mandrillTex.width, (1 - randY) * mandrillTex.height / polygon, LoadTextureType::Normal);
          data[indexX * bpp + indexY * _size * bpp] = 255 * col.x;
          data[indexX * bpp + indexY * _size * bpp + 1] = 255 * col.y;
          data[indexX * bpp + indexY * _size * bpp + 2] = 255 * col.z;
        }
      }
      Texture hoge(data, _size, _size);
      std::stringstream ss;
      ss << "./Image/CheckMandrillTexture" << polygon << "p" << std::fixed << std::setprecision(3) << _prog << ".png";
      hoge.WriteImage(ss.str().c_str());
    }

    void SpeedCheck(Random& _rnd) {
      const double randX = _rnd.next01();
      const unsigned int polygonIndex = (unsigned int)(randX * polygon);
      const double randY = _rnd.next01();
      //const vec3<> offseted = blue.Sample(blueX - ((double)(int)(blueX * polygon) / polygon), blueY);

      vec3<>& vec_a = coner[GetIndex(polygonIndex, coner.size())];
      vec3<>& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

      double cellX = randX * polygon - (int)(randX * polygon);
      auto triVec = Square2triangle(vec3<>(cellX, randY, 0));
      auto point = vec_a * triVec.x + vec_b * triVec.y;
    }
  };
}
