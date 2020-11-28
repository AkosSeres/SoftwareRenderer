/**
 * Copyright 2020 Ákos Seres
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#ifndef _CCANVAS_CAMERA_
#define _CCANVAS_CAMERA_

#include <math.h>
#include <point.h>
#include <vec3.h>

typedef struct {
  Vec3 pos;
  Vec3 up;
  Vec3 lookDirection;
  double hRes;
  double vRes;
  double hFov;
  double vFov;
  double minAngleUp;
  double minAngleDown;
} Camera;

Camera Camera_new(Vec3 pos, Vec3 up, double hRes, double vRes, double hFov,
                  double vFov);
void Camera_setLookDirection(Camera* cam, Vec3* direction);
void Camera_turnLeft(Camera* cam, double angle);
void Camera_turnRight(Camera* cam, double angle);
void Camera_tiltUp(Camera* cam, double angle);
void Camera_tiltDown(Camera* cam, double angle);
void Camera_moveForward(Camera* cam, double distance);
void Camera_moveForwardHorizontally(Camera* cam, double distance);
void Camera_moveUp(Camera* cam, double distance);
void Camera_moveLeft(Camera* cam, double distance);
void Camera_moveRight(Camera* cam, double distance);
Vec3 Camera_directionLeft(Camera* cam);
Vec3 Camera_directionRight(Camera* cam);
Vec3 Camera_directionForwardHorizontal(Camera* cam);
Point Camera_project(Camera* cam, Vec3* point);
Point Camera_projectLinear(Camera* cam, Vec3* point);

#endif