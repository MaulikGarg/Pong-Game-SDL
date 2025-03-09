#include "entities.h"

#include "properties.h"

#define CONST_PI 3.1415

Ball::Ball(Vec2d&& pre_position, Vec2d&& velocity)
    : m_current_position{pre_position}, m_velocity{velocity} {
  m_ball_properties.x = m_current_position.m_xPosition;
  m_ball_properties.y = m_current_position.m_yPosition;
  m_ball_properties.w = entity_data::ballRadius;
  m_ball_properties.h = entity_data::ballRadius;
}

void Ball::draw() {
  m_ball_properties.x = m_current_position.m_xPosition;
  m_ball_properties.y = m_current_position.m_yPosition;

  SDL_RenderFillRect(window::mainRenderer, &m_ball_properties);
}

void Ball::move(float dt) { m_current_position += m_velocity * dt; }

Paddle::Paddle(Vec2d&& pre_position, Vec2d&& velocity)
    : m_current_position{pre_position}, m_velocity{velocity} {
  m_paddle_properties.x = m_current_position.m_xPosition;
  m_paddle_properties.y = m_current_position.m_yPosition;
  m_paddle_properties.w = entity_data::paddleWidth;
  m_paddle_properties.h = entity_data::paddleHeight;
}

void Paddle::draw() {
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

Score::Score(Vec2d&& position)
    : m_current_score{nullptr}, m_temporary{nullptr} {
  m_position.x = position.m_xPosition;
  m_position.y = position.m_yPosition;
  setScore(0);
}

Score::~Score() {
  SDL_FreeSurface(m_temporary);
  SDL_DestroyTexture(m_current_score);
}

void Score::setScore(int score) {
  if (m_temporary) SDL_FreeSurface(m_temporary);
  if (m_current_score) SDL_DestroyTexture(m_current_score);

  m_temporary = TTF_RenderText_Solid(
      window::mainFont, std::to_string(score).c_str(), {0xff, 0xff, 0xff});

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
void Score::draw() {
  SDL_RenderCopy(window::mainRenderer, m_current_score, nullptr, &m_position);
}

collision checkCollision(const Ball& ball, const Paddle& paddle) {
  using namespace entity_data;

  collision col{};

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
  
   // penetration ;) in this wholesome game,
  // refers to how deep the L/R edge of the ball rectangle goes
  // inside the R/L edge of the paddle rectangle when the ball is collided

  // if the ball is coming towards the left paddle, i.e, velocity is less than
  // 0:

  if (ball.m_velocity.m_xPosition < 0) {
    col.penetrated = paddleRight - ballLeft; //negative
  }
  // if the ball is coming towards the right paddle, i.e, velocity is more than
  // 0:
  else if (ball.m_velocity.m_xPosition > 0) {
    col.penetrated = paddleLeft - ballRight;
  }  

    col.type =  collisionType::col_paddle; //positive
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

void Ball::collide(const Paddle& paddle, const collision& col) {
  // check ball velocity direction, direction 1 means to right
  int direction = (m_velocity.m_xPosition > 0 ? 1 : -1);

  m_current_position.m_xPosition += col.penetrated;

  double maxAngle { 75.0 * CONST_PI/180.0};
  double angle{
      (m_current_position.m_yPosition - paddle.m_current_position.m_yPosition) /
      (entity_data::paddleHeight / 2) * maxAngle};

  m_velocity.m_xPosition = entity_data::ballSpeed * cos(angle) * direction;
  m_velocity.m_yPosition = entity_data::ballSpeed * sin(angle);

  // prevent near zero velocity
  if (fabs(m_velocity.m_xPosition) < 0.01f) {
    m_velocity.m_xPosition = direction * 0.01f;
  }
}

void Ball::collideWall(const collision& col) {
  using namespace screen;
  if (col.type == collisionType::col_top ||
      col.type == collisionType::col_bottom) {
    m_current_position.m_yPosition += col.penetrated;
    m_velocity.m_yPosition *= -1;
    if(m_velocity.m_xPosition < 0) m_velocity.m_xPosition -= 0.3f;
    else m_velocity.m_xPosition += 0.3f;
    return;
  }

  // for LR walls, reset ball to top center and launch
  m_current_position.m_xPosition = screen_width / 2 - m_ball_properties.w;
  m_current_position.m_yPosition = 10.0f;
  m_velocity.m_yPosition =
      entity_data::collisionImpactVelocity * entity_data::ballSpeed;

  if (col.type == collisionType::col_left) {
    m_velocity.m_xPosition = entity_data::ballSpeed;
  }

  else if (col.type == collisionType::col_right) {
    m_velocity.m_xPosition = -entity_data::ballSpeed;
  }
}
