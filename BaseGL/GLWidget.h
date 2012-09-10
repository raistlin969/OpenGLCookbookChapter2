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

protected:
  virtual void initializeGL();
  virtual void resizeGL(int w, int h);
  virtual void paintGL();

  virtual void keyPressEvent(QKeyEvent* e);
  
protected:
  void FreeResources();
  void DumpGLInfo(bool dump_extentions = false);

protected slots:
  void Idle();

private:
  QTimer* _timer;
  QElapsedTimer _elapsed;
  GLint _mvp;
  GLuint _vao;

  GLSLProgram _program;

  boost::log::sources::severity_logger<severity_level>& _log;

protected:
  _glGenVertexArrays glGenVertexArrays;
  _glBindVertexArray glBindVertexArray;

};
