#include <SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <ccanvas.h>
#include <stdbool.h>
#include <stdlib.h>

void draw(CCanvas *cnv) {
  // Clear canvas before drawing
  CCanvas_clear(cnv);
}

// TODO: make event handling cleaner
void update(double dt, CCanvas *cnv) {}

void onKeyDown(CCanvas *cnv, SDL_Scancode code) {
  switch (code) {
    case SDL_SCANCODE_SPACE:
      CCanvas_setBgColor(cnv, rgb(52, 52, 52));
      break;
  }
}

void onKeyUp(CCanvas *cnv, SDL_Scancode code) {
  switch (code) {
    case SDL_SCANCODE_SPACE:
      CCanvas_setBgColor(cnv, rgb(255, 255, 255));
      break;
    case SDL_SCANCODE_ESCAPE:
      CCanvas_quit(cnv);
      break;
  }
}

void init(CCanvas *cnv) {
  CCanvas_watchKeyDown(cnv, onKeyDown);
  CCanvas_watchKeyUp(cnv, onKeyUp);
}

int main(int argc, char *argv[]) {
  CCanvas_create(init, update, draw, 1024, 1024, 512, 512);
  return 0;
}