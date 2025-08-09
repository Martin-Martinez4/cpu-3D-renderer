#ifndef MESH_H_
#define MESH_H_

#include <vector.h>
#include <triangle.h>

#define N_MESH_VERTS 8
extern vec3_t mesh_vertices[N_MESH_VERTS];

#define N_MESH_FACES (6 * 2)
extern face_t mesh_faces[N_MESH_FACES];

#endif