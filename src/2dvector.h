#ifndef TWO_D_VECTOR_H
#define TWO_D_VECTOR_H

#include "properties.h"

class Vec2d {
 public:
  Vec2d();  // basic constructor, sets values to zero
  Vec2d(float x, float y);

  Vec2d operator+(const Vec2d& rhs);
  Vec2d operator*(const Vec2d& rhs);
  Vec2d operator*(float rhs);

  Vec2d& operator+=(const Vec2d& rhs);
  Vec2d& operator*=(const Vec2d& rhs);

  float m_xPosition;
  float m_yPosition;
};

#endif
