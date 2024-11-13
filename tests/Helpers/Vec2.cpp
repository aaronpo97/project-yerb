#include "../../includes/Helpers/Vec2.hpp"
#include "../../src/Helpers/Vec2.cpp"
#include "gtest/gtest.h"
#include <cmath>

class Vec2Test : public ::testing::Test {
protected:
  Vec2 v1, v2;

  void SetUp() override {
    std::cout << "Setting up Vec2Test" << std::endl;
    v1 = Vec2(3.0f, 4.0f); // A vector with length 5 (3^2 + 4^2 = 25)
    v2 = Vec2(1.0f, 2.0f); // A vector with length ~2.236 (1^2 + 2^2 = 5)
  }
};

// Test for the constructor and length calculation
TEST_F(Vec2Test, ConstructorAndLength) {
  EXPECT_FLOAT_EQ(v1.length(), 5.0f);
  EXPECT_FLOAT_EQ(v2.length(), std::sqrt(5.0f)); // sqrt(5) ≈ 2.236
}

// Test for the equality operator
TEST_F(Vec2Test, EqualityOperator) {
  Vec2 v3(3.0f, 4.0f);
  EXPECT_TRUE(v1 == v3);  // Same values, should be equal
  EXPECT_FALSE(v1 == v2); // Different values, should not be equal
}

// Test for the inequality operator
TEST_F(Vec2Test, InequalityOperator) {
  EXPECT_TRUE(v1 != v2); // Different values, should be not equal
  Vec2 v3(3.0f, 4.0f);
  EXPECT_FALSE(v1 != v3); // Same values, should be equal
}

// Test for vector addition
TEST_F(Vec2Test, AdditionOperator) {
  Vec2 v3 = v1 + v2;
  EXPECT_FLOAT_EQ(v3.x, 4.0f); // 3.0 + 1.0
  EXPECT_FLOAT_EQ(v3.y, 6.0f); // 4.0 + 2.0
}

// Test for vector subtraction
TEST_F(Vec2Test, SubtractionOperator) {
  Vec2 v3 = v1 - v2;
  EXPECT_FLOAT_EQ(v3.x, 2.0f); // 3.0 - 1.0
  EXPECT_FLOAT_EQ(v3.y, 2.0f); // 4.0 - 2.0
}

// Test for scalar multiplication
TEST_F(Vec2Test, MultiplicationOperator) {
  Vec2 v3 = v1 * 2.0f;
  EXPECT_FLOAT_EQ(v3.x, 6.0f); // 3.0 * 2.0
  EXPECT_FLOAT_EQ(v3.y, 8.0f); // 4.0 * 2.0
}

// Test for scalar division
TEST_F(Vec2Test, DivisionOperator) {
  Vec2 v3 = v1 / 2.0f;
  EXPECT_FLOAT_EQ(v3.x, 1.5f); // 3.0 / 2.0
  EXPECT_FLOAT_EQ(v3.y, 2.0f); // 4.0 / 2.0
}

// Test for vector normalization
TEST_F(Vec2Test, Normalize) {
  Vec2 v3 = v1.normalize();
  EXPECT_FLOAT_EQ(v3.length(), 1.0f); // After normalization, the length should be 1
}

// Test for compound assignment operators (+=, -=, *=, /=)
TEST_F(Vec2Test, CompoundAssignmentOperators) {
  v1 += v2;
  EXPECT_FLOAT_EQ(v1.x, 4.0f); // 3.0 + 1.0
  EXPECT_FLOAT_EQ(v1.y, 6.0f); // 4.0 + 2.0

  v1 -= v2;
  EXPECT_FLOAT_EQ(v1.x, 3.0f); // 4.0 - 1.0
  EXPECT_FLOAT_EQ(v1.y, 4.0f); // 6.0 - 2.0

  v1 *= 2.0f;
  EXPECT_FLOAT_EQ(v1.x, 6.0f); // 3.0 * 2.0
  EXPECT_FLOAT_EQ(v1.y, 8.0f); // 4.0 * 2.0

  v1 /= 2.0f;
  EXPECT_FLOAT_EQ(v1.x, 3.0f); // 6.0 / 2.0
  EXPECT_FLOAT_EQ(v1.y, 4.0f); // 8.0 / 2.0
}

// Test for stream output
TEST_F(Vec2Test, StreamOperator) {
  std::stringstream ss;
  ss << v1;
  EXPECT_EQ(ss.str(), "Vec2(3, 4)"); // The expected output format
}
