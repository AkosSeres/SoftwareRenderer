/**
 * Copyright 2020 Ákos Seres
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#include <ccanvas.h>

#ifdef __EMSCRIPTEN__
EM_JS(int, getBrowserWidth, (), { return window.innerWidth; });
EM_JS(int, getBrowserHeight, (), { return window.innerHeight; });
#endif

/**
 * Function that creates the CCanvas instance and sets up SDL
 * Creates the window and starts the main loop
 */
void CCanvas_create(initFuncDef initFunc, updateFuncDef updateFunc,
                    drawFuncDef drawFunc, int windowWidth, int windowHeight,
                    void* data) {
  // Allocate memory for the canvas
  CCanvas* cnv = malloc(sizeof(CCanvas));

  // Create window and set sizes (both logical and window size)
  SDL_Init(SDL_INIT_VIDEO);

  // Create window differently based on the platform
  // In the browser the canvas will take up the whole screen so the given size
  // values will be overwritten
#ifdef __EMSCRIPTEN__
  windowWidth = getBrowserWidth();
  windowHeight = getBrowserHeight();
#endif
  SDL_CreateWindowAndRenderer(windowWidth, windowHeight, SDL_WINDOW_RESIZABLE,
                              &(cnv->window), &(cnv->renderer));

  // Set CCanvas member values
  CCancas_resetEventHandlers(cnv);
  cnv->data = data;
  cnv->updateFunc = updateFunc;
  cnv->drawFunc = drawFunc;
  cnv->quit = false;
  cnv->width = windowWidth;
  cnv->height = windowHeight;
  cnv->lastTime = cnv->currentTime =
      clock();  // Set clock values for the first time

  // Create the texture containing the single pixel used for lines
  cnv->brush = SDL_CreateTexture(cnv->renderer, SDL_PIXELFORMAT_RGBA8888,
                                 SDL_TEXTUREACCESS_STREAMING, 1, 1);

  // Set default colors (white for background and black for painting)
  CCanvas_setBgColor(cnv, rgb(255, 255, 255));
  CCanvas_setBrushColor(cnv, rgb(0, 0, 0));

  // Call the init function before entering the main loop
  initFunc(cnv);

  /**
   * This is main loop of the program.
   * It is handled differently in the WASM build
   * The emscripten_set_main_loop_arg uses the window.requestAnimationFrame() JS
   * method to periodically call the given function in sync with the monitor
   * refresh rate and it handles the timing automatically.
   */
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(CCanvas_loop, cnv, 0, 1);
#else
  while (cnv->quit == false) {
    CCanvas_loop(cnv);
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
  SDL_SetRenderDrawColor(cnv->renderer, getR(cnv->brushColor),
                         getG(cnv->brushColor), getB(cnv->brushColor),
                         getA(cnv->brushColor));
}

/**
 * This funciton is called every frame
 * It calls the given update and draw functions then updates the screen
 */
void CCanvas_loop(void* _cnv) {
  // Cast the cnv struct pointer into the right type for easier use
  CCanvas* cnv = (CCanvas*)_cnv;

  // Handle events first
  CCanvas_handleEvents(cnv);

  // Calculate elapsed time
  cnv->currentTime = clock();
  double dt = ((double)(cnv->currentTime - cnv->lastTime) * 1000.0) /
              ((double)CLOCKS_PER_SEC);

  // Call the update function with the elapsed time since the last update in
  // milliseconds
  ((updateFuncDef)(cnv)->updateFunc)(dt, cnv);

  // Then set current time as the last one for the next update
  cnv->lastTime = cnv->currentTime;

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

/**
 * Function for drawing lines with thickness of 1
 * It the default SDL line rendering method
 */
void CCanvas_preciseLine(CCanvas* cnv, int x1, int y1, int x2, int y2) {
  SDL_RenderDrawLine(cnv->renderer, x1, y1, x2, y2);
}

void CCanvas_handleEvents(CCanvas* cnv) {
  // Fetch all events from SDL
  while (SDL_PollEvent(&(cnv->event))) {
    SDL_Event* event = &(cnv->event);

    // Call the corresponding function based on what's happened
    switch (event->type) {
      case SDL_QUIT:
        CCanvas_quit(cnv);
        return;
        break;

      case SDL_KEYDOWN:
        if (cnv->onKeyDown != NULL)
          ((keyDownFunc)cnv->onKeyDown)(cnv, event->key.keysym.sym);
        break;
      case SDL_KEYUP:
        if (cnv->onKeyUp != NULL)
          ((keyUpFunc)cnv->onKeyUp)(cnv, event->key.keysym.sym);
        break;

      case SDL_MOUSEBUTTONDOWN:
        if (cnv->onMouseButtonDown != NULL)
          ((mouseButtonDownFunc)cnv->onMouseButtonDown)(
              cnv, event->button.button, event->button.x, event->button.y);
        break;
      case SDL_MOUSEBUTTONUP:
        if (cnv->onMouseButtonUp != NULL)
          ((mouseButtonUpFunc)cnv->onMouseButtonUp)(
              cnv, event->button.button, event->button.x, event->button.y);
        break;

      case SDL_MOUSEMOTION:
        if (cnv->onMouseMove != NULL)
          ((mouseMoveFunc)cnv->onMouseMove)(cnv, event->motion.xrel,
                                            event->motion.yrel);
        break;

      case SDL_DROPFILE:
        if (cnv->onFileDrop != NULL)
          ((fileDropFunc)cnv->onFileDrop)(cnv, event->drop.file);
        SDL_free(event->drop.file);
        break;

      case SDL_WINDOWEVENT:
        if (event->window.event == SDL_WINDOWEVENT_RESIZED) {
          cnv->width = event->window.data1;
          cnv->height = event->window.data2;
          if (cnv->onResize != NULL)
            ((resizeFunc)cnv->onResize)(cnv, event->window.data1,
                                        event->window.data2);
        }
        break;

      case SDL_USEREVENT:
        switch (event->user.code) {
#ifdef __EMSCRIPTEN__
          case CCANVAS_WASM_WINDOW_RESIZED:
            cnv->width = *((int*)(event->user.data1));
            cnv->height = *((int*)(event->user.data2));
            SDL_SetWindowSize(cnv->window, cnv->width, cnv->height);
            if (cnv->onResize != NULL)
              ((resizeFunc)cnv->onResize)(cnv, cnv->width, cnv->height);
            break;
#endif
        }
        break;
    }
  }
}

/**
 * Sets all event handler function pointers to NULL
 */
void CCancas_resetEventHandlers(CCanvas* cnv) {
  cnv->onKeyDown = NULL;
  cnv->onKeyUp = NULL;
  cnv->onMouseButtonDown = NULL;
  cnv->onMouseButtonUp = NULL;
  cnv->onMouseMove = NULL;
  cnv->onFileDrop = NULL;
  cnv->onResize = NULL;
}

/**
 * These functions are responsible for setting the corresponding function
 * pointer in the structure
 */
void CCanvas_watchKeyDown(CCanvas* cnv, keyDownFunc f) { cnv->onKeyDown = f; }
void CCanvas_watchKeyUp(CCanvas* cnv, keyUpFunc f) { cnv->onKeyUp = f; }
void CCanvas_watchMouseButtonDown(CCanvas* cnv, mouseButtonDownFunc f) {
  cnv->onMouseButtonDown = f;
}
void CCanvas_watchMouseButtonUp(CCanvas* cnv, mouseButtonUpFunc f) {
  cnv->onMouseButtonUp = f;
}
void CCanvas_watchMouseMove(CCanvas* cnv, mouseMoveFunc f) {
  cnv->onMouseMove = f;
}
void CCanvas_watchFileDrop(CCanvas* cnv, fileDropFunc f) {
  cnv->onFileDrop = f;
}
void CCanvas_watchResize(CCanvas* cnv, resizeFunc f) { cnv->onResize = f; }

#ifdef __EMSCRIPTEN__

int CCanvas_dropEventForSDL(char* fileName) {
  SDL_Event* e = SDL_malloc(sizeof(SDL_Event));
  SDL_DropEvent event;
  event.type = SDL_DROPFILE;
  event.timestamp = SDL_GetTicks();
  size_t memLen = strlen(fileName) + 1;
  event.file = SDL_malloc(memLen);
  memcpy(event.file, fileName, memLen);
  event.windowID = 0;
  e->type = SDL_DROPFILE;
  e->drop = event;
  int retVal = SDL_PushEvent(e);
  SDL_free(e);
  return retVal;
}

int CCanvas_browserWasResized() {
  SDL_Event* e = SDL_malloc(sizeof(SDL_Event));
  SDL_UserEvent event;
  event.type = SDL_USEREVENT;
  int* width = malloc(sizeof(int));
  int* height = malloc(sizeof(int));
  *width = getBrowserWidth();
  *height = getBrowserHeight();
  event.data1 = width;
  event.data2 = height;
  event.timestamp = SDL_GetTicks();
  event.windowID = 1;
  event.code = CCANVAS_WASM_WINDOW_RESIZED;
  e->type = SDL_USEREVENT;
  e->user = event;
  int retVal = SDL_PushEvent(e);
  SDL_free(e);
  free(width);
  free(height);
  return retVal;
}

#endif