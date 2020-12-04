/**
 * Copyright 2020 Ákos Seres
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

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
typedef struct SoftwareRenderer {
  Scene scene;       // Scene containing the geometry and camera
  Vec3 vel;          // Current velocity of the camera
  double moveForce;  // How much force is applied to the camera when moving
  bool movingForward, movingBackward, movingLeft, movingRight, movingUp,
      movingDown;      // Boolean values describing current movement direction
  double sceneRadius;  // The "size" of the scene, the movement speed and
                       // default camera distance is calculated from it
  Uint32 lastInput;    // Time of the last mouse or keyboard input
  Uint32 currentTick;  // Time at the start of the current loop cycle
} SoftwareRenderer;

void init(CCanvas *cnv);
void update(double dt, CCanvas *cnv);
void draw(CCanvas *cnv);
void onMouseButtonDown(CCanvas *cnv, Uint8 button, Sint32 x, Sint32 y);
void onMouseMove(CCanvas *cnv, Sint32 dx, Sint32 dy);
void onFileDrop(CCanvas *cnv, char *fileName);
void onKeyDown(CCanvas *cnv, SDL_Keycode code);
void onKeyUp(CCanvas *cnv, SDL_Keycode code);
void onResize(CCanvas *cnv, Sint32 newWidth, Sint32 newHeight);
void calculateSceneRadius(SoftwareRenderer *app);
void calculateCameraPosAndSpeed(SoftwareRenderer *app);

// The main function just starts the app
int main(int argc, char *argv[]) {
  SoftwareRenderer app;
  CCanvas_create(init, update, draw, 512, 512, &app);
  // Free up geometry memory after the quit signal
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

  // Set initial tick counts
  app->currentTick = SDL_GetTicks();
  app->lastInput = 0;

  // Set brush colors
  CCanvas_setBgColor(cnv, rgb(0, 0, 0));
  CCanvas_setBrushColor(cnv, rgb(255, 255, 255));

  // Initalise variables with default values
  app->vel = Vec3_new(0, 0, 0);
  app->moveForce = 1;
  app->movingBackward = app->movingDown = app->movingForward = app->movingLeft =
      app->movingRight = app->movingUp = false;
  Scene_erase(scene);
  Scene_setCamera(scene,
                  Camera_new(Vec3_new(0, 0, 0), Vec3_new(0, 1, 0), cnv->width,
                             cnv->height, 3.14 / 3,
                             3.14 / 3));  // Put camera in some default position

  // Then load the base scene
  Scene_loadObj(&(app->scene), "base_scene.obj");
  calculateSceneRadius(app);
  calculateCameraPosAndSpeed(app);

  // Set watchers/event listeners
  CCanvas_watchKeyDown(cnv, onKeyDown);
  CCanvas_watchKeyUp(cnv, onKeyUp);
  CCanvas_watchMouseButtonDown(cnv, onMouseButtonDown);
  CCanvas_watchMouseMove(cnv, onMouseMove);
  CCanvas_watchFileDrop(cnv, onFileDrop);
  CCanvas_watchResize(cnv, onResize);
}

/**
 * The update function handles camera movement and projects the points to screen
 * space
 */
void update(double dt, CCanvas *cnv) {
  SoftwareRenderer *app = (SoftwareRenderer *)cnv->data;
  Scene *scene = &app->scene;

  // Save time of current cycle
  app->currentTick = SDL_GetTicks();

  // Calculate forces accelerating the camera based on the moving direction
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

  // Make the move velocity proportional to the size of the scene
  // Then add the change in velocity to the velocity of the camera
  Vec3_mult(&force, app->sceneRadius * dt * app->moveForce * 0.01);
  Vec3_add(&app->vel, &force);

  // Apply "air friction" to the camera for natural feeling movement
  Vec3_mult(&app->vel, pow(0.00001, dt / 1000));

  // Change the camera position based on its current velocity
  Vec3 displacement = Vec3_copy(&app->vel);
  Vec3_mult(&displacement, dt / 1000);
  Vec3_add(&(scene->cam.pos), &displacement);

  // Put the camera into circulating motion if last input was more than 10
  // seconds ago
  if (app->lastInput == 0 || app->currentTick - app->lastInput > 10000) {
    calculateCameraPosAndSpeed(app);
  }

  // Project points into screen space
  Scene_projectPoints(scene);
}

/**
 * The draw function clears the canvas then proceeds to draw the visible geomety
 */
void draw(CCanvas *cnv) {
  SoftwareRenderer *app = (SoftwareRenderer *)cnv->data;
  Scene *scene = &app->scene;

  // Clear canvas before drawing
  CCanvas_clear(cnv);

  // Loop through all the geometry
  for (long int i = 0; i < scene->edgeCount; i++) {
    Edge e = scene->edges[i];
    if (scene->projectedPoints[e.a].x != NAN &&
        scene->projectedPoints[e.b].x != NAN &&
        scene->projectedPoints[e.a].y != NAN &&
        scene->projectedPoints[e.b].y != NAN) {
      // Use precise lines for drawing because it looks better than with thick
      // lines
      CCanvas_preciseLine(
          cnv, scene->projectedPoints[e.a].x, scene->projectedPoints[e.a].y,
          scene->projectedPoints[e.b].x, scene->projectedPoints[e.b].y);
    }
  }
}

void onMouseButtonDown(CCanvas *cnv, Uint8 button, Sint32 x, Sint32 y) {
  SoftwareRenderer *app = ((SoftwareRenderer *)cnv->data);
  app->lastInput = app->currentTick;
  switch (button) {
    case SDL_BUTTON_LEFT:
      // Lock the mouse if the user clicks on the canvas
      if (!SDL_GetRelativeMouseMode()) SDL_SetRelativeMouseMode(SDL_TRUE);
      break;
  }
}

void onMouseMove(CCanvas *cnv, Sint32 dx, Sint32 dy) {
  SoftwareRenderer *app = (SoftwareRenderer *)cnv->data;
  Scene *scene = &app->scene;
  app->lastInput = app->currentTick;

  // Turn the camera based off of the mouse movement
  Camera_turnRight(&scene->cam, dx / 1000.0);
  Camera_tiltDown(&scene->cam, dy / 1000.0);
}

void onFileDrop(CCanvas *cnv, char *fileName) {
  SoftwareRenderer *app = (SoftwareRenderer *)cnv->data;
  // Free memory of the old geometry then proceed to load the new scene from the
  // given file
  Scene_free(&(app->scene));
  Scene_loadObj(&(app->scene), fileName);
  // Set camera speed for new scene
  calculateSceneRadius(app);
  // Put the camera to an overlook position over the scene
  calculateCameraPosAndSpeed(app);
}

void onKeyDown(CCanvas *cnv, SDL_Keycode code) {
  SoftwareRenderer *app = (SoftwareRenderer *)cnv->data;
  Scene *scene = &app->scene;
  app->lastInput = app->currentTick;

  // Set the corresponding boolean values for each pressed key for describing
  // movement
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
      // Unlock the mouse when pressing ESC
    case SDLK_ESCAPE:
      SDL_SetRelativeMouseMode(SDL_FALSE);
      break;
  }
}

void onKeyUp(CCanvas *cnv, SDL_Keycode code) {
  SoftwareRenderer *app = (SoftwareRenderer *)cnv->data;
  Scene *scene = &app->scene;
  app->lastInput = app->currentTick;

  // Set the corresponding boolean values for each pressed key for describing
  // movement
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

void onResize(CCanvas *cnv, Sint32 newWidth, Sint32 newHeight) {
  SoftwareRenderer *app = (SoftwareRenderer *)cnv->data;
  Camera *cam = &(app->scene.cam);

  // Put the new canvas size into the camera so it projects to the right
  // coordinate system
  cam->hRes = newWidth;
  cam->vRes = newHeight;
}

void calculateSceneRadius(SoftwareRenderer *app) {
  Scene *scene = &(app->scene);
  // Save the size of the scene into the struct
  app->sceneRadius = Scene_radius(scene);
}

void calculateCameraPosAndSpeed(SoftwareRenderer *app) {
  Scene *scene = &(app->scene);
  Camera *cam = &(scene->cam);
  double r = app->sceneRadius;

  // Puts the camera in a position and orientation so that is has a nice view of
  // the scene One of the cylindrical coordinates is proportional to the elapsed
  // time wich results in a circular motion
  double angle = (double)SDL_GetTicks() / 2000.0;
  Vec3 newPos = Vec3_cylindrical(r, angle, r / 1.2);
  Vec3 newDirection = Vec3_copy(&newPos);
  Vec3_setLength(&newDirection, -1);

  Camera_setLookDirection(cam, &newDirection);
  cam->pos = newPos;
}