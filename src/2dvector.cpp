#include "2dvector.h"
Vec2d::Vec2d() : m_xPosition{0.0}, m_yPosition{0.0} {}

Vec2d::Vec2d(float x, float y) : m_xPosition{x}, m_yPosition{y} {}

Vec2d& Vec2d::operator*=(const Vec2d& rhs) {
  m_xPosition *= rhs.m_xPosition;
  m_yPosition *= rhs.m_yPosition;
  return *this;
}

Vec2d Vec2d::operator*(float rhs){
  return {m_xPosition*rhs, m_yPosition*rhs};
}

Vec2d& Vec2d::operator+=(const Vec2d& rhs) {
  m_xPosition += rhs.m_xPosition;
  m_yPosition += rhs.m_yPosition;
  return *this;
}

Vec2d Vec2d::operator*(const Vec2d& rhs) {
  return Vec2d{m_xPosition * rhs.m_xPosition, m_yPosition * rhs.m_yPosition};
}

Vec2d Vec2d::operator+(const Vec2d& rhs) {
  return Vec2d{m_xPosition + rhs.m_xPosition, m_yPosition + rhs.m_yPosition};
}

