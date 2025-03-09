#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <string>

#define FAILURE false;
#define SUCCESS true;

namespace screen {
constexpr int screen_width{1366};
constexpr int screen_height{720};
}  // namespace screen

namespace window {
constexpr int maxFPS {60};
constexpr int frameDelay{1000/maxFPS};
const std::string fontPath{"../assets/mononoki.ttf"};
extern SDL_Window* mainWindow;
extern SDL_Renderer* mainRenderer;
extern TTF_Font* mainFont;
}  // namespace window

namespace entity_data {
constexpr int ballRadius{15};
constexpr int paddleWidth{10};
constexpr int paddleHeight{120};
constexpr float paddleSpeed{1.0f};
constexpr float ballSpeed{1.0f};
// the slowness impact multiplier of the velocity after a collision occurs
constexpr float collisionImpactVelocity{0.75};
}  // namespace entity_data

namespace music{
  extern Mix_Chunk* wallHit;
  extern Mix_Chunk* paddleHit;
}

#endif  // PROPERTIES_H
