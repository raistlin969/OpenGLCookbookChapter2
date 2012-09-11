#include "GLWidgetADS.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

using glm::vec3;
using glm::mat4;

GLWidgetADS::GLWidgetADS(const QGLFormat& format, QWidget* parent)
  : GLWidget(format, parent)
{
}

GLWidgetADS::~GLWidgetADS()
{
}

void GLWidgetADS::initializeGL()
{
  GLWidget::initializeGL();
  _vertex_shader_files.push_back("diffuse_vert.glsl");
  _fragment_shader_files.push_back("diffuse_frag.glsl");
  CompileAndLinkShaders();
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glEnable(GL_DEPTH_TEST);

  _torus = new VboTorus(0.7f, 0.3f, 30, 30);
  _model = mat4(1.0);
  _model *= glm::rotate(-35.0f, vec3(1.0f, 0.0f, 0.0f));
  _model *= glm::rotate(35.0f, vec3(0.0f, 1.0f, 0.0f));
  _view = glm::lookAt(vec3(0.0f, 0.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
  _projection = mat4(1.0f);

  _program.SetUniform("kd", 0.9f, 0.5f, 0.3f);
  _program.SetUniform("ld", 1.0f, 1.0f, 1.0f);
  _program.SetUniform("light_position", _view * vec4(5.0f, 5.0f, 2.0f, 1.0f));
}

void GLWidgetADS::resizeGL( int w, int h )
{
  glViewport(0, 0, w, h);
  _projection = glm::perspective(70.0f, (float)w/h, 0.3f, 100.0f);
}

void GLWidgetADS::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  SetMatrices();
  _torus->Render();
}

void GLWidgetADS::SetMatrices()
{
  glm::mat4 mv = _view * _model;
  _program.SetUniform("model_view_matrix", mv);
  _program.SetUniform("normal_matrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
  _program.SetUniform("mvp", _projection * mv);
}