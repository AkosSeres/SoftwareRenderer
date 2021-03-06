/**
 * Copyright 2020 Ákos Seres
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#ifndef _CCANVAS_SCENE_
#define _CCANVAS_SCENE_

#include <camera.h>
#include <point.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vec3.h>

/**
 * Struct for storing the vertex indices for the endpoints of an edge wich is
 * just a linesegment to be rendered
 */
typedef struct {
  long int a;
  long int b;
} Edge;

Edge Edge_new(long int a, long int b);

/**
 * Struct containing the whole scene that can be rendered: the camera and the
 * geometry data, plus the points projected to screen space
 */
typedef struct {
  Camera cam;
  Vec3* vertices;
  long int verticesCount;
  Point* projectedPoints;
  Edge* edges;
  long int edgeCount;
} Scene;

void Scene_erase(Scene* scene);
void Scene_setCamera(Scene* scene, Camera cam);
void Scene_projectPoints(Scene* scene);
void Scene_loadObj(Scene* scene, const char* fileName);
void Scene_free(Scene* scene);
double Scene_radius(Scene* scene);

void readVertexNumbers(char* str, long int* vertexList, int* vertexCount);
void pushEdgeNoDuplicates(Scene* scene, long a, long b);

#endif