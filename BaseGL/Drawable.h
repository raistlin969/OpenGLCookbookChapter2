#pragma once
#include <glm/glm.hpp>

class GLSLProgram;

class Drawable
{
protected:
  glm::mat4 _model;
  glm::mat4 _rot;
  glm::mat4 _trans;
  glm::vec3 _position;
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
  void Translate(glm::vec3& v);
  void SetPosition(glm::vec3& v);
  void Move(glm::vec3& v);
  virtual void Render() = 0;
};