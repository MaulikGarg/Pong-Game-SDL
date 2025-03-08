#ifndef ENTITIES_H
#define ENTITIES_H

#include "2dvector.h"
#include "properties.h"

enum class collisionType { col_none, col_top, col_middle, col_bottom, col_left, col_right, col_max };

// this struct represents the type of collision and the penetrated x represents
// the pixels to move the ball relative to a paddle before launching it in
// direction
struct collision {
  collisionType type;
  float penetrated_x;
};

class Ball {
 public:
  Ball(Vec2d pre_position, Vec2d velocity);
  void draw(SDL_Renderer* renderer);
  void move(float dt);
  void collide(const collision& col);

  Vec2d m_current_position;
  Vec2d m_velocity;
  SDL_Rect m_ball_properties;
};

class Paddle {
 public:
  Paddle(Vec2d pre_position, Vec2d velocity);
  void draw(SDL_Renderer* renderer);
  void move(float dt);

  Vec2d m_current_position;
  Vec2d m_velocity;
  SDL_Rect m_paddle_properties;
};

class Score {
 public:
  Score(Vec2d position);
  ~Score();
  void draw(SDL_Renderer* renderer);

  SDL_Texture* m_current_score;
  SDL_Rect m_position;
  SDL_Surface* m_temporary;
};

// checks for collison between a ball and a paddle
collision checkCollision(const Ball& ball, const Paddle& paddle);

#endif
