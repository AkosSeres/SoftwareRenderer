/**
 * Copyright 2020 Ákos Seres
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#include <scene.h>

/**
 * Erases the geometry data from the scene by setting the pointers to NULL
 */
void Scene_erase(Scene* scene) {
  scene->vertices = NULL;
  scene->projectedPoints = NULL;
  scene->edges = NULL;
  scene->edgeCount = 0;
  scene->verticesCount = 0;
}

/**
 * Sets the camera in the scene
 */
void Scene_setCamera(Scene* scene, Camera cam) { scene->cam = cam; }

/**
 * Projects all the vertices in the scene to screen space and stores the
 * coordinates in member projectedPoints in the same order
 */
void Scene_projectPoints(Scene* scene) {
  for (long int i = 0; i < scene->verticesCount; i++) {
    (scene->projectedPoints)[i] =
        Camera_projectLinear(&(scene->cam), &((scene->vertices)[i]));
  }
}

/**
 * Creates and returns a new Edge struct
 */
Edge Edge_new(long int a, long int b) {
  Edge retVal;
  retVal.a = a;
  retVal.b = b;
  return retVal;
}

/**
 * Goes through the Wawefront .obj file at the given path and loads the
 * geometry into the scene
 */
void Scene_loadObj(Scene* scene, const char* fileName) {
  // Open file for reading
  FILE* filePointer;
  filePointer = fopen(fileName, "r");
  if (filePointer == NULL) return;
  char line[256];

  // Allocate memory for a default number of vertices and edges
  long allocatedVertices = 512;
  scene->vertices = (Vec3*)malloc(allocatedVertices * sizeof(Vec3));
  scene->verticesCount = 0;

  long allocatedEdges = 1024;
  scene->edges = (Edge*)malloc(allocatedEdges * sizeof(Edge));
  scene->edgeCount = 0;

  // Read the file line by line
  while (fgets(line, sizeof(line), filePointer) != NULL) {
    // Skip empty lines
    if (line[0] == '\0' || line[0] == '\n') {
      continue;
    }
    // If the line starts with the letter v, then it contains a vertex
    if (line[0] == 'v' && line[1] == ' ') {
      // If running out of allocated memory for vertices then allocate more for
      // them, copy the already read data to the new location and free the old
      // pointer
      if ((scene->verticesCount) >= (allocatedVertices - 16)) {
        Vec3* old = scene->vertices;
        scene->vertices = (Vec3*)malloc(2 * allocatedVertices * sizeof(Vec3));
        memcpy(scene->vertices, old, allocatedVertices * sizeof(Vec3));
        free(old);
        allocatedVertices *= 2;
      }
      // Read vertex
      double x, y, z;
      char *nextPtr, *currPtr;
      currPtr = &(line[2]);
      // Strod is used to read and convert to double the three consecutive
      // coordinates after the letter v divided by spaces
      x = strtod(currPtr, &nextPtr);
      currPtr = nextPtr;
      y = strtod(currPtr, &nextPtr);
      currPtr = nextPtr;
      z = strtod(currPtr, &nextPtr);
      // Push the vertex to the list
      scene->vertices[scene->verticesCount++] = Vec3_new(x, y, z);
    }
    // If the line starts with the letter f, it contains a polygon
    // It lists the indices for the vertices of the polygon
    else if (line[0] == 'f' && line[1] == ' ') {
      // Allocate more memory if running out of space
      if ((scene->edgeCount) >= (allocatedEdges - 32)) {
        Edge* old = scene->edges;
        scene->edges = (Edge*)malloc(2 * allocatedEdges * sizeof(Edge));
        memcpy(scene->edges, old, allocatedEdges * sizeof(Edge));
        free(old);
        allocatedEdges *= 2;
      }
      // Read polygon
      long int vertexNumbers[32];
      int vCount = 0;
      // Read and push all edges of the poligon to the edge array while making
      // sure to exclude duplicates
      readVertexNumbers(&(line[2]), vertexNumbers, &vCount);
      for (int i = 1; i < vCount; i++) {
        pushEdgeNoDuplicates(scene, vertexNumbers[i - 1] - 1,
                             vertexNumbers[i] - 1);
      }
      pushEdgeNoDuplicates(scene, vertexNumbers[0] - 1,
                           vertexNumbers[vCount - 1] - 1);
    }
    // It is not commonly used but obj files can also contain 'polylines'
    // They are pretty much handled the same way as polygons except that there
    // does not have to be an edge between the last and the firs polyon
    else if (line[0] == 'l' && line[1] == ' ') {
      // Allocate more memory if running out of space
      if ((scene->edgeCount) >= (allocatedEdges - 64)) {
        Edge* old = scene->edges;
        scene->edges = (Edge*)malloc(2 * allocatedEdges * sizeof(Edge));
        memcpy(scene->edges, old, allocatedEdges * sizeof(Edge));
        free(old);
        allocatedEdges *= 2;
      }
      // Read polyline
      long int vertexNumbers[64];
      int vCount = 0;
      readVertexNumbers(&(line[2]), vertexNumbers, &vCount);
      for (int i = 1; i < vCount; i++) {
        pushEdgeNoDuplicates(scene, vertexNumbers[i - 1] - 1,
                             vertexNumbers[i] - 1);
      }
    }
  }

  // Allocate enough memory for the projected points as well
  scene->projectedPoints = (Point*)malloc(allocatedVertices * sizeof(Point));

  fclose(filePointer);
}

/**
 * Frees up memory allocated by loader functions then erases the scene
 */
void Scene_free(Scene* scene) {
  free(scene->vertices);
  free(scene->edges);
  free(scene->projectedPoints);
  Scene_erase(scene);
}

/**
 * Returns the distance to the origo from the furthest point in the scene
 */
double Scene_radius(Scene* scene) {
  double max = 0;

  for (int i = 0; i < scene->verticesCount; i++) {
    Vec3* v = &(scene->vertices[i]);
    double r = v->x * v->x + v->y * v->y + v->z * v->z;
    max = max < r ? r : max;
  }

  return sqrt(max);
}

/**
 * Function for loading any number of integers inside a string divided by spaces
 * to the array at vertexList
 */
void readVertexNumbers(char* str, long int* vertexList, int* vertexCount) {
  char* current = str;
  char* next;
  vertexList[0] = strtol(current, &next, 10);
  if (vertexList[0] == 0) {
    return;
  }
  (*vertexCount)++;
  current = next;
  if (*current == '\n' || *current == '\0') return;
  while (*current != ' ') {
    current = &(current[1]);
    if (*current == '\n' || *current == '\0') return;
  }
  readVertexNumbers(current, &(vertexList[1]), vertexCount);
}

/**
 * Pushes an edge to the array of edges if it is not a duplicate
 */
void pushEdgeNoDuplicates(Scene* scene, long a, long b) {
  if (a > b) {
    int swap = b;
    b = a;
    a = swap;
  }
  for (long i = scene->edgeCount - 1; i >= 0; i--) {
    if (scene->edges[i].a == a && scene->edges[i].b == b) return;
  }
  scene->edges[scene->edgeCount++] = Edge_new(a, b);
}