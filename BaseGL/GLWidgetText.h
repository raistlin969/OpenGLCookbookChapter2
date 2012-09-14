#pragma once

#include "GLWidget.h"
#include <ft2build.h>
#include FT_FREETYPE_H

struct Point
{
  GLfloat x;
  GLfloat y;
  GLfloat s;
  GLfloat t;
};

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

  void RenderText(const char* text, float x, float y, float sx, float sy);
  inline int NextP2(int a)
  {
    int rval = 1;
    while(rval < a)
      rval <<= 1;
    return rval;
  }

protected:
  FT_GlyphSlot _g;
  FT_Library _ft;
  FT_Face _face;

protected slots:
  virtual void Idle();
};
