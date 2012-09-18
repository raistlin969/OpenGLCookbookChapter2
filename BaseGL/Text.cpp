#include "Text.h"
#include <boost/log/trivial.hpp>
#include <GL/glew.h>
#include "Logger.h"
#include <glm/glm.hpp>
#include "GLSLProgram.h"
#include <cstdlib>

using glm::vec4;

Text::Text(GLSLProgram* p) : _program(p)
{
  if(FT_Init_FreeType(&_ft))
  {
    BOOST_LOG_SEV(get_global_log(), error) << "Could not init freetype lib";
  }
  if(FT_New_Face(_ft, "c:\\windows\\fonts\\lucon.ttf", 0, &_face))
  {
    BOOST_LOG_SEV(get_global_log(), error) << "Could not open font";
  }

  //FT_Set_Pixel_Sizes(_face, 0, 12);

  glGenVertexArrays(1, &_vao);
  glGenBuffers(1, &_vbo);
  glBindVertexArray(_vao);
  _atlas = new Atlas(_face, 24, _program);
}

Text::~Text()
{
  delete _atlas;
}

void Text::Render(const char* text, float x, float y, float sx, float sy, vec4& color)
{
  _program->Use();
  const char* p;
  glBindVertexArray(_vao);
  glBindTexture(GL_TEXTURE_2D, _atlas->tex);
  GLint loc = glGetUniformLocation(_program->Handle(), "tex");
  if(loc >= 0)
    glUniform1i(loc, 0);
  else
    BOOST_LOG_SEV(get_global_log(), error) << "Uniform tex not found";
  _program->SetUniform("color", color);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  int text_len = strlen(text);
  Point* coords = new Point[6 * text_len];
  int c = 0;
  for(p = text; *p; p++)
  {
    float x2 = x + _atlas->c[*p].bl * sx;
    float y2 = -y - _atlas->c[*p].bt * sy;
    float w = _atlas->c[*p].bw * sx;
    float h = _atlas->c[*p].bh * sy;

    x += _atlas->c[*p].ax * sx;
    y += _atlas->c[*p].ay * sy;

    if(!w || !h)
      continue;

    coords[c].x = x2; coords[c].y = -y2; coords[c].s = _atlas->c[*p].tx; coords[c++].t = 0;
    coords[c].x = x2 + w; coords[c].y =  -y2; coords[c].s = _atlas->c[*p].tx + _atlas->c[*p].bw / w; coords[c++].t =  0;
    coords[c].x = x2; coords[c].y = -y2 - h; coords[c].s =  _atlas->c[*p].tx; coords[c++].t = _atlas->c[*p].bh / h;
    coords[c].x = x2 + w; coords[c].y =  -y2; coords[c].s = _atlas->c[*p].tx + _atlas->c[*p].bw / w; coords[c++].t = 0;
    coords[c].x = x2; coords[c].y = -y2 - h; coords[c].s = _atlas->c[*p].tx, coords[c++].t = _atlas->c[*p].bh / h;
    coords[c].x = x2 + w; coords[c].y = -y2 - h; coords[c].s = _atlas->c[*p].tx + _atlas->c[*p].bw / w; coords[c++].t = _atlas->c[*p].bh / h;
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * 6 * text_len, coords, GL_DYNAMIC_DRAW);
  
  glDrawArrays(GL_TRIANGLES, 0, c);

  glBindVertexArray(0);
}

Atlas::Atlas(FT_Face face, int height, GLSLProgram* program)
{
  FT_Set_Pixel_Sizes(face, 0, height);
  FT_GlyphSlot g = face->glyph;

  int roww = 0;
  int rowh = 0;
  w = 0;
  h = 0;

  memset(c, 0, sizeof(c));

  //find min size for texture holding all visible ASCII chars
  for(int i = 32; i < 128; ++i)
  {
    if(FT_Load_Char(face, i, FT_LOAD_RENDER))
    {
      BOOST_LOG_SEV(get_global_log(), error) << "Loading character " << char(i) << " failed";
      continue;
    }
    if(roww + g->bitmap.width + 1 >= MAXWIDTH)
    {
      w = std::max(w, roww);
      h += rowh;
      roww = 0;
      rowh = 0;
    }
    roww += g->bitmap.width + 1;
    rowh = std::max(rowh, g->bitmap.rows);
  }

  w = std::max(w, roww);
  h += rowh;

  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  GLint loc = glGetUniformLocation(program->Handle(), "tex");
  if(loc >= 0)
    glUniform1i(loc, 0);
  else
    BOOST_LOG_SEV(get_global_log(), error) << "Uniform tex not found";

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int ox = 0;
  int oy = 0;
  rowh = 0;

  for(int i = 32; i < 128; ++i)
  {
    if(FT_Load_Char(face, i, FT_LOAD_RENDER))
    {
      BOOST_LOG_SEV(get_global_log(), error) << "Loading character " << char(i) << " failed";
      continue;
    }
    if(ox + g->bitmap.width + 1 >= MAXWIDTH)
    {
      oy += rowh;
      rowh = 0;
      ox = 0;
    }
    glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
    c[i].ax = g->advance.x >> 6;
    c[i].ay = g->advance.y >> 6;
    c[i].bw = g->bitmap.width;
    c[i].bh = g->bitmap.rows;
    c[i].bl = g->bitmap_left;
    c[i].bt = g->bitmap_top;
    c[i].tx = ox / (float)w;
    c[i].ty = oy / (float)h;
    
    rowh = std::max(rowh, g->bitmap.rows);
    ox += g->bitmap.width + 1;
  }

  BOOST_LOG_SEV(get_global_log(), info) << "Generated a " << w << " x " << h << " (" << w * h / 1024 << " kb) texture atlas";
}

Atlas::~Atlas()
{
  glDeleteTextures(1, &tex);
}