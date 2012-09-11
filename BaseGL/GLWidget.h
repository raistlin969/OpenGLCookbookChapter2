#pragma once
#include <GL/glew.h>
#include <GL/wglew.h>
#include <QGLWidget>
#include <QtGui/qevent.h>
#include <QTimer>
#include <QElapsedTimer>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <boost/log/trivial.hpp>
#include <vector>
#include <string>
#include "Logger.h"
#include "GLSLProgram.h"

class GLWidget : public QGLWidget
{
  Q_OBJECT
public:
  GLWidget(const QGLFormat& format, QWidget* parent = 0);
  virtual ~GLWidget();

protected:
  typedef void (APIENTRY *_glGenVertexArrays) (GLsizei, GLuint*);
  typedef void (APIENTRY *_glBindVertexArray) (GLuint);
  typedef std::vector<string> ShaderFiles;

protected:
  //Qt functions
  virtual void initializeGL();
  virtual void resizeGL(int w, int h);
  virtual void paintGL();
  virtual void keyPressEvent(QKeyEvent* e);
  
  virtual void FreeResources();
  virtual void SetMatrices();
  virtual void CompileAndLinkShaders();
  
protected:
  void DumpGLInfo(bool dump_extentions = false);

protected slots:
  virtual void Idle();

protected:
  QTimer* _timer;
  QElapsedTimer _elapsed;

  GLuint _vao;

  GLSLProgram _program;
  glm::mat4 _model;
  glm::mat4 _view;
  glm::mat4 _projection;

  ShaderFiles _vertex_shader_files;
  ShaderFiles _fragment_shader_files;

  boost::log::sources::severity_logger<severity_level>& _log;

protected:
  _glGenVertexArrays glGenVertexArrays;
  _glBindVertexArray glBindVertexArray;

};
