#include <iostream>

#include "2dvector.h"
#include "entities.h"
#include "properties.h"

bool init();     // initializes the SDL and renderer
void drawNet();  // draws a net(centre line)
void close();

int main(int argc, char* argv[]) {
  if (!init()) {
    std::cerr << "INITIALIZATION FAILURE DETECED.\n";
    return -1;
  } else {
    std::cout << "INITIALIZATION SUCCESS.\n";
  }

  Ball ball{Vec2d{screen::screen_width / 2 - entity_data::ballRadius / 2,
                  screen::screen_height / 2 - entity_data::ballRadius / 2}};

  Paddle p1{Vec2d{30 + entity_data::paddleWidth,
                  screen::screen_height / 2 - entity_data::paddleHeight / 2}};

  Paddle p2{Vec2d{screen::screen_width - 30 - entity_data::paddleWidth,
                  screen::screen_height / 2 - entity_data::paddleHeight / 2}};

  Score p1Score{Vec2d{screen::screen_width/4, 20}};  
  Score p2Score{Vec2d{(screen::screen_width*3)/4, 20}};  

  bool quit{false};
  SDL_Event event;

  while (!quit) {
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT) quit = true;
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE) quit = true;
      }
    }

    SDL_SetRenderDrawColor(window::mainRenderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(window::mainRenderer);
    drawNet();

    ball.draw(window::mainRenderer);
    p1.draw(window::mainRenderer);
    p2.draw(window::mainRenderer);
    p1Score.draw(window::mainRenderer);
    p2Score.draw(window::mainRenderer);
    SDL_RenderPresent(window::mainRenderer);
  }

  return 0;
}

bool init() {
  using namespace window;
  using namespace screen;

  std::cout << "INITIALIZATION STARTED.\n";

  // initialize the libraries
  {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      std::cerr << "SDL INITIALIZATION FAILURE: " << SDL_GetError() << '\n';
      return FAILURE;
    } else {
      std::cout << "SDL Initialized. \n";
    }
  }

  // try to create the window
  {
    mainWindow = SDL_CreateWindow("Pong Pong!", SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, screen_width,
                                  screen_height, SDL_WINDOW_SHOWN);

    if (!mainWindow) {
      std::cerr << "Window creation failed: " << SDL_GetError() << '\n';
      return FAILURE;
    } else {
      std::cout << "Window created.\n";
    }
  }

  // try to create the renderer
  {
    mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!mainRenderer) {
      std::cerr << "Renderer creation failed: " << SDL_GetError() << '\n';
      return FAILURE;
    } else {
      std::cout << "Renderer created.\n";
    }
  }

  // set the font
  {
    TTF_Init();
    mainFont = TTF_OpenFont(fontPath.c_str(), 30);
    if (!mainFont) {
      std::cerr << "Font settings failed: " << TTF_GetError() << '\n';
      return FAILURE;
    } else {
      std::cout << "Font set.\n";
    }
  }

  return SUCCESS;
}

void drawNet() {
  using namespace window;
  SDL_SetRenderDrawColor(mainRenderer, 0xff, 0xff, 0xff, 0xff);

  for (int i = 0; i < screen::screen_height; i++) {
    if (i % 4) {
      SDL_RenderDrawPoint(mainRenderer, screen::screen_width / 2, i);
      i += 6;
    }
  }
}

void close() {
  using namespace window;

  SDL_DestroyRenderer(mainRenderer);
  SDL_DestroyWindow(mainWindow);
  TTF_CloseFont(mainFont);
  TTF_Quit();
  SDL_Quit();
}
