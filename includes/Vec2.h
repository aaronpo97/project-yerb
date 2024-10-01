#ifndef VEC2_H
#define VEC2_H

class Vec2 {
public:
  double x, y;

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

  Vec2  normalize();
  float length() const;
};

#endif // VEC2_H
