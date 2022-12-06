#pragma once
#include <gl.hpp>
namespace Resources{
  namespace Shaders{
    extern const char* VertexShader;
    extern const char* FragmentShader;
    GLuint CompileShaders(const char* vertex, const char* fragment);
  }
}