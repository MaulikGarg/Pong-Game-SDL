#ifndef ENTITIES_H
#define ENTITIES_H

#include "2dvector.h"
#include "properties.h"

class Ball {
 public:
  Ball(Vec2d pre_position);
  void draw(SDL_Renderer* renderer);

  Vec2d m_current_position;
  SDL_Rect m_ball_properties;
};

class Paddle {
 public:
  Paddle(Vec2d pre_position);
  void draw(SDL_Renderer* renderer);

  Vec2d m_current_position;
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

#endif
