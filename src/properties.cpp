#include "properties.h"

namespace window {
SDL_Window* mainWindow{nullptr};
SDL_Renderer* mainRenderer{nullptr};
TTF_Font* mainFont{nullptr};
}  // namespace window

namespace music {
Mix_Chunk* wallHit{nullptr};
Mix_Chunk* paddleHit{nullptr};
}  // namespace music
