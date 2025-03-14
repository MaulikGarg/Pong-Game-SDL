#include <array>
#include <chrono>
#include <iostream>

#include "2dvector.h"
#include "entities.h"
#include "properties.h"

bool init();  // initializes the SDL and renderer
void drawNet(SDL_Rect& left,
             SDL_Rect& right);  // draws a net(centre line) and border
// handles key presses
void handleKeyDown(const SDL_Event& event, std::array<bool, 4>& buttons,
                   Ball& ball);
void handleKeyUp(const SDL_Event& event, std::array<bool, 4>& buttons);
// update the paddle state per frame according to the buttons pressed
void collisionHandler(Ball& ball, Paddle& p1, Paddle& p2, Score& p1Score,
                      Score& p2Score);
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

  SDL_Rect left{0, 0, 8, screen::screen_height};
  SDL_Rect right{screen::screen_width - 8, 0, 8, screen::screen_height};

  Ball ball{{screen::screen_width / 2 - entity_data::ballRadius / 2,
             screen::screen_height / 2 - entity_data::ballRadius / 2},
            {entity_data::ballSpeed, 0}};

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
  Uint32 frameStart{};
  Uint32 frameTime{};
  while (!quit) {
    frameStart = SDL_GetTicks();
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
          handleKeyDown(event, buttons, ball);
        }
      }
      // when a key is lift, pass into the handler
      else if (event.type == SDL_KEYUP) {
        handleKeyUp(event, buttons);
      }
    }

    SDL_SetRenderDrawColor(window::mainRenderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(window::mainRenderer);
    drawNet(left, right);

    updatePaddles(buttons, p1, p2);
    p1.move(dt);
    p2.move(dt);
    ball.move(dt);

    collisionHandler(ball, p1, p2, p1Score, p2Score);

    p1Score.draw();
    p2Score.draw();
    p1.draw();
    p2.draw();
    ball.draw();
    SDL_RenderPresent(window::mainRenderer);

    frameTime = SDL_GetTicks() - frameStart;
    if (frameTime < window::frameDelay) {
      SDL_Delay(window::frameDelay - frameTime);
    }

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
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
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

  // set the sound effects
  {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048)) {
      std::cerr << "Mixer Settings failed: " << Mix_GetError() << '\n';
      return FAILURE;
    } else {
      std::cout << "Mixer set.\n";
    }

    music::paddleHit = Mix_LoadWAV("../assets/1noisecollector.wav");
    music::wallHit = Mix_LoadWAV("../assets/2noisecollector.wav");

    if (!music::paddleHit || !music::wallHit) {
      std::cerr << "Sound effect setting failed: " << Mix_GetError() << '\n';
      return FAILURE;
    } else {
      std::cout << "Sounds set.\n";
    }
  }

  return SUCCESS;
}

void drawNet(SDL_Rect& left, SDL_Rect& right) {
  using namespace window;
  SDL_SetRenderDrawColor(mainRenderer, 0xff, 0xff, 0xff, 0xff);

  // skip 2 pixels every four pixels
  for (int i = 0; i < screen::screen_height; i++) {
    if (i % 4) {
      SDL_RenderDrawPoint(mainRenderer, screen::screen_width / 2, i);
      i += 6;
    }
  }

  // draw the edges
  SDL_SetRenderDrawColor(mainRenderer, 0xff, 0x00, 0x00, 0xff);
  SDL_RenderFillRect(window::mainRenderer, &left);
  SDL_RenderFillRect(window::mainRenderer, &right);

  // set color back to white
  SDL_SetRenderDrawColor(mainRenderer, 0xff, 0xff, 0xff, 0xff);
}

void handleKeyDown(const SDL_Event& event, std::array<bool, 4>& buttons,
                   Ball& ball) {
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

void collisionHandler(Ball& ball, Paddle& p1, Paddle& p2, Score& p1Score,
                      Score& p2Score) {
  collision col{};

  if ((col = checkCollision(ball, p1)).type == collisionType::col_paddle) {

    ball.collide(p1, col);
    Mix_PlayChannel(-1, music::paddleHit, 0);

  } else if ((col = checkCollision(ball, p2)).type ==
             collisionType::col_paddle) {

    ball.collide(p2, col);
    Mix_PlayChannel(-1, music::paddleHit, 0);

  } 

  else if ((col = checkWallCol(ball)).type != collisionType::col_none) {
    ball.collideWall(col);
    Mix_PlayChannel(-1, music::wallHit, 0);
    switch (col.type) {
      case collisionType::col_left:
        p2Score.setScore(p2Score.m_score++);
        break;
      case collisionType::col_right:
        p1Score.setScore(p1Score.m_score++);
        break;
    }
  }
}

void close() {
  using namespace window;

  SDL_DestroyRenderer(mainRenderer);
  SDL_DestroyWindow(mainWindow);
  TTF_CloseFont(mainFont);
  Mix_FreeChunk(music::paddleHit);
  Mix_FreeChunk(music::wallHit);
  Mix_CloseAudio();
  Mix_Quit();
  TTF_Quit();
  SDL_Quit();
}
