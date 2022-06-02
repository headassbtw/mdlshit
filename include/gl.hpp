#pragma once
//SO FUN FACT
//MAC USES "OpenGL" FOR INCLUDES, INSTEAD OF GL

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif