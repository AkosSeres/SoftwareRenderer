#include <SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <stdbool.h>
#include <stdlib.h>

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;
SDL_Texture *brush;
bool quit = false;

void draw() {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  SDL_RenderPresent(renderer);
}

void update() {
  SDL_PollEvent(&event);
  if (event.type == SDL_QUIT) {
    quit = true;
    return;
  }
}

int main(int argc, char *argv[]) {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(512, 512, SDL_WINDOW_RESIZABLE, &window,
                              &renderer);
  SDL_RenderSetLogicalSize(renderer, 1024, 1024);

  void *pixel;
  SDL_Rect bRect;
  int pitch = 1;
  bRect.x = 0;
  bRect.y = 0;
  bRect.w = 1;
  bRect.h = 1;
  brush = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                            SDL_TEXTUREACCESS_STREAMING, 1, 1);
  SDL_LockTexture(brush, &bRect, &pixel, &pitch);
  Uint32 *upixel = (Uint32 *)pixel;
  *upixel = 0;
  SDL_UnlockTexture(brush);

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(draw, 0, 1);
#else
  while (quit == false) {
    update();
    draw();
    SDL_Delay(16);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();
#endif
  return 0;
}