#include "entities.h"

#include "properties.h"

Ball::Ball(Vec2d pre_position, Vec2d velocity)
    : m_current_position{pre_position}, m_velocity{velocity} {
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

void Ball::move(float dt) { m_current_position += m_velocity * dt; }

Paddle::Paddle(Vec2d pre_position, Vec2d velocity)
    : m_current_position{pre_position}, m_velocity{velocity} {
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

void Paddle::move(float dt) {
  m_current_position += m_velocity * dt;

  if (m_current_position.m_yPosition < 0) {
    m_current_position.m_yPosition = 0;
  }

  if (m_current_position.m_yPosition >
      screen::screen_height - m_paddle_properties.h) {
    m_current_position.m_yPosition =
        screen::screen_height - m_paddle_properties.h;
  }
}

Score::Score(Vec2d position) {
  m_position.x = position.m_xPosition;
  m_position.y = position.m_yPosition;

  m_temporary = TTF_RenderText_Solid(window::mainFont, "0", {0xff, 0xff, 0xff});

  if (!m_temporary) {
    std::cerr << "Font Surface Creation Failed: " << TTF_GetError() << '\n';
    return;
  }

  m_current_score =
      SDL_CreateTextureFromSurface(window::mainRenderer, m_temporary);
  if (!m_current_score) {
    std::cerr << "Font Texture Creation Failed: " << SDL_GetError() << '\n';
    return;
  }

  SDL_QueryTexture(m_current_score, nullptr, nullptr, &m_position.w,
                   &m_position.h);
}

Score::~Score() {
  SDL_FreeSurface(m_temporary);
  SDL_DestroyTexture(m_current_score);
}

void Score::draw(SDL_Renderer* renderer) {
  SDL_RenderCopy(renderer, m_current_score, nullptr, &m_position);
}

collision checkCollision(const Ball& ball, const Paddle& paddle) {
  using namespace entity_data;

  collision col{};  // none, 0 penetration

  int ballLeft = ball.m_current_position.m_xPosition;
  int ballRight = ballLeft + ballRadius;
  int ballTop = ball.m_current_position.m_yPosition;
  int ballBottom = ballTop + ballRadius;

  int paddleLeft = paddle.m_current_position.m_xPosition;
  int paddleRight = paddleLeft + paddleWidth;
  int paddleTop = paddle.m_current_position.m_yPosition;
  int paddleBottom = paddleTop + paddleHeight;

  // if true then no collision happened, return empty struct
  if (paddleBottom < ballTop || ballBottom < paddleTop ||
      paddleRight < ballLeft || ballRight < paddleLeft)
    return col;

  // get the co ordinates of the paddle's top and middle divider
  float paddleRangeMiddle =
      paddleBottom -
      1 / 0.3 * paddleHeight;  // the middle is 1/3 above the bottom
  float paddleRangeTop = paddleRangeMiddle -
                         1 / 0.3 * paddleHeight;  // the top is 1/3 above middle

  // penetration ;) in this wholesome game,
  // refers to how deep the L/R edge of the ball rectangle goes
  // inside the R/L edge of the paddle rectangle when the ball is collided

  // if the ball is coming towards the left paddle, i.e, velocity is less than
  // 0:
  if (ball.m_velocity.m_xPosition < 0) {
    col.penetrated = paddleRight - ballLeft;
  }
  // if the ball is coming towards the right paddle, i.e, velocity is more than
  // 0:
  else if (ball.m_velocity.m_xPosition > 0) {
    col.penetrated = paddleLeft - ballRight;
  }

  // now check for which part (top/middle/bottom) the collision happened
  if (ballBottom > paddleTop && ballBottom < paddleRangeTop) {
    col.type = collisionType::col_top;
  } else if (ballBottom > paddleRangeTop && ballBottom < paddleRangeMiddle) {
    col.type = collisionType::col_middle;
  } else {
    col.type = collisionType::col_bottom;
  }

  return col;
}

collision checkWallCol(const Ball& ball) {
  using namespace entity_data;

  collision col{};  // none, 0 penetration
  int ballLeft = ball.m_current_position.m_xPosition;
  int ballRight = ballLeft + ballRadius;
  int ballTop = ball.m_current_position.m_yPosition;
  int ballBottom = ballTop + ballRadius;

  if (ballLeft < 0.0f) {
    col.type = collisionType::col_left;
    return col;
  } else if (ballRight > screen::screen_width) {
    col.type = collisionType::col_right;
    return col;
  } else if (ballTop < 0.0f) {
    col.type = collisionType::col_top;
    col.penetrated = -ballTop;
    return col;
  } else if (ballBottom > screen::screen_height) {
    col.type = collisionType::col_bottom;
    col.penetrated = screen::screen_height - ballBottom;
  }

  return col;
}

void Ball::collide(const collision& col) {
  m_current_position.m_xPosition += col.penetrated;
  m_velocity.m_xPosition *= -1;

  if (col.type == collisionType::col_top) {
    m_velocity.m_yPosition = -0.75f * entity_data::ballSpeed;
  } else if (col.type == collisionType::col_bottom) {
    m_velocity.m_yPosition = 0.75f * entity_data::ballSpeed;
  }
}

void Ball::collideWall(const collision& col) {
  using namespace screen;
  if (col.type == collisionType::col_top ||
      col.type == collisionType::col_bottom) {
    m_current_position.m_yPosition += col.penetrated;
    m_velocity.m_yPosition *= -1;
    return;
  }

  // for LR walls, reset ball to center and launch
  m_current_position.m_xPosition = screen_width / 2 - m_ball_properties.w;
  m_current_position.m_yPosition = screen_height / 2 - m_ball_properties.h;
  m_velocity.m_yPosition = 0.75 * entity_data::ballSpeed;

  if (col.type == collisionType::col_left) {
    m_velocity.m_xPosition = entity_data::ballSpeed;
  }

  else if (col.type == collisionType::col_right) {
    m_velocity.m_xPosition = -entity_data::ballSpeed;
  }
}
