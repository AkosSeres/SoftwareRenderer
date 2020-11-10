#include <ccanvas.h>

void CCanvas_create(updateFuncDef updateFunc, drawFuncDef drawFunc,
                    int canvasWidth, int canvasHeight, int windowWidth,
                    int windowHeight) {
  // Allocate memory for the canvas
  CCanvas* cnv = malloc(sizeof(CCanvas));

  // Create window and set sizes
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(windowWidth, windowHeight, SDL_WINDOW_RESIZABLE,
                              &(cnv->window), &(cnv->renderer));
  SDL_RenderSetLogicalSize(cnv->renderer, canvasWidth, canvasHeight);

  // Set CCanvas member values
  cnv->updateFunc = updateFunc;
  cnv->drawFunc = drawFunc;
  cnv->canvasWidth = canvasWidth;
  cnv->canvasHeight = canvasHeight;
  cnv->quit = false;

  // Create the texture containing the single pixel used for lines
  cnv->brush = SDL_CreateTexture(cnv->renderer, SDL_PIXELFORMAT_RGBA8888,
                                 SDL_TEXTUREACCESS_STREAMING, 1, 1);

  // Set default colors (white for background and black for painting)
  CCanvas_setBgColor(cnv, rgb(255, 255, 255));
  CCanvas_setBrushColor(cnv, rgb(0, 0, 0));

  /**
   * This is main loop of the program.
   * It is handled differently in the WASM build
   * The emscripten_set_main_loop_arg uses the window.requestAnimationFrame() JS method
   * to periodically call the given function in sync with the monitor refresh rate
   * and it handles the timing automatically.
   * In the native build, the timing has to be handled manually
   * TODO: find a way to sync the native build with the monitor and not use a hardcoded frametime
   */
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(CCanvas_loop, cnv, 0, 1);
#else
  while (cnv->quit == false) {
    CCanvas_loop(cnv);
    SDL_Delay(16);
  }
#endif

  // Free up allocated memory and close window upon quitting
  SDL_DestroyRenderer(cnv->renderer);
  SDL_DestroyWindow(cnv->window);

  SDL_Quit();
  free(cnv);
}

void CCanvas_setBgColor(CCanvas* cnv, Uint32 color) { cnv->bgColor = color; }

void CCanvas_setBrushColor(CCanvas* cnv, Uint32 color) {
  cnv->brushColor = color;

  void* pixel;
  SDL_Rect bRect;
  int pitch = 1;
  bRect.x = 0;
  bRect.y = 0;
  bRect.w = 1;
  bRect.h = 1;
  SDL_LockTexture(cnv->brush, &bRect, &pixel, &pitch);
  Uint32* upixel = (Uint32*)pixel;
  *upixel = color;
  SDL_UnlockTexture(cnv->brush);
}

void CCanvas_quit(CCanvas* cnv) { cnv->quit = true; }

Uint32 rgba(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
  return ((Uint32)a | (Uint32)b << 8 | (Uint32)g << 16 | (Uint32)r << 24);
}

Uint32 rgb(Uint8 r, Uint8 g, Uint8 b) {
  return (255 | (Uint32)b << 8 | (Uint32)g << 16 | (Uint32)r << 24);
}

void CCanvas_clear(CCanvas* cnv) {
  SDL_SetRenderDrawColor(cnv->renderer, getR(cnv->bgColor), getG(cnv->bgColor),
                         getB(cnv->bgColor), getA(cnv->bgColor));
  SDL_RenderClear(cnv->renderer);
}

void CCanvas_loop(void* _cnv) {
  // Cast it into the right type for easier use
  CCanvas* cnv = (CCanvas*)_cnv;

  // Call the update function with a fixed dt (60 fps)
  // TODO: implement frame timing and/or time measuring
  ((updateFuncDef)(cnv)->updateFunc)(16.0, cnv);

  // Call draw function
  ((drawFuncDef)cnv->drawFunc)(cnv);
  // Update screen after rendering
  SDL_RenderPresent(cnv->renderer);
}