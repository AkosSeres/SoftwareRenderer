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
void update(double dt, CCanvas *cnv) {
  SDL_PollEvent(&(cnv->event));
  if (cnv->event.type == SDL_QUIT) {
    CCanvas_quit(cnv);
    return;
  }
}

int main(int argc, char *argv[]) {
  CCanvas_create(update, draw, 1024, 1024, 512, 512);
  return 0;
}