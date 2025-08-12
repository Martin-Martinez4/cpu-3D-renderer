#ifndef MESH_H_
#define MESH_H_

#include <vector.h>
#include <triangle.h>
#include "matrix.h"

#define N_MESH_VERTS 8
extern vec3_t mesh_vertices[N_MESH_VERTS];

#define N_MESH_FACES (6 * 2)
extern face_t mesh_faces[N_MESH_FACES];

typedef struct {
  vec3_t* vertices;
  face_t* faces;
  vec3_t rotation;
  vec3_t scale;
  vec3_t translation;
  mat4_t transformations;
} mesh_t;

extern mesh_t mesh;

void load_cube_mesh_data(void);
void load_obj_file_data(char* filename);

#endif