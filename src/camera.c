#include <camera.h>

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

void Camera_setLookDirection(Camera* cam, Vec3* direction) {
  cam->lookDirection = *direction;
  Vec3_setLength(&(cam->lookDirection), 1);
}

void Camera_turnLeft(Camera* cam, double angle) {
  Vec3_rotateAroundAxis(&(cam->lookDirection), &(cam->up), angle);
}
void Camera_turnRight(Camera* cam, double angle) {
  Vec3_rotateAroundAxis(&(cam->lookDirection), &(cam->up), -angle);
}

void Camera_tiltUp(Camera* cam, double angle) {
  Vec3 axis = Vec3_cross(&(cam->lookDirection), &(cam->up));
  double currentAngle = Vec3_angle(&(cam->lookDirection), &(cam->up));
  if (angle > 0) {
    if (currentAngle - angle < cam->minAngleUp)
      angle = currentAngle - cam->minAngleUp;
  } else if (angle < 0) {
    if (M_PI - currentAngle + angle < cam->minAngleDown)
      angle = cam->minAngleDown - M_PI + currentAngle;
  }
  Vec3_rotateAroundAxis(&(cam->lookDirection), &axis, angle);
}

void Camera_tiltDown(Camera* cam, double angle) { Camera_tiltUp(cam, -angle); }

void Camera_moveForward(Camera* cam, double distance) {
  Vec3 dir = Vec3_copy(&(cam->lookDirection));
  Vec3_setLength(&dir, distance);
  Vec3_add(&(cam->pos), &dir);
}

void Camera_moveUp(Camera* cam, double distance) {
  Vec3 dir = Vec3_copy(&(cam->up));
  Vec3_setLength(&dir, distance);
  Vec3_add(&(cam->pos), &dir);
}

Point Camera_project(Camera* cam, Vec3* point) {
  Vec3 toPoint = Vec3_copy(point);
  Vec3_sub(&toPoint, &(cam->pos));
  double angleFromCenter = Vec3_angle(&toPoint, &(cam->lookDirection));
  if (angleFromCenter >= M_PI_2) return Point_new(0, 0);
  Vec3_setLength(&toPoint, 1);
  Vec3 sideAxis = Vec3_cross(&(cam->lookDirection), &(cam->up));
  Vec3_setLength(&sideAxis, 1);

  Vec3 topLeft = Vec3_copy(&toPoint);
  Vec3_rotateAroundAxis(&topLeft, &sideAxis, cam->vFov / 2);
  Vec3 axisX = Vec3_copy(&topLeft);
  Vec3_rotateAroundAxis(&topLeft, &(cam->up), cam->hFov / 2);
  Vec3_rotateAroundAxis(&axisX, &(cam->up), -cam->hFov / 2);
  Vec3 axisY = Vec3_copy(&toPoint);
  Vec3_rotateAroundAxis(&axisY, &sideAxis, -cam->vFov / 2);
  Vec3_rotateAroundAxis(&axisY, &(cam->up), cam->hFov / 2);

  Vec3_sub(&axisX, &topLeft);
  Vec3_sub(&axisY, &topLeft);
  Vec3_sub(&toPoint, &topLeft);
  double xLen = Vec3_length(&axisX), yLen = Vec3_length(&axisY);
  Vec3_setLength(&axisX, 1);
  Vec3_setLength(&axisY, 1);
  double xCoord = cam->hRes * Vec3_dot(&toPoint, &axisX) / xLen;
  double yCoord = cam->vRes * Vec3_dot(&toPoint, &axisY) / yLen;

  return Point_new(xCoord, yCoord);
}