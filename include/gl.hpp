#pragma once
//SO FUN FACT
//MAC USES "OpenGL" FOR INCLUDES, INSTEAD OF GL

#ifdef __APPLE__
#include <windows.h>
#include <OpenGL/gl.h>
#else
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
//#include "f:\OpenGL\glut.h"
#endif