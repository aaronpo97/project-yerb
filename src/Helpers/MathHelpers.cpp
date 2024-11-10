#include "../../includes/Helpers/MathHelpers.hpp"

namespace MathHelpers {
  float degreesToRadians(const float degrees) {
    const float radians = degrees * pi / 180;
    return radians;
  }

  float radiansToDegrees(const float radians) {
    const float degrees = radians * 180 / pi;
    return degrees;
  }

  /**
   * @brief Pythagorean theorem, returns actual value.
   */
  float pythagoras(const float a, const float b) {
    const float c = sqrt(a * a + b * b);
    return c;
  }
  /**
   * @brief Version of the Pythagorean theorem that doesn't calculate the square root.
   */
  float pythagorasSquared(const float a, const float b) {
    const float c = a * a + b * b;
    return c;
  }
} // namespace MathHelpers