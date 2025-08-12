#ifndef MATRIX_H_
#define MATRIX_H_

#include "vector.h"

typedef struct {
  float m[4][4];
} mat4_t;

mat4_t mat4_identity(void);
mat4_t mat4_make_scale(float sx, float sy, float sz);
mat4_t mat4_make_translate(float tx, float ty, float tz);
mat4_t mat4_make_rotation_along_x(float radians);
mat4_t mat4_make_rotation_along_y(float radians);
mat4_t mat4_make_rotation_along_z(float radians);
vec4_t mat4_mul_vec4(mat4_t m, vec4_t v);
mat4_t mat4_mul_mat4(mat4_t m1, mat4_t m2);

#endif