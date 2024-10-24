#pragma once
#include <iostream>
class Vec2 {
public:
  float x, y;

  Vec2(float x = 0, float y = 0);

  bool operator==(const Vec2 &rhs) const;
  bool operator!=(const Vec2 &rhs) const;

  Vec2 operator-(const Vec2 &rhs) const;
  Vec2 operator+(const Vec2 &rhs) const;
  Vec2 operator*(const float val) const;
  Vec2 operator/(const float val) const;

  void operator+=(const Vec2 &rhs);
  void operator-=(const Vec2 &rhs);
  void operator*=(const float val);
  void operator/=(const float val);

  friend std::ostream &operator<<(std::ostream &os, const Vec2 &vec2);

  Vec2  normalize();
  float length() const;
};
