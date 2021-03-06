#pragma once

#include "GLWidget.h"
#include "Text.h"
#include "VboTorus.h"
#include "VboTeapot.h"
#include "Camera.h"

class GLWidgetText : public GLWidget
{
  Q_OBJECT
public:
  GLWidgetText(const QGLFormat& format, QWidget* parent = 0);
  virtual ~GLWidgetText();

protected:
  //Qt functions
  virtual void initializeGL();
  virtual void resizeGL(int w, int h);
  virtual void paintGL();
  virtual void keyPressEvent(QKeyEvent* e);
  virtual void SetMatrices();

  Text* _text;
  GLSLProgram _text_program;
  GLSLProgram _ads;
  VboTorus* _torus;
  VBOTeapot* _tea;
  float _angle;
  Camera _camera;
  int _width;
  int _height;
  unsigned int _frames;

protected slots:
  virtual void Idle();
};
