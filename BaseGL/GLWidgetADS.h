#pragma once

#include "GLWidget.h"
#include "VboTorus.h"

class GLWidgetADS : public GLWidget
{
  Q_OBJECT
public:
  GLWidgetADS(const QGLFormat& format, QWidget* parent = 0);
  virtual ~GLWidgetADS();

protected:
  //Qt functions
  virtual void initializeGL();
  virtual void resizeGL(int w, int h);
  virtual void paintGL();
  virtual void SetMatrices();

protected:
  VboTorus* _torus;
};
