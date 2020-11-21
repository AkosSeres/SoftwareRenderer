#include <SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <camera.h>
#include <ccanvas.h>
#include <point.h>
#include <scene.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <vec3.h>

// A struct to hold all the data needed for the program
typedef struct {
  Scene scene;       // Scene containing the geometry and camera
  Vec3 vel;          // Current velocity of the camera
  double moveForce;  // How much force is applied to the camera when moving
  bool movingForward, movingBackward, movingLeft, movingRight, movingUp,
      movingDown;
} SoftwareRenderer;

void init(CCanvas *cnv);
void update(double dt, CCanvas *cnv);
void draw(CCanvas *cnv);
void onMouseButtonDown(CCanvas *cnv, Uint8 button, Sint32 x, Sint32 y);
void onMouseMove(CCanvas *cnv, Sint32 dx, Sint32 dy);
void onFileDrop(CCanvas *cnv, char *fileName);
void onKeyDown(CCanvas *cnv, SDL_Keycode code);
void onKeyUp(CCanvas *cnv, SDL_Keycode code);

int main(int argc, char *argv[]) {
  SoftwareRenderer app;
  CCanvas_create(init, update, draw, 512, 512, 512, 512, &app);
  Scene_free(&(app.scene));
  return 0;
}

/**
 * The init function gets called before the app enters the main loop
 * Watchers/event listeners should be set here and initalisation of variables
 */
void init(CCanvas *cnv) {
  // Cast the data pointer stored in cnv to the right type for easy manipulation
  SoftwareRenderer *app = (SoftwareRenderer *)cnv->data;
  Scene *scene = &app->scene;

  // Set brush colors
  CCanvas_setBgColor(cnv, rgb(0, 0, 0));
  CCanvas_setBrushColor(cnv, rgb(255, 255, 255));

  // Initalise variables with default values
  app->vel = Vec3_new(0, 0, 0);
  app->moveForce = 1;
  app->movingBackward = app->movingDown = app->movingForward = app->movingLeft =
      app->movingRight = app->movingUp = false;
  Scene_erase(scene);
  Scene_setCamera(
      scene,
      Camera_new(Vec3_new(0, 0, 0), Vec3_new(0, 1, 0), 512, 512, 3.14 / 3,
                 3.14 / 3));  // Put camera in some default position

  // Set watchers/event listeners
  CCanvas_watchKeyDown(cnv, onKeyDown);
  CCanvas_watchKeyUp(cnv, onKeyUp);
  CCanvas_watchMouseButtonDown(cnv, onMouseButtonDown);
  CCanvas_watchMouseMove(cnv, onMouseMove);
  CCanvas_watchFileDrop(cnv, onFileDrop);
}

void update(double dt, CCanvas *cnv) {
  SoftwareRenderer *app = (SoftwareRenderer *)cnv->data;
  Scene *scene = &app->scene;

  Vec3 force = Vec3_new(0, 0, 0), temp;
  temp = Camera_directionForwardHorizontal(&scene->cam);
  if (app->movingForward) Vec3_add(&force, &temp);
  if (app->movingBackward) Vec3_sub(&force, &temp);
  temp = Camera_directionRight(&scene->cam);
  if (app->movingRight) Vec3_add(&force, &temp);
  if (app->movingLeft) Vec3_sub(&force, &temp);
  if (Vec3_sqLength(&force) != 0) Vec3_setLength(&force, 1);

  if (app->movingUp) Vec3_add(&force, &(scene->cam.up));
  if (app->movingDown) Vec3_sub(&force, &(scene->cam.up));

  Vec3_mult(&force, dt * app->moveForce);
  Vec3_add(&app->vel, &force);

  Vec3_mult(&app->vel, pow(0.00001, dt / 1000));

  Vec3 displacement = Vec3_copy(&app->vel);
  Vec3_mult(&displacement, dt / 1000);
  Vec3_add(&(scene->cam.pos), &displacement);

  Scene_projectPoints(scene);
}

void draw(CCanvas *cnv) {
  SoftwareRenderer *app = (SoftwareRenderer *)cnv->data;
  Scene *scene = &app->scene;

  // Clear canvas before drawing
  CCanvas_clear(cnv);

  for (long int i = 0; i < scene->edgeCount; i++) {
    Edge e = scene->edges[i];
    if (scene->projectedPoints[e.a].x != NAN &&
        scene->projectedPoints[e.b].x != NAN &&
        scene->projectedPoints[e.a].y != NAN &&
        scene->projectedPoints[e.b].y != NAN) {
      CCanvas_line(cnv, scene->projectedPoints[e.a].x,
                   scene->projectedPoints[e.a].y, scene->projectedPoints[e.b].x,
                   scene->projectedPoints[e.b].y, 1);
    }
  }
}

void onMouseButtonDown(CCanvas *cnv, Uint8 button, Sint32 x, Sint32 y) {
  switch (button) {
    case SDL_BUTTON_LEFT:
      if (!SDL_GetRelativeMouseMode()) SDL_SetRelativeMouseMode(SDL_TRUE);
      break;
  }
}

void onMouseMove(CCanvas *cnv, Sint32 dx, Sint32 dy) {
  SoftwareRenderer *app = (SoftwareRenderer *)cnv->data;
  Scene *scene = &app->scene;

  Camera_turnRight(&scene->cam, dx / 1000.0);
  Camera_tiltDown(&scene->cam, dy / 1000.0);
}

void onFileDrop(CCanvas *cnv, char *fileName) {
  Scene_free((Scene *)cnv->data);
  Scene_loadObj((Scene *)cnv->data, fileName);
}

void onKeyDown(CCanvas *cnv, SDL_Keycode code) {
  SoftwareRenderer *app = (SoftwareRenderer *)cnv->data;
  Scene *scene = &app->scene;

  switch (code) {
    case SDLK_w:
      app->movingForward = true;
      break;
    case SDLK_s:
      app->movingBackward = true;
      break;
    case SDLK_a:
      app->movingLeft = true;
      break;
    case SDLK_d:
      app->movingRight = true;
      break;
    case SDLK_SPACE:
      app->movingUp = true;
      break;
    case SDLK_LSHIFT:
      app->movingDown = true;
      break;
    case SDLK_ESCAPE:
      SDL_SetRelativeMouseMode(SDL_FALSE);
      break;
  }
}

void onKeyUp(CCanvas *cnv, SDL_Keycode code) {
  SoftwareRenderer *app = (SoftwareRenderer *)cnv->data;
  Scene *scene = &app->scene;

  switch (code) {
    case SDLK_ESCAPE:
      SDL_SetRelativeMouseMode(SDL_FALSE);
      break;
    case SDLK_w:
      app->movingForward = false;
      break;
    case SDLK_s:
      app->movingBackward = false;
      break;
    case SDLK_a:
      app->movingLeft = false;
      break;
    case SDLK_d:
      app->movingRight = false;
      break;
    case SDLK_SPACE:
      app->movingUp = false;
      break;
    case SDLK_LSHIFT:
      app->movingDown = false;
      break;
  }
}
