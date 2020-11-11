#ifndef _CCANVAS_SCENE_
#define _CCANVAS_SCENE_

#include <camera.h>
#include <point.h>
#include <vec3.h>

typedef struct {
  int a;
  int b;
} Edge;

Edge Edge_new(int a, int b);

typedef struct {
  Camera cam;
  Vec3* vertices;
  int verticesCount;
  Point* projectedPoints;
  Edge* edges;
  int edgeCount;
} Scene;

void Scene_setCamera(Scene* scene, Camera cam);
void Scene_projectPoints(Scene* scene);

#endif