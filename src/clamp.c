#include "clamp.h"

// Maybe should be a macro?

int clamp(int val, int min, int max){
  const int t = val < min ? min : val;
  return t > max ? max : t;
}

float clamp_float(float val, float min, float max){
  const float t = val < min ? min : val;
  return t > max ? max : t;
}
