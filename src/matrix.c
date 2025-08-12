#include <math.h>
#include "matrix.h"

mat4_t mat4_identity(void){

  mat4_t m = {{
    { 1, 0, 0, 0 },
    { 0, 1, 0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 },
  }};
  return m;
}
mat4_t mat4_make_scale(float sx, float sy, float sz){
  mat4_t m = mat4_identity();
  m.m[0][0] = sx;
  m.m[1][1] = sy;
  m.m[2][2] = sz;

  return m;
}

mat4_t mat4_make_translate(float tx, float ty, float tz){
  mat4_t m = mat4_identity();
  m.m[0][3] = tx;
  m.m[1][3] = ty;
  m.m[2][3] = tz;

  return m;
}
mat4_t mat4_make_rotation_along_x(float radians){
  mat4_t m = {{
    { 1, 0, 0, 0 },
    { 0, cos(radians), -sin(radians), 0 },
    { 0, sin(radians), cos(radians), 0 },
    { 0, 0, 0, 1 },
  }};
  return m;
}
mat4_t mat4_make_rotation_along_y(float radians){
 mat4_t m = {{
    { cos(radians), 0, sin(radians), 0 },
    { 0, 1, 0, 0 },
    { -sin(radians), 0, cos(radians), 0 },
    { 0, 0, 0, 1 },
  }};
  return m;
}
mat4_t mat4_make_rotation_along_z(float radians){
 mat4_t m = {{
    { cos(radians), -sin(radians), 0, 0 },
    { sin(radians), cos(radians), 0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 },
  }};
  return m;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v){
  vec4_t result;
  result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
  result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
  result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
  result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;

  return result;
}

mat4_t mat4_mul_mat4(mat4_t m1, mat4_t m2){
   mat4_t m = {{
    { 
      m1.m[0][0]*m2.m[0][0] + m1.m[0][1]*m2.m[1][0] + m1.m[0][2]*m2.m[2][0] + m1.m[0][3]*m2.m[3][0],
      m1.m[0][0]*m2.m[0][1] + m1.m[0][1]*m2.m[1][1] + m1.m[0][2]*m2.m[2][1] + m1.m[0][3]*m2.m[3][1],
      m1.m[0][0]*m2.m[0][2] + m1.m[0][1]*m2.m[1][2] + m1.m[0][2]*m2.m[2][2] + m1.m[0][3]*m2.m[3][2],
      m1.m[0][0]*m2.m[0][3] + m1.m[0][1]*m2.m[1][3] + m1.m[0][2]*m2.m[2][3] + m1.m[0][3]*m2.m[3][3],
    },
    { 
      m1.m[1][0]*m2.m[0][0] + m1.m[1][1]*m2.m[1][0] + m1.m[1][2]*m2.m[2][0] + m1.m[1][3]*m2.m[3][0],
      m1.m[1][0]*m2.m[0][1] + m1.m[1][1]*m2.m[1][1] + m1.m[1][2]*m2.m[2][1] + m1.m[1][3]*m2.m[3][1],
      m1.m[1][0]*m2.m[0][2] + m1.m[1][1]*m2.m[1][2] + m1.m[1][2]*m2.m[2][2] + m1.m[1][3]*m2.m[3][2],
      m1.m[1][0]*m2.m[0][3] + m1.m[1][1]*m2.m[1][3] + m1.m[1][2]*m2.m[2][3] + m1.m[1][3]*m2.m[3][3], },
    { 
      m1.m[2][0]*m2.m[0][0] + m1.m[2][1]*m2.m[1][0] + m1.m[2][2]*m2.m[2][0] + m1.m[2][3]*m2.m[3][0],
      m1.m[2][0]*m2.m[0][1] + m1.m[2][1]*m2.m[1][1] + m1.m[2][2]*m2.m[2][1] + m1.m[2][3]*m2.m[3][1],
      m1.m[2][0]*m2.m[0][2] + m1.m[2][1]*m2.m[1][2] + m1.m[2][2]*m2.m[2][2] + m1.m[2][3]*m2.m[3][2],
      m1.m[2][0]*m2.m[0][3] + m1.m[2][1]*m2.m[1][3] + m1.m[2][2]*m2.m[2][3] + m1.m[2][3]*m2.m[3][3], 
    },
    { 
      m1.m[3][0]*m2.m[0][0] + m1.m[3][1]*m2.m[1][0] + m1.m[3][2]*m2.m[2][0] + m1.m[3][3]*m2.m[3][0],
      m1.m[3][0]*m2.m[0][1] + m1.m[3][1]*m2.m[1][1] + m1.m[3][2]*m2.m[2][1] + m1.m[3][3]*m2.m[3][1],
      m1.m[3][0]*m2.m[0][2] + m1.m[3][1]*m2.m[1][2] + m1.m[3][2]*m2.m[2][2] + m1.m[3][3]*m2.m[3][2],
      m1.m[3][0]*m2.m[0][3] + m1.m[3][1]*m2.m[1][3] + m1.m[3][2]*m2.m[2][3] + m1.m[3][3]*m2.m[3][3],  
    },
  }};
  return m;
}