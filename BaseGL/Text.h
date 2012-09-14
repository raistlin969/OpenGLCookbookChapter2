#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

class Text
{
protected:
  unsigned int _vao;
  FT_Library _ft;
  FT_Face _face;

public:
  Text();
  virtual ~Text();
  void Render(const char* text, float x, float y, float sx, float sy);
};