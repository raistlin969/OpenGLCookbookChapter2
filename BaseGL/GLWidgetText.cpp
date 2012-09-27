#include "GLWidgetText.h"
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <QtCore/qcoreapplication.h>

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
  _ads.CompileAndLinkShaders("front_back_vert.glsl", "front_back_frag.glsl");
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glEnable(GL_DEPTH_TEST);
  _text = new Text(&_text_program);

  _torus = new VboTorus(0.7f, 0.3f, 50, 50);
  _torus->Program(&_ads);
  _torus->Model(mat4(1.0));
  //_torus->Rotate(-35.0f, vec3(1.0f, 0.0f, 0.0f));
  //_torus->Rotate(35.0f, vec3(0.0f, 1.0f, 0.0f));
  _torus->SetPosition(glm::vec3(0.0, 0.0, 0.0));
  mat4 transform = glm::translate(mat4(1.0f), vec3(0.0f, 1.5f, 0.25f));
  _tea = new VBOTeapot(13, transform);
  //_tea->Translate(glm::vec3(-5.0f, 0.0f, 0.0f));
  _view = _camera.View();
  _projection = _camera.Projection();
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
  _torus->Rotate(1.0f, vec3(1.0f, 1.0f, 1.0f));
//  _torus->Rotate(-35.0f, vec3(1.0f, 0.0f, 0.0f));
//  _torus->Rotate(35.0f, vec3(0.0f, 1.0f, 0.0f));
//  _torus->Translate(vec3(-1.0, 0.0, 0.0));
  //_torus->Move(glm::vec3(0.01, 0.0, 0.0));
  //mat4 rot = glm::rotate(-35.0f, vec3(1.0f, 0.0f, 0.0f));
  //rot *= glm::rotate(35.0f, vec3(0.0f, 1.0f, 0.0f));
  //mat4 tran = glm::translate(mat4(1.0f), vec3(-1.0, 0.0, 0.0));
  //_torus->Model(tran * rot);
  SetMatrices();
  _torus->Render();

  vec4 world_light = vec4(2.0f, 4.0f, 2.0f, 1.0f);
  mat4 model = glm::rotate(_angle, vec3(0.0f, 1.0f, 0.0f));
  _ads.SetUniform("light.position", _view * model * world_light);

  //model = mat4(1.0f);
  //model *= glm::translate(vec3(0.0f, -1.0f, 0.0f));
  //_tea->Model(model);
  //_tea->Translate(glm::vec3(5.0f, 0.0f, 0.0f));
  //_tea->Rotate(-90.0f, vec3(1.0f, 0.0f, 0.0f)); 
  //SetMatrices();
  //_tea->Render();
  glUseProgram(0);
  float sx = 2.0 / this->width();
  float sy = 2.0 / this->height();
  vec4 transparent_green(0.0f, 1.0f, 0.0f, 0.5f);
  _frames++;
  float fps = _frames /(_elapsed.elapsed() / 1000.0);
  std::ostringstream out;
  out << "FPS: " << fps;
  _text_program.Use();
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //_text->Render(out.str().c_str(), -1.0 , 0.9 , sx, sy, transparent_green);
  //glDisable(GL_BLEND);
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
  glm::mat4 mv = _camera.View() * _torus->Model();
  _ads.SetUniform("model_view_matrix", mv);
  _ads.SetUniform("normal_matrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
  _ads.SetUniform("mvp", _projection * mv);
}

void GLWidgetText::keyPressEvent( QKeyEvent* e )
{
  switch ( e->key() )
  {
    case Qt::Key_Escape:
      QCoreApplication::instance()->quit();
        break;
    case Qt::Key::Key_A:
      _camera.StrafeLeft();
      break;
    case Qt::Key::Key_D:
      _camera.StrafeRight();
      break;
    case Qt::Key::Key_W:
      _camera.MoveForward();
      break;
    case Qt::Key::Key_S:
      _camera.MoveBack();
      break;
    case Qt::Key::Key_Q:
      _camera.DeltaBearing(-0.1f);
      break;
    case Qt::Key::Key_E:
      _camera.DeltaBearing(0.1f);
      break;
    case Qt::Key::Key_Z:
      _camera.DeltaPitch(-0.1f);
      break;
    case Qt::Key::Key_C:
      _camera.DeltaPitch(0.1f);
      break;
    default:
      QGLWidget::keyPressEvent( e );
    }
}