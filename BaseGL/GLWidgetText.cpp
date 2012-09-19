#include "GLWidgetText.h"
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include FT_GLYPH_H

GLWidgetText::GLWidgetText(const QGLFormat& format, QWidget* parent)
  : GLWidget(format, parent)
{
  _angle = 0.0f;
  _frames = 0;
}

GLWidgetText::~GLWidgetText()
{
}

void GLWidgetText::initializeGL()
{
  GLWidget::initializeGL();
  _text_program.CompileAndLinkShaders("text_vert.glsl", "text_frag.glsl");
  _ads.CompileAndLinkShaders("diffuse_vert.glsl", "diffuse_frag.glsl");
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glEnable(GL_DEPTH_TEST);
  _text = new Text(&_text_program);

  _torus = new VboTorus(0.7f, 0.3f, 50, 50);
  _torus->Model(mat4(1.0));
  _torus->Rotate(-35.0f, vec3(1.0f, 0.0f, 0.0f));
  _torus->Rotate(35.0f, vec3(0.0f, 1.0f, 0.0f));
  _view = glm::lookAt(vec3(0.0f, 0.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
  _projection = mat4(1.0f);
  vec4 world_light = vec4(5.0f, 5.0f, 2.0f, 1.0f);

  _ads.SetUniform("material.kd", 0.9f, 0.5f, 0.3f);
  _ads.SetUniform("light.ld", 1.0f, 1.0f, 1.0f);
  _ads.SetUniform("light.position", _view * world_light);
  _ads.SetUniform("material.ka", 0.9f, 0.5f, 0.3f);
  _ads.SetUniform("light.la", 0.4f, 0.4f, 0.4f);
  _ads.SetUniform("material.ks", 0.8f, 0.8f, 0.8f);
  _ads.SetUniform("light.ls", 1.0f, 1.0f, 1.0f);
  _ads.SetUniform("material.shininess", 100.0f);
}

void GLWidgetText::resizeGL( int w, int h )
{
  glViewport(0, 0, w, h);
  _width = w;
  _height = h;
  _projection = glm::perspective(70.0f, (float)w/h, 0.3f, 100.0f);
}

void GLWidgetText::paintGL()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  _ads.Use();
  _torus->Model(mat4(1.0));
  _torus->Rotate(_angle, vec3(0.0f, 1.0f, 0.0f));
  _torus->Rotate(-35.0f, vec3(1.0f, 0.0f, 0.0f));
  _torus->Rotate(35.0f, vec3(0.0f, 1.0f, 0.0f));
  SetMatrices();
  _torus->Render();
  glUseProgram(0);
  float sx = 2.0 / this->width();
  float sy = 2.0 / this->height();
  vec4 transparent_green(0.0f, 1.0f, 0.0f, 0.5f);
  _frames++;
  float fps = _frames /(_elapsed.elapsed() / 1000.0);
  std::ostringstream out;
  out << "FPS: " << fps;
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  _text_program.Use();
  _text->Render(out.str().c_str(), -1.0 , 0.9 , sx, sy, transparent_green);
  glUseProgram(0);
}


void GLWidgetText::Idle()
{
  _angle += 1.0f;
  if(_angle > 360.0)
    _angle -= 360.0;

  updateGL();
}

void GLWidgetText::SetMatrices()
{
  glm::mat4 mv = _view * _torus->Model();
  _ads.SetUniform("model_view_matrix", mv);
  _ads.SetUniform("normal_matrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
  _ads.SetUniform("mvp", _projection * mv);
}
