#pragma once

#include "Drawable.h"

class VboTorus : public Drawable
{
private:
  unsigned int _vao;
  int _faces;
  int _rings;
  int _sides;
  float* v;
  float* n;
  float* tex;
  int num_verts;
  unsigned int* el;
  unsigned int handle[4];

private:
  void GenerateVerts(float*, float*, float*, unsigned int*, float, float);

public:
  VboTorus(float, float, int, int);
  void Render();
  void Bind();
};