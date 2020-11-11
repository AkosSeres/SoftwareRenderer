#include <scene.h>

void Scene_setCamera(Scene* scene, Camera cam) { scene->cam = cam; }

void Scene_projectPoints(Scene* scene) {
  for (int i = 0; i < scene->verticesCount; i++) {
    (scene->projectedPoints)[i] =
        Camera_project(&(scene->cam), &((scene->vertices)[i]));
  }
}

Edge Edge_new(int a, int b) {
  Edge retVal;
  retVal.a = a;
  retVal.b = b;
  return retVal;
}