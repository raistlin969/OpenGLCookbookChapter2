#pragma once
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>

#define MAXWIDTH 1024

using glm::vec4;

class GLSLProgram;

struct Point
{
  float x;
  float y;
  float s;
  float t;
};

struct Atlas
{
  GLuint tex;
  int w;
  int h;
  struct
  {
    float ax; //advance x
    float ay; //advance y
    float bw; //bitmap.width
    float bh; //bitmap.height
    float bl; //bitmap.left
    float bt; //bitmap.top
    float tx; //x offset of glyph in texture coordinates
    float ty;
  } c[128]; //character info

  Atlas(FT_Face face, int height, GLSLProgram* program);
  ~Atlas();
};

class Text
{
protected:
  GLuint _vao;
  GLuint _vbo;
  FT_Library _ft;
  FT_Face _face;
  GLSLProgram* _program;
  Atlas* _atlas;

public:
  Text(GLSLProgram* p);
  virtual ~Text();
  void Render(const char* text, float x, float y, float sx, float sy, vec4& color);
};