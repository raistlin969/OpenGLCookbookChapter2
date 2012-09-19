#pragma once
#include <glm/glm.hpp>

class GLSLProgram;

class Drawable
{
protected:
  glm::mat4 _model;
  GLSLProgram* _program;
  unsigned int _vao;

public:
  Drawable();
  virtual ~Drawable();
  void Program(GLSLProgram* p);
  GLSLProgram* Program();
  void Model(glm::mat4& m);
  glm::mat4& Model();

  void Rotate(float angle, glm::vec3& v);
  virtual void Render() = 0;
};