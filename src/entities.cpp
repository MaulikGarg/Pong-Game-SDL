#include "entities.h"

#include "properties.h"

Ball::Ball(Vec2d pre_position) : m_current_position{pre_position} {
  m_ball_properties.x = m_current_position.m_xPosition;
  m_ball_properties.y = m_current_position.m_yPosition;
  m_ball_properties.w = entity_data::ballRadius;
  m_ball_properties.h = entity_data::ballRadius;
}

void Ball::draw(SDL_Renderer* renderer) {
  m_ball_properties.x = m_current_position.m_xPosition;
  m_ball_properties.y = m_current_position.m_yPosition;

  SDL_RenderFillRect(window::mainRenderer, &m_ball_properties);
}

Paddle::Paddle(Vec2d pre_position) : m_current_position{pre_position} {
  m_paddle_properties.x = m_current_position.m_xPosition;
  m_paddle_properties.y = m_current_position.m_yPosition;
  m_paddle_properties.w = entity_data::paddleWidth;
  m_paddle_properties.h = entity_data::paddleHeight;
}

void Paddle::draw(SDL_Renderer* renderer) {
  m_paddle_properties.x = m_current_position.m_xPosition;
  m_paddle_properties.y = m_current_position.m_yPosition;

  SDL_RenderFillRect(window::mainRenderer, &m_paddle_properties);
}

Score::Score(Vec2d position) {
  m_position.x = position.m_xPosition;
  m_position.y = position.m_yPosition;

  m_temporary = TTF_RenderText_Solid(window::mainFont, "0", {0xff, 0xff, 0xff});
  
  if(!m_temporary){
    std::cerr << "Font Surface Creation Failed: " << TTF_GetError() << '\n';
    return;
  } 
  
  m_current_score = SDL_CreateTextureFromSurface(window::mainRenderer, m_temporary);
  if(!m_current_score){
    std::cerr << "Font Texture Creation Failed: " << SDL_GetError() << '\n';
    return;
  }
  
  SDL_QueryTexture(m_current_score, nullptr, nullptr, &m_position.w, &m_position.h);

}

Score::~Score(){
  SDL_FreeSurface(m_temporary);
  SDL_DestroyTexture(m_current_score);
}

void Score::draw(SDL_Renderer* renderer){
  SDL_RenderCopy(renderer, m_current_score, nullptr, &m_position);
}
