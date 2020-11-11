#include <ccanvas.h>

/**
 * Function that creates the CCanvas instance and sets up SDL
 * Creates the window and starts the main loop
 */
void CCanvas_create(updateFuncDef updateFunc, drawFuncDef drawFunc,
                    int canvasWidth, int canvasHeight, int windowWidth,
                    int windowHeight) {
  // Allocate memory for the canvas
  CCanvas* cnv = malloc(sizeof(CCanvas));

  // Create window and set sizes (both logical and window size)
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
   * The emscripten_set_main_loop_arg uses the window.requestAnimationFrame() JS
   * method to periodically call the given function in sync with the monitor
   * refresh rate and it handles the timing automatically. In the native build,
   * the timing has to be handled manually
   * TODO: find a way to sync the native build with the monitor and not use a
   * hardcoded frametime
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

/**
 * Sets the bavkground color of the canvas
 * It is the color used when CCanvas_clear is called
 * Note that the canvas clearing is not done automatically every frame, it has
 * to be called manually for greater control (there may be some cases when you
 * want to keep the frame from the last update and draw on it additionally)
 */
void CCanvas_setBgColor(CCanvas* cnv, Uint32 color) { cnv->bgColor = color; }

/**
 * Sets the brush color for drawing shapes
 * It sets the pixel's color in the texture that is used for drawing lines and
 * other primitives
 */
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

/**
 * Makes the program terminate after the next update by flipping the quit member
 * to true
 */
void CCanvas_quit(CCanvas* cnv) { cnv->quit = true; }

/**
 * CCanvas only uses standard RGBA8888 colors and stores them as a Uint32 so
 * CCanvas colors are compatible with SDL
 * (SDL stores RGBA8888 like this: 0xRRGGBBAA)
 * This function takes four values for all four channels (0-255) and stores them
 * in a Uint32
 */
Uint32 rgba(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
  return ((Uint32)a | (Uint32)b << 8 | (Uint32)g << 16 | (Uint32)r << 24);
}

/**
 * This function does the same thing as rgba(r, g, b, a), but does not take the
 * alpha channel, returns an opaque color
 */
Uint32 rgb(Uint8 r, Uint8 g, Uint8 b) {
  return (255 | (Uint32)b << 8 | (Uint32)g << 16 | (Uint32)r << 24);
}

/**
 * Fills the whole canvas with the set background color
 */
void CCanvas_clear(CCanvas* cnv) {
  SDL_SetRenderDrawColor(cnv->renderer, getR(cnv->bgColor), getG(cnv->bgColor),
                         getB(cnv->bgColor), getA(cnv->bgColor));
  SDL_RenderClear(cnv->renderer);
}

/**
 * This funciton is called every frame
 * It calls the given update and draw functions then updates the screen
 * TODO: implement time measuring so that the updateFunc is given a hardcoded
 * delta time value
 */
void CCanvas_loop(void* _cnv) {
  // Cast the cnv struct pointer into the right type for easier use
  CCanvas* cnv = (CCanvas*)_cnv;

  // Call the update function with a fixed dt (60 fps)
  // TODO: implement frame timing and/or time measuring
  ((updateFuncDef)(cnv)->updateFunc)(16.0, cnv);

  // Call draw function
  ((drawFuncDef)cnv->drawFunc)(cnv);
  // Update screen after rendering
  SDL_RenderPresent(cnv->renderer);
}

/**
 * Function for drawing lines with arbitrary thickness
 * It uses the 1x1 brush texture for srawing
 * It streches the texture then rotates it to fit the desired footprint
 */
void CCanvas_line(CCanvas* cnv, int x1, int y1, int x2, int y2, int thickness) {
  // Does not do anything if the line has length zero
  if (x1 == x2 && y1 == y2) return;

  // Select the 1x1 texture
  SDL_Rect srcRect;
  srcRect.x = 0;
  srcRect.y = 0;
  srcRect.w = 1;
  srcRect.h = 1;

  // Calculate length, orientation and position of the line
  int len = (int)sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
  int centerX = (x1 + x2) / 2;
  int centerY = (y1 + y2) / 2;
  double angle = acos((double)abs(x1 - x2) / len);
  // Correct the sign of the angle
  // (it has to be correctedbecause len is always positive)
  if ((x1 - x2) * (y1 - y2) < 0) angle *= -1;

  // Set destination rectangle to fit the line as if it was horizontal
  SDL_Rect dst;
  dst.x = centerX - len / 2;
  dst.y = centerY - thickness / 2;
  dst.w = len;
  dst.h = thickness;

  // Render the rectangle while rotating it
  SDL_RenderCopyEx(cnv->renderer, cnv->brush, &srcRect, &dst,
                   angle * 180 / M_PI, NULL, SDL_FLIP_NONE);
}