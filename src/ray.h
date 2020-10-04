#pragma once

#include <cmath>

namespace png {
  class vec3 {
  public:
    float x, y, z;
    vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) :x(x), y(y), z(z) {}
    vec3(vec3& a) : x(a.x), y(a.y), z(a.z) {}

    //operator
    vec3 operator +(const vec3& b) {
      return vec3(this->x + b.x, this->y + b.y, this->z + b.z);
    }
    vec3 operator -(const vec3& b) {
      return vec3(this->x - b.x, this->y - b.y, this->z - b.z);
    }
    vec3 operator -() {
      return vec3(-this->x, -this->y, -this->z);
    }
    vec3 operator *(const vec3& b) {
      return vec3(this->x * b.x, this->y * b.y, this->z * b.z);
    }
    vec3 operator* (float a) {
      return vec3(this->x * a, this->y * a, this->z * a);
    }
    vec3 operator /(const vec3& b) {
      return vec3(this->x / b.x, this->y / b.y, this->z / b.z);
    }
    vec3 operator /(const float b) {
      return vec3(this->x / b, this->y / b, this->z / b);
    }
    vec3& operator +=(const vec3& b) {
      this->x += b.x;
      this->y += b.y;
      this->z += b.z;
      return *this;
    }
    vec3& operator -=(const vec3& b) {
      this->x -= b.x;
      this->y -= b.y;
      this->z -= b.z;
      return *this;
    }
    vec3& operator *=(const vec3& b) {
      this->x *= b.x;
      this->y *= b.y;
      this->z *= b.z;
      return *this;
    }
    vec3& operator /=(const vec3& b) {
      this->x /= b.x;
      this->y /= b.y;
      this->z /= b.z;
      return *this;
    }
    vec3& operator /=(const float& b) {
      this->x /= b;
      this->y /= b;
      this->z /= b;
      return *this;
    }

    static float Dot(vec3 a, vec3 b) {
      return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    static vec3 Cross(vec3 a, vec3 b) {
      return vec3(a.y * a.z - a.z * b.y, -a.x * b.z + a.z * b.x, a.x * b.y - a.y * b.x);
    }
    static float Magnitude(vec3 a) {
      return std::sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
    }
    static vec3 Normalize(vec3 a) {
      return  a / Magnitude(a);
    }
  };

  class Ray {
  public:
    vec3 org, dir;
    Ray(vec3 o, vec3 d) :org(o), dir(d) {}
    Ray() :org(vec3()), dir(vec3()) {}
  };
}
