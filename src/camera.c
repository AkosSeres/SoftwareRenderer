/**
 * Copyright 2020 Ákos Seres
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#include <camera.h>

/**
 * Creates a new camera struct and returns it
 * Fills the unspecified members with default values
 */
Camera Camera_new(Vec3 pos, Vec3 up, double hRes, double vRes, double hFov,
                  double vFov) {
  Camera cam;
  cam.pos = pos;
  cam.up = up;
  cam.lookDirection = Vec3_new(1, 0, 0);
  cam.hRes = hRes;
  cam.vRes = vRes;
  cam.hFov = hFov;
  cam.vFov = vFov;
  cam.minAngleUp = M_PI / 180;
  cam.minAngleDown = M_PI / 180;
  return cam;
}

/**
 * Sets the looking direction of the camera
 */
void Camera_setLookDirection(Camera* cam, Vec3* direction) {
  if (Vec3_sqLength(direction) == 0) return;
  cam->lookDirection = *direction;
  Vec3_setLength(&(cam->lookDirection), 1);
}

/**
 * Rotates the camera towards it's realtive left direction by a given angle
 */
void Camera_turnLeft(Camera* cam, double angle) {
  Vec3_rotateAroundAxis(&(cam->lookDirection), &(cam->up), -angle);
}
/**
 * Rotates the camera towards it's realtive right direction by a given angle
 */
void Camera_turnRight(Camera* cam, double angle) {
  Vec3_rotateAroundAxis(&(cam->lookDirection), &(cam->up), angle);
}

/**
 * Rotates the camera upwards by a given angle while making sure it does not
 * flip over
 */
void Camera_tiltUp(Camera* cam, double angle) {
  Vec3 axis = Vec3_cross(&(cam->lookDirection), &(cam->up));
  double currentAngle = Vec3_angle(&(cam->lookDirection), &(cam->up));
  if (currentAngle < M_PI_2) {
    if (currentAngle - angle < cam->minAngleUp)
      angle = currentAngle - cam->minAngleUp;
  } else if (currentAngle > M_PI_2 && angle < 0) {
    if (M_PI - currentAngle + angle < cam->minAngleDown)
      angle = cam->minAngleDown - M_PI + currentAngle;
  }
  Vec3_rotateAroundAxis(&(cam->lookDirection), &axis, angle);
}

/**
 * Rotates the camera downwards by a given angle while making sure it does not
 * flip over
 */
void Camera_tiltDown(Camera* cam, double angle) { Camera_tiltUp(cam, -angle); }

/**
 * Moves the camera by a given distance towards it's looking direction
 */
void Camera_moveForward(Camera* cam, double distance) {
  Vec3 dir = Vec3_copy(&(cam->lookDirection));
  Vec3_setLength(&dir, distance);
  Vec3_add(&(cam->pos), &dir);
}

/**
 * Moves the camera forward horizontally by a given distance
 * The horizontal direction is perpendicular to the 'up' direction
 */
void Camera_moveForwardHorizontally(Camera* cam, double distance) {
  Vec3 right = Vec3_cross(&(cam->up), &cam->lookDirection);
  Vec3 dir = Vec3_cross(&(cam->up), &right);
  Vec3_setLength(&dir, -distance);
  Vec3_add(&(cam->pos), &dir);
}

/**
 * Moves the camera upwards by a given distance
 */
void Camera_moveUp(Camera* cam, double distance) {
  Vec3 dir = Vec3_copy(&(cam->up));
  Vec3_setLength(&dir, distance);
  Vec3_add(&(cam->pos), &dir);
}

/**
 * Moves the camera to it's relative left by a given distance
 */
void Camera_moveLeft(Camera* cam, double distance) {
  Vec3 dir = Vec3_cross(&(cam->lookDirection), &(cam->up));
  Vec3_setLength(&dir, distance);
  Vec3_add(&(cam->pos), &dir);
}

/**
 * Moves the camera to it's relative right by a given distance
 */
void Camera_moveRight(Camera* cam, double distance) {
  Vec3 dir = Vec3_cross(&(cam->lookDirection), &(cam->up));
  Vec3_setLength(&dir, -distance);
  Vec3_add(&(cam->pos), &dir);
}

/**
 * Returns a vector pointing to the camera's realtive left direction
 */
Vec3 Camera_directionLeft(Camera* cam) {
  Vec3 dir = Vec3_cross(&(cam->lookDirection), &(cam->up));
  Vec3_setLength(&dir, 1);
  return dir;
}

/**
 * Returns a vector pointing to the camera's realtive right direction
 */
Vec3 Camera_directionRight(Camera* cam) {
  Vec3 dir = Vec3_cross(&(cam->lookDirection), &(cam->up));
  Vec3_setLength(&dir, -1);
  return dir;
}

/**
 * Returns a vector pointing to the camera's realtive hotizontal forward
 * direction
 */
Vec3 Camera_directionForwardHorizontal(Camera* cam) {
  Vec3 right = Vec3_cross(&(cam->up), &cam->lookDirection);
  Vec3 dir = Vec3_cross(&(cam->up), &right);
  Vec3_setLength(&dir, -1);
  return dir;
}

/**
 * Projects the given vertex to screen space with some over-engineered
 * projection method that I came up with
 */
Point Camera_project(Camera* cam, Vec3* point) {
  Vec3 toPoint = Vec3_copy(point);
  Vec3_sub(&toPoint, &(cam->pos));
  double angleFromCenter = Vec3_dot(&toPoint, &(cam->lookDirection));
  // Return with NaN coordinates if not visible
  if (angleFromCenter <= 0) return Point_new(NAN, NAN);
  Vec3_setLength(&toPoint, 1);
  Vec3 sideAxis = Vec3_cross(&(cam->lookDirection), &(cam->up));

  // Calculate vector pointing to the top-left corner of the frame
  // Then calculate vectors aligned with the screen's coordianate system
  Vec3 topLeft = Vec3_copy(&(cam->lookDirection));
  Vec3_rotateAroundAxis(&topLeft, &sideAxis, -cam->vFov / 2);
  Vec3 axisX = Vec3_copy(&topLeft);
  Vec3_rotateAroundAxis(&topLeft, &(cam->up), -cam->hFov / 2);
  Vec3_rotateAroundAxis(&axisX, &(cam->up), cam->hFov / 2);
  Vec3 axisY = Vec3_copy(&(cam->lookDirection));
  Vec3_rotateAroundAxis(&axisY, &sideAxis, cam->vFov / 2);
  Vec3_rotateAroundAxis(&axisY, &(cam->up), -cam->hFov / 2);

  Vec3_sub(&axisX, &topLeft);
  Vec3_sub(&axisY, &topLeft);
  Vec3_sub(&toPoint, &topLeft);
  double xLen = Vec3_length(&axisX), yLen = Vec3_length(&axisY);
  Vec3_setLength(&axisX, 1);
  Vec3_setLength(&axisY, 1);
  // Calculate relative position on the screen then scale with resolution
  double xCoord = cam->hRes * Vec3_dot(&toPoint, &axisX) / xLen;
  double yCoord = cam->vRes * Vec3_dot(&toPoint, &axisY) / yLen;

  return Point_new(xCoord, yCoord);
}

/**
 * Projects the givent vertex to screen space with a simple projection
 * Basically the vertex is transformed to the camera's coordinate system, then
 * it's screen space coordinates are calculated using a simple
 * transformation to then be scaled to the right resolution
 */
Point Camera_projectLinear(Camera* cam, Vec3* point) {
  // Determine the camera's coordianate system's basis
  Vec3 toPoint = Vec3_copy(point);
  Vec3_sub(&toPoint, &(cam->pos));
  Vec3 right = Vec3_cross(&(cam->up), &cam->lookDirection);
  Vec3_setLength(&right, 1);
  Vec3 perspectiveUp = Vec3_cross(&right, &cam->lookDirection);
  Vec3_setLength(&perspectiveUp, 1);
  // Calculate transformed coordinates
  double y = Vec3_dot(&toPoint, &perspectiveUp);
  double z = Vec3_dot(&toPoint, &cam->lookDirection);
  double x = Vec3_dot(&toPoint, &right);

  // Return with NaN coordinates if not visible
  if (z < 0) return Point_new(NAN, NAN);

  // Perform the transformation to screen space and scale
  return Point_new(cam->hRes * (1 + x / z) / 2,
                   cam->vRes * (1 + (cam->hRes / cam->vRes) * y / z) / 2);
}