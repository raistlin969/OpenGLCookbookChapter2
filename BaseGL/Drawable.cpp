#include "Drawable.h"
#include "GLSLProgram.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

Drawable::Drawable()
{
  _model = glm::mat4(1.0f);
  _rot = glm::mat4(1.0f);
  _trans = glm::mat4(1.0f);
  _position = glm::vec3(0.0f, 0.0f, 0.0f);
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
  _trans = glm::translate(mat4(1.0f), _position);
  _model = _trans * _rot * _model;
  return _model;
}

void Drawable::Rotate(float angle, glm::vec3& v)
{
  //_rot = mat4(1.0f);
  _rot *= glm::rotate(angle, v);
}

void Drawable::Translate(glm::vec3& v)
{
  _trans = mat4(1.0f);
  _trans *= glm::translate(mat4(1.0f), v);
}

void Drawable::SetPosition(glm::vec3& v)
{
  _position = v;
}

void Drawable::Move(glm::vec3& v)
{
  _position += v;
}