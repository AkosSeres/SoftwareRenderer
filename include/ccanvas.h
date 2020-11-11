#ifndef _CCANVAS_
#define _CCANVAS_

#include <SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

// Struct that holds all the data needed for the program to run
typedef struct {
  SDL_Window* window;  // SDL window
  SDL_Renderer* renderer;
  SDL_Event event;  // Used for event polling
  Uint32 bgColor;
  Uint32 brushColor;
  SDL_Texture* brush;
  bool quit;  // False by default, the program quits when set to true
  int canvasWidth, canvasHeight;  // Logical size of the canvas (not the window
                                  // size since the window is resizable)
  void* updateFunc;  // Functions given by the user, called every frame in the
                     // main loop
  void* drawFunc;
  // Function pointers called when certain events occur
  void* onKeyDown;
  void* onKeyUp;
  void* onMouseButtonDown;
  void* onMouseButtonUp;
  // Data pointer
  void* data;
} CCanvas;

// Function pointer definitions for main loop functions
typedef void (*updateFuncDef)(double, CCanvas*);
typedef void (*drawFuncDef)(CCanvas*);
typedef void (*initFuncDef)(CCanvas*);

// Functions to handle creating the instance and quitting
void CCanvas_create(initFuncDef initFunc, updateFuncDef updateFunc,
                    drawFuncDef drawFunc, int canvasWidth, int canvasHeight,
                    int windowWidth, int windowHeight, void* data);
void CCanvas_quit(CCanvas* cnv);

// Main loop function
void CCanvas_loop(void* _cnv);

// Functions to set "painting" colors
void CCanvas_setBgColor(CCanvas* cnv, Uint32 color);
void CCanvas_setBrushColor(CCanvas* cnv, Uint32 color);

// Functions and macros for handling colors
Uint32 rgba(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
Uint32 rgb(Uint8 r, Uint8 g, Uint8 b);
#define getR(C) ((Uint8)(((C)&0xFF000000) >> 24))
#define getG(C) ((Uint8)(((C)&0x00FF0000) >> 16))
#define getB(C) ((Uint8)(((C)&0x0000FF00) >> 8))
#define getA(C) ((Uint8)(((C)&0x000000FF)))

// Functions for painting/drawing/clearing the canvas
// TODO: implement functions for drawing rectangles and maybe circles
void CCanvas_clear(CCanvas* cnv);
void CCanvas_line(CCanvas* cnv, int x1, int y1, int x2, int y2, int thickness);

// Function definitions for event handling
typedef void (*keyDownFunc)(CCanvas*, SDL_Scancode);
typedef void (*keyUpFunc)(CCanvas*, SDL_Scancode);
typedef void (*mouseButtonDownFunc)(CCanvas*, Uint8, Sint32, Sint32);
typedef void (*mouseButtonUpFunc)(CCanvas*, Uint8, Sint32, Sint32);

// Functions for event handling and for setting up listeners/watchers
void CCanvas_handleEvents(CCanvas* cnv);
void CCancas_resetEventHandlers(CCanvas* cnv);
void CCanvas_watchKeyDown(CCanvas* cnv, keyDownFunc f);
void CCanvas_watchKeyUp(CCanvas* cnv, keyUpFunc f);
void CCanvas_watchMouseButtonDown(CCanvas* cnv, mouseButtonDownFunc f);
void CCanvas_watchMouseButtonUp(CCanvas* cnv, mouseButtonUpFunc f);

#endif