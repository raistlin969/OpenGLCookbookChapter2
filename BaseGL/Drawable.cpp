#include "Drawable.h"
#include "GLSLProgram.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

Drawable::Drawable()
{
  _model = glm::mat4(1.0f);
}

Drawable::~Drawable()
{
}

void Drawable::Program(GLSLProgram* p)
{
  _program = p;
}

GLSLProgram* Drawable::Program()
{
  return _program;
}

void Drawable::Model(glm::mat4& m)
{
  _model = m;
}

glm::mat4& Drawable::Model()
{
  return _model;
}

void Drawable::Rotate(float angle, glm::vec3& v)
{
  _model *= glm::rotate(angle, v);
}