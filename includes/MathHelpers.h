#ifndef MATH_HELPERS_H
#define MATH_HELPERS_H
#include <cmath>
namespace MathHelpers {
  inline const float pi = atan(1) * 4;
  float              degreesToRadians(const float degrees);
  float              radiansToDegrees(const float radians);
  float              pythagoras(const float a, const float b);

}; // namespace MathHelpers

#endif // MATH_HELPERS_H