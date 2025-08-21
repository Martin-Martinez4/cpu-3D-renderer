#ifndef CLIPPING_H_
#define CLIPPING_H_

#include "vector.h"

enum {
  LEFT_FRUSTUM_PLANE,
  RIGHT_FRUSTUM_PLANE,
  TOP_FRUSTUM_PLANE,
  BOTTOM_FRUSTUM_PLANE,
  NEAR_FRUSTUM_PLANE,
  FAR_FRUSTUM_PLANE
};

typedef struct{
  vec3_t point;
  vec3_t normal;
} plane_t;

#endif