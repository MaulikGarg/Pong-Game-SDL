#ifndef ENTITIES_H
#define ENTITIES_H

#include "2dvector.h"
#include "properties.h"
enum class collisionType { col_none, col_paddle, col_top, col_bottom, col_left, col_right, col_max };

// this struct represents the type of collision and the penetrated x represents
// the pixels to move the ball relative to a paddle before launching it in
// direction
struct collision {
  collisionType type;
  float penetrated;
};

class Paddle {
 public:
  Paddle(Vec2d&& pre_position, Vec2d&& velocity);
  void draw(SDL_Renderer* renderer);
  void move(float dt);

  Vec2d m_current_position;
  Vec2d m_velocity;
  SDL_Rect m_paddle_properties;
};


class Ball {
  public:
   Ball(Vec2d&& pre_position, Vec2d&& velocity);
   void draw(SDL_Renderer* renderer);
   void move(float dt);
   void collide(const Paddle& paddle, const collision& col);
   void collideWall(const collision& col);
 
   Vec2d m_current_position;
   Vec2d m_velocity;
   SDL_Rect m_ball_properties;
 };

class Score {
 public:
  Score(Vec2d&& position);
  ~Score();
  void setScore(int score);
  void draw(SDL_Renderer* renderer);

  int m_score{};
  SDL_Texture* m_current_score;
  SDL_Rect m_position;
  SDL_Surface* m_temporary;
};

// checks for collisons
collision checkCollision(const Ball& ball, const Paddle& paddle);
collision checkWallCol(const Ball& ball);
#endif
