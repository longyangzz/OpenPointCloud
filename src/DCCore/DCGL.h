#ifndef DCCORE_INCLUDE_DCGL_H
#define DCCORE_INCLUDE_DCGL_H

//GLEW (if needed, must be included first)
#include <GL/glew.h>

#include <QtOpenGL/QGLContext>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLFormat>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <gl/glut.h>
//#include <gl/GLAUX.H>
#endif

#ifndef GL_INVALID_TEXTURE_ID
#define GL_INVALID_TEXTURE_ID 0xffffffff
#endif

#endif //DCCORE_INCLUDE_DCGL_H