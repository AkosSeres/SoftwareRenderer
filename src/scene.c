#include <scene.h>

void Scene_erase(Scene* scene) {
  scene->vertices = NULL;
  scene->projectedPoints = NULL;
  scene->edges = NULL;
  scene->edgeCount = 0;
  scene->verticesCount = 0;
}

void Scene_setCamera(Scene* scene, Camera cam) { scene->cam = cam; }

void Scene_projectPoints(Scene* scene) {
  for (long int i = 0; i < scene->verticesCount; i++) {
    (scene->projectedPoints)[i] =
        Camera_projectLinear(&(scene->cam), &((scene->vertices)[i]));
  }
}

Edge Edge_new(long int a, long int b) {
  Edge retVal;
  retVal.a = a;
  retVal.b = b;
  return retVal;
}

void Scene_loadObj(Scene* scene, const char* fileName) {
  FILE* filePointer;
  filePointer = fopen(fileName, "r");
  if (filePointer == NULL) return;
  char line[256];

  long allocatedVertices = 512;
  scene->vertices = (Vec3*)malloc(allocatedVertices * sizeof(Vec3));
  scene->verticesCount = 0;

  long allocatedEdges = 1024;
  scene->edges = (Edge*)malloc(allocatedEdges * sizeof(Edge));
  scene->edgeCount = 0;

  while (fgets(line, sizeof(line), filePointer) != NULL) {
    if (line[0] == '\0' || line[0] == '\n') {
      continue;
    }
    if (line[0] == 'v' && line[1] == ' ') {
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
      x = strtod(currPtr, &nextPtr);
      currPtr = nextPtr;
      y = strtod(currPtr, &nextPtr);
      currPtr = nextPtr;
      z = strtod(currPtr, &nextPtr);
      scene->vertices[scene->verticesCount++] = Vec3_new(x, y, z);
    } else if (line[0] == 'f' && line[1] == ' ') {
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
      readVertexNumbers(&(line[2]), vertexNumbers, &vCount);
      for (int i = 1; i < vCount; i++) {
        scene->edges[scene->edgeCount++] =
            Edge_new(vertexNumbers[i - 1] - 1, vertexNumbers[i] - 1);
      }
      scene->edges[scene->edgeCount++] =
          Edge_new(vertexNumbers[0] - 1, vertexNumbers[vCount - 1] - 1);
    } else if (line[0] == 'l' && line[1] == ' ') {
      // Read polyline
    }
  }

  scene->projectedPoints = (Point*)malloc(allocatedVertices * sizeof(Point));

  fclose(filePointer);
}

void Scene_free(Scene* scene) {
  free(scene->vertices);
  free(scene->edges);
  free(scene->projectedPoints);
  Scene_erase(scene);
}

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