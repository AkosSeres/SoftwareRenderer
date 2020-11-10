#ifndef _CCANVAS_
#define _CCANVAS_

#include <SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <stdbool.h>
#include <stdlib.h>

typedef struct {
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Event event;
  Uint32 bgColor;
  Uint32 brushColor;
  SDL_Texture* brush;
  bool quit;
  int canvasWidth, canvasHeight;
  void* updateFunc;
  void* drawFunc;
} CCanvas;

typedef void (*updateFuncDef)(double, CCanvas*);
typedef void (*drawFuncDef)(CCanvas*);

void CCanvas_create(updateFuncDef updateFunc,
                    drawFuncDef drawFunc, int canvasWidth,
                    int canvasHeight, int windowWidth, int windowHeight);
void CCanvas_quit(CCanvas* cnv);

void CCanvas_setBgColor(CCanvas* cnv, Uint32 color);
void CCanvas_setBrushColor(CCanvas* cnv, Uint32 color);

// Color handling functions and macros
Uint32 rgba(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
Uint32 rgb(Uint8 r, Uint8 g, Uint8 b);
#define getR(C) ((Uint8)(((C)&0xFF000000) >> 24))
#define getG(C) ((Uint8)(((C)&0x00FF0000) >> 16))
#define getB(C) ((Uint8)(((C)&0x0000FF00) >> 8))
#define getA(C) ((Uint8)(((C)&0x000000FF)))

void CCanvas_clear(CCanvas* cnv);
void CCanvas_loop(void* _cnv);

#endif