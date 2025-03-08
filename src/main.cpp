#include <array>
#include <chrono>
#include <iostream>

#include "2dvector.h"
#include "entities.h"
#include "properties.h"

bool init();     // initializes the SDL and renderer
void drawNet();  // draws a net(centre line) and border
// handles key presses
void handleKeyDown(const SDL_Event& event, std::array<bool, 4>& buttons);
void handleKeyUp(const SDL_Event& event, std::array<bool, 4>& buttons);
// update the paddle state per frame according to the buttons pressed
void updatePaddles(const std::array<bool, 4>& buttons, Paddle& p1, Paddle& p2);
void close();

enum buttons { paddleOneUp, paddleOneDown, paddleTwoUp, paddleTwoDown, max };

int main(int argc, char* argv[]) {
  if (!init()) {
    std::cerr << "INITIALIZATION FAILURE DETECED.\n";
    return -1;
  } else {
    std::cout << "INITIALIZATION SUCCESS.\n";
  }

  Ball ball{{screen::screen_width / 2 - entity_data::ballRadius / 2,
             screen::screen_height / 2 - entity_data::ballRadius / 2},
            {entity_data::ballSpeed, entity_data::ballSpeed * (float(screen::screen_height)/screen::screen_width)}};

  Paddle p1{{30 + entity_data::paddleWidth,
             screen::screen_height / 2 - entity_data::paddleHeight / 2},
            {0, 0}};

  Paddle p2{{screen::screen_width - 30 - entity_data::paddleWidth,
             screen::screen_height / 2 - entity_data::paddleHeight / 2},
            {0, 0}};

  Score p1Score{{screen::screen_width / 4, 20}};
  Score p2Score{{(screen::screen_width * 3) / 4, 20}};

  float dt{0.0f};
  bool quit{false};
  SDL_Event event;
  // represents the buttons enum, sets all pressed to false
  std::array<bool, 4> buttons{};

  while (!quit) {
    // record the time before the frame has been rendered
    auto startTime{std::chrono::high_resolution_clock::now()};
    // pulls events continuously, eliminates outer loop at close or Esc
    while (SDL_PollEvent(&event) != 0) {
      // if the window is X 'd out
      if (event.type == SDL_QUIT) quit = true;

      // if a key is pressed, check if Esc, if so, quit, otherwise pass into the
      // handler
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE)
          quit = true;
        else {
          handleKeyDown(event, buttons);
        }
      }
      // when a key is lift, pass into the handler
      else if (event.type == SDL_KEYUP) {
        handleKeyUp(event, buttons);
      }
    }

    SDL_SetRenderDrawColor(window::mainRenderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(window::mainRenderer);
    drawNet();

    updatePaddles(buttons, p1, p2);
    p1.move(dt);
    p2.move(dt);
    ball.move(dt);

    p1Score.draw(window::mainRenderer);
    p2Score.draw(window::mainRenderer);
    p1.draw(window::mainRenderer);
    p2.draw(window::mainRenderer);
    ball.draw(window::mainRenderer);
    SDL_RenderPresent(window::mainRenderer);

    // record time when frame finished rendering
    auto endTime{std::chrono::high_resolution_clock::now()};
    // update the time needed to record the time
    auto duration{
        std::chrono::duration<float, std::chrono::milliseconds::period>(
            endTime - startTime)};
    dt = duration.count();
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

  // skip 2 pixels every four pixels
  for (int i = 0; i < screen::screen_height; i++) {
    if (i % 4) {
      SDL_RenderDrawPoint(mainRenderer, screen::screen_width / 2, i);
      i += 6;
    }
  }

  // SDL_SetRenderDrawColor(mainRenderer, 0xff, 0xff, 0xff, 0xff);
}

void handleKeyDown(const SDL_Event& event, std::array<bool, 4>& buttons) {
  switch (event.key.keysym.sym) {
    case SDLK_w:
      buttons[paddleOneUp] = true;
      return;
    case SDLK_s:
      buttons[paddleOneDown] = true;
      return;
    case SDLK_UP:
      buttons[paddleTwoUp] = true;
      return;
    case SDLK_DOWN:
      buttons[paddleTwoDown] = true;
      return;
    default:
      return;
  }
}

void handleKeyUp(const SDL_Event& event, std::array<bool, 4>& buttons) {
  switch (event.key.keysym.sym) {
    case SDLK_w:
      buttons[paddleOneUp] = false;
      return;
    case SDLK_s:
      buttons[paddleOneDown] = false;
      return;
    case SDLK_UP:
      buttons[paddleTwoUp] = false;
      return;
    case SDLK_DOWN:
      buttons[paddleTwoDown] = false;
      return;
    default:
      return;
  }
}

void updatePaddles(const std::array<bool, 4>& buttons, Paddle& p1, Paddle& p2) {
  using namespace entity_data;

  if (buttons[paddleOneUp])
    p1.m_velocity.m_yPosition = -paddleSpeed;
  else if (buttons[paddleOneDown])
    p1.m_velocity.m_yPosition = paddleSpeed;
  else
    p1.m_velocity.m_yPosition = 0.0f;

  if (buttons[paddleTwoUp])
    p2.m_velocity.m_yPosition = -paddleSpeed;
  else if (buttons[paddleTwoDown])
    p2.m_velocity.m_yPosition = paddleSpeed;
  else
    p2.m_velocity.m_yPosition = 0.0f;
}

void close() {
  using namespace window;

  SDL_DestroyRenderer(mainRenderer);
  SDL_DestroyWindow(mainWindow);
  TTF_CloseFont(mainFont);
  TTF_Quit();
  SDL_Quit();
}
