#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

#define FAILURE false;
#define SUCCESS true;

namespace screen {
constexpr int screen_width{1366};
constexpr int screen_height{720};
}  // namespace screen

namespace window {
const std::string fontPath{"../assets/mononoki.ttf"};
extern SDL_Window* mainWindow;
extern SDL_Renderer* mainRenderer;
extern TTF_Font* mainFont;
}  // namespace window

namespace entity_data {
constexpr int ballRadius{15};
constexpr int paddleWidth{10};
constexpr int paddleHeight{100};
constexpr float paddleSpeed{1.0f};
constexpr float ballSpeed{0.5f};
}  // namespace entity_data

#endif  // PROPERTIES_H
