#pragma once

class Drawable
{
public:
  Drawable() {}

  virtual void Render() = 0;
};