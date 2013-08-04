
#include <stdio.h>
#include <unistd.h>
#include <directfb.h>


#include <EGL/egl.h>
#include <GLES/gl.h>

#include "common.h"

#define GL_BLEND_BIT (1<<1)
#define GL_TEXTURE_2D_BIT (1<<2)

static unsigned char m_color_clear_r = 0;
static unsigned char m_color_clear_g = 0;
static unsigned char m_color_clear_b = 0;
static unsigned char m_color_clear_a = 0;

extern IDirectFB             *m_dfb;
extern IDirectFBSurface      *m_mainwindow;

typedef struct {
  void               *addr;
  /* unsigned */ int  pitch;
} CoreSurfaceBufferLock;

typedef struct {
IDirectFBSurface *surface;
//EGLDisplay display;
} EGLTexture;

static EGLTexture   egl_textures[256];
static int          texturesCurrent = 0;
static unsigned int texturesActive = GL_TEXTURE0;

static int m_bound_texture;
static unsigned int m_drawflags = 0;

GLvoid  *g_glColorPointer       = NULL;
GLsizei  g_glColorPointerStride = 0;
GLint    g_glColorPointerSize   = 0;
GLenum   g_glColorPointerType   = 0;
GLint    g_glColorPointerEnabled = 0;

GLvoid  *g_glTexCoordPointer       = NULL;
GLsizei  g_glTexCoordPointerStride = 0;
GLint    g_glTexCoordPointerSize   = 0;
GLenum   g_glTexCoordPointerType   = 0;
GLint    g_glTexCoordPointerEnabled = 0;

GLvoid  *g_glVertexPointer       = NULL;
GLsizei  g_glVertexPointerStride = 0;
GLint    g_glVertexPointerSize   = 0;
GLenum   g_glVertexPointerType   = 0;
GLint    g_glVertexPointerEnabled = 0;

void glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
#ifdef DEBUG
  printf("%s:%s[%d] size=%d stride=%u \n", __FILE__, __func__, __LINE__, size, stride);
#endif
  g_glColorPointerSize   = size;
  g_glColorPointerType   = type;
  if(stride)
    g_glColorPointerStride = stride;
  else
    g_glColorPointerStride = size * sizeof(unsigned char);
  g_glColorPointer       = pointer;
}

void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
#ifdef DEBUG
  printf("%s:%s[%d] size=%d stride=%u \n", __FILE__, __func__, __LINE__, size, stride);
#endif
  g_glTexCoordPointerSize   = size;
  g_glTexCoordPointerType   = type;
  if(stride)
    g_glTexCoordPointerStride = stride;
  else
    g_glTexCoordPointerStride = size * sizeof(GLfloat);
  g_glTexCoordPointer       = pointer;
}

void glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
#ifdef DEBUG
  printf("%s:%s[%d] size=%d stride=%u \n", __FILE__, __func__, __LINE__, size, stride);
#endif
  g_glVertexPointerSize   = size;
  g_glVertexPointerType   = type;
  if(stride)
    g_glVertexPointerStride = stride;
  else
    g_glVertexPointerStride = size * sizeof(GLfloat);
  g_glVertexPointer       = pointer;
}

void glEnableClientState(GLenum array)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif
  switch(array)
  {
    case GL_COLOR_ARRAY:         g_glColorPointerEnabled    = 1; break;
    case GL_TEXTURE_COORD_ARRAY: g_glTexCoordPointerEnabled = 1; break;
    case GL_VERTEX_ARRAY:        g_glVertexPointerEnabled   = 1; break;
    default: break;
  }
}
void glDisableClientState(GLenum array)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif
  switch(array)
  {
    case GL_COLOR_ARRAY:         g_glColorPointerEnabled    = 0; break;
    case GL_TEXTURE_COORD_ARRAY: g_glTexCoordPointerEnabled = 0; break;
    case GL_VERTEX_ARRAY:        g_glVertexPointerEnabled   = 0; break;
    default: break;
  }
}





void glGenTextures (GLsizei n, GLuint *textures)
{
  texturesCurrent++;
  texturesCurrent %= 256;
  if (texturesCurrent == 0)
    texturesCurrent++; /* 0 is reserved*/

  while(1)
  {
    if (egl_textures[texturesCurrent].surface == NULL)
      break;

    texturesCurrent++;
    texturesCurrent %= 256;
    if (texturesCurrent == 0)
      texturesCurrent++; /* 0 is reserved*/
  }
  *textures = texturesCurrent;
#ifdef DEBUG
  printf("%s:%s[%d] n=%d texture=%d\n", __FILE__, __func__, __LINE__, n, *textures);
#endif
}

void glDeleteTextures (GLsizei n, GLuint *textures)
{
#ifdef DEBUG
  printf("%s:%s[%d] -> n=%d texture=%d p=%p\n", __FILE__, __func__, __LINE__, n, *textures, (void*)egl_textures[*textures].surface );
#endif

  if (egl_textures[*textures].surface == NULL) return;
  
  egl_textures[*textures].surface->Release(egl_textures[*textures].surface);
  
  egl_textures[*textures].surface = NULL;
#ifdef DEBUG
  printf("%s:%s[%d] <-\n", __FILE__, __func__, __LINE__);
#endif
  //*textures = 0;
}

void glBindTexture (GLenum target, GLuint texture)
{
#ifdef DEBUG
  printf("%s:%s[%d] ---------------- texture=%d (%p)\n", __FILE__, __func__, __LINE__, texture, egl_textures[texture].surface);
#endif
  //if (target == GL_TEXTURE_2D)
  m_bound_texture = texture;
}


GLboolean glIsTexture (GLuint texture) {
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif
  return egl_textures[texture].surface == NULL?GL_FALSE:GL_TRUE;
}

//Not implemented
void glTexParameteri (GLenum target, GLenum pname, GLint param)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif
  return;
}

void glActiveTexture(GLenum texture)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif
  texturesActive = texture;
}


EGLint g_pixelformat = -1;

//Hardcore translateion
void glTexImage2D (GLenum target, GLint level, GLint internalformat, 
                   GLsizei width, GLsizei height, GLint border, GLenum format, 
                   GLenum type, const GLvoid *pixels)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif

  IDirectFBSurface * surface = NULL;
  EGLSurface egl_surface = NULL;
  DFBResult err = 0;
  DFBSurfaceDescription dsc;
  CoreSurfaceBufferLock lock;

  dsc.flags  = (DFBSurfaceDescriptionFlags) (DSDESC_WIDTH | DSDESC_HEIGHT | 
                                             DSDESC_PIXELFORMAT | DSDESC_CAPS);
  dsc.caps   = DSCAPS_PREMULTIPLIED;
  dsc.width  = width;
  dsc.height = height;

  dsc.pixelformat = DSPF_ARGB;

  m_dfb->CreateSurface(
      m_dfb, 
      &dsc, 
      &surface);

#if 0
  surface->Lock (surface, DSLF_WRITE, &lock.addr, &lock.pitch);
  memcpy(lock.addr, pixels, width*height*4);
  surface->Unlock (surface);
#else
  DFBRectangle rect;
  rect.x = 0;
  rect.y = 0;
  rect.w = width;
  rect.h = height;

  surface->Write(surface, &rect, pixels, 4*width);
#endif
  egl_textures[m_bound_texture].surface = surface;
  
#ifdef DEBUG
  printf("%s:%s[%d] ---------------- texture=%d (%p)\n", __FILE__, __func__, __LINE__, texturesCurrent, egl_textures[texturesCurrent].surface);
#endif
}

void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, 
                     GLint yoffset, GLsizei width, GLsizei height, 
                     GLenum format, GLenum type, const GLvoid * pixels)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif
  glTexImage2D(target, level, 0, width, height, 0, format, type, pixels);

}


void glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  int i, j;
  
#ifdef DEBUG
  printf("%s:%s[%d] ---------------------------------\n", __FILE__, __func__, __LINE__);
  printf("%s:%s[%d] first=%d count=%u\n", __FILE__, __func__, __LINE__, first, count);
#endif

  if ((m_drawflags & GL_BLEND_BIT) == GL_BLEND_BIT)
    m_mainwindow->SetPorterDuff (m_mainwindow, DSPD_SRC_OVER);

  if (mode == GL_QUADS)
  {
    /*
    | [1] [3]
    | [0] [2]
    |_______
    */
    
    if (!count) return;
    
    if(g_glVertexPointerEnabled == 0) return;
    
    unsigned char *glColorPointerItr    = g_glColorPointer;
    GLfloat       *glVertexPointerItr   = g_glVertexPointer;
    GLfloat       *glTexCoordPointerItr = g_glTexCoordPointer;
    DFBSurfaceBlittingFlags dsbf;
    unsigned char r=0, g=0, b=0, a=0;

    for (i=first/*0*/; i < count; i+=4 /*4 Coordinates*/)
    {
      dsbf = DSBLIT_BLEND_ALPHACHANNEL;

      //COLOR TODO: OTHER FORMATS AS U8
      if (g_glColorPointerEnabled)
      {
#ifdef DEBUG
        printf("\t[%d] r=%u g=%u b=%u a=%u\n", i, glColorPointerItr[0], glColorPointerItr[1],
                                                  glColorPointerItr[2], glColorPointerItr[3]);
#endif
        r = glColorPointerItr[0];
        g = glColorPointerItr[1];
        b = glColorPointerItr[2];
        a = glColorPointerItr[3];

        if ((m_drawflags & GL_BLEND_BIT) == GL_BLEND_BIT)
        {
          if (a != 0xff)
            dsbf |= DSBLIT_BLEND_COLORALPHA;
        }
        else
          dsbf = DSBLIT_NOFX;

        if (r != 0xff || g != 0xff || 
            b != 0xff || a != 0xff)
          dsbf |= DSBLIT_COLORIZE;

        if ((dsbf & DSBLIT_BLEND_COLORALPHA) == DSBLIT_BLEND_COLORALPHA || (dsbf & DSBLIT_COLORIZE) == DSBLIT_COLORIZE)
          m_mainwindow->SetColor (m_mainwindow, r, g, b, a);

#ifdef DEBUG
        glColorPointerItr += g_glColorPointerStride;
        
        printf("\t[%d] r=%u g=%u b=%u a=%u\n", i+1, glColorPointerItr[0], glColorPointerItr[1],
                                                  glColorPointerItr[2], glColorPointerItr[3]);
        glColorPointerItr += g_glColorPointerStride;
        
        printf("\t[%d] r=%u g=%u b=%u a=%u\n", i+2, glColorPointerItr[0], glColorPointerItr[1],
                                                  glColorPointerItr[2], glColorPointerItr[3]);
        glColorPointerItr += g_glColorPointerStride;
        
        printf("\t[%d] r=%u g=%u b=%u a=%u\n", i+3, glColorPointerItr[0], glColorPointerItr[1],
                                                  glColorPointerItr[2], glColorPointerItr[3]);
        glColorPointerItr += g_glColorPointerStride;
#else
        glColorPointerItr += 4 * g_glColorPointerStride;
#endif
      }
      
      
      //POSITIONS TODO: OTHER FORMATS AS FLOAT
      /* LEGACY
      GLfloat points[] = { -640.0f+glVertexPointerItr[i].x, -(-360.0f+m_vertex[i+1].y), 0.0f,
                         -640.0f+glVertexPointerItr[i+2].x, -(-360.0f+m_vertex[i+3].y), 0.0f,
                         -640.0f+glVertexPointerItr[i+3].x, -(-360.0f+m_vertex[i+2].y), 0.0f,
                         -640.0f+glVertexPointerItr[i+1].x, -(-360.0f+m_vertex[i+0].y), 0.0f };
      */
      
      GLfloat points[12];
      memset(points, 0, 12*sizeof(GLfloat));

#ifdef DEBUG
      printf("\t[%d] x=%f y=%f z=%f\n", i, glVertexPointerItr[0], glVertexPointerItr[1],
                                           glVertexPointerItr[2]);
#endif
      points[ 0] = glVertexPointerItr[0]/*x*/;
      points[10] = glVertexPointerItr[1]/*y*/;
      glVertexPointerItr = ((unsigned char*)glVertexPointerItr) + g_glVertexPointerStride;

#ifdef DEBUG
      printf("\t[%d] x=%f y=%f z=%f\n", i+1, glVertexPointerItr[0], glVertexPointerItr[1],
                                           glVertexPointerItr[2]);
#endif
      points[ 1] = glVertexPointerItr[1]/*y*/;
      points[ 9] = glVertexPointerItr[0]/*x*/;
      glVertexPointerItr = ((unsigned char*)glVertexPointerItr) + g_glVertexPointerStride;
      
#ifdef DEBUG
      printf("\t[%d] x=%f y=%f z=%f\n", i+2, glVertexPointerItr[0], glVertexPointerItr[1],
                                           glVertexPointerItr[2]);
#endif
      points[ 3] = glVertexPointerItr[0]/*x*/;
      points[ 7] = glVertexPointerItr[1]/*y*/;
      glVertexPointerItr = ((unsigned char*)glVertexPointerItr) + g_glVertexPointerStride;
      
#ifdef DEBUG
      printf("\t[%d] x=%f y=%f z=%f\n", i+3, glVertexPointerItr[0], glVertexPointerItr[1],
                                           glVertexPointerItr[2]);
#endif
      points[ 4] = glVertexPointerItr[1]/*y*/;
      points[ 6] = glVertexPointerItr[0]/*x*/;
      glVertexPointerItr = ((unsigned char*)glVertexPointerItr) + g_glVertexPointerStride;


#ifdef DRAWDEBUG
      for (j=0; j<12; j+=3)
        printf("\t\t[%d] x=%f y=%f z=%f\n", j/3, points[j], points[j+1], points[j+2]);
#endif

      DFBRectangle dstRectangle;
      dstRectangle.w = points[6] - points[0];
      dstRectangle.h = -(points[7] - points[1]);
      dstRectangle.x = points[0];
      dstRectangle.y = points[7];

      DFBRectangle srcRectangle;
      int w, h;
      egl_textures[m_bound_texture].surface->GetSize(egl_textures[m_bound_texture].surface, &w, &h);

      if (g_glTexCoordPointerEnabled)
      {
#define MIRROR
#ifdef MIRROR
        GLfloat x1, y1, x2, y2;
        
        x1 = glTexCoordPointerItr[0];
        y1 = glTexCoordPointerItr[1];
        
        glTexCoordPointerItr = ((unsigned char*)glTexCoordPointerItr) + 3* g_glTexCoordPointerStride;
        
        x2 = glTexCoordPointerItr[0];
        y2 = glTexCoordPointerItr[1];
        
        glTexCoordPointerItr = ((unsigned char*)glTexCoordPointerItr) + g_glTexCoordPointerStride;
        
        if (x1 <= x2) {
          srcRectangle.x = x1 * w;
          srcRectangle.w = x2 * w - srcRectangle.x;
        }
        else {
          srcRectangle.x = x2 * w;
          srcRectangle.w = x1 * w - srcRectangle.x;
          dsbf |= DSBLIT_FLIP_HORIZONTAL;
        }
        
        if (y1 <= y2) {
          srcRectangle.y = y1 * h;
          srcRectangle.h = y2 * h - srcRectangle.y;
        }
        else {
          srcRectangle.y = y2 * h;
          srcRectangle.h = y1 * h - srcRectangle.y;
          dsbf |= DSBLIT_FLIP_VERTICAL;
        }
        
#else
        srcRectangle.x = glTexCoordPointerItr[0] * w;
        srcRectangle.y = glTexCoordPointerItr[1] * h;
        
        glTexCoordPointerItr = ((unsigned char*)glTexCoordPointerItr) + 3* g_glTexCoordPointerStride;

        srcRectangle.w = glTexCoordPointerItr[0] * w - srcRectangle.x;
        srcRectangle.h = glTexCoordPointerItr[1] * h - srcRectangle.y;
        glTexCoordPointerItr = ((unsigned char*)glTexCoordPointerItr) + g_glTexCoordPointerStride;
#endif
      }
      else
      {
        srcRectangle.x = 0;
        srcRectangle.y = 0;
        srcRectangle.w = w;
        srcRectangle.h = h;
      }

      //DRAW
#ifdef DRAWDEBUG
      printf("StrechBlit %d,%d:%d,%d -> %d,%d:%d,%d\n", srcRectangle.x, srcRectangle.y, srcRectangle.w, srcRectangle.h,
                                                        dstRectangle.x, dstRectangle.y, dstRectangle.w, dstRectangle.h);
#endif
      //if (g_glColorPointerEnabled) m_mainwindow->FillRectangle (m_mainwindow, dstRectangle.x, dstRectangle.y, dstRectangle.w, dstRectangle.h);

      m_mainwindow->SetBlittingFlags(m_mainwindow, dsbf);

      if ((m_drawflags & GL_BLEND_BIT) == GL_BLEND_BIT)
        m_mainwindow->SetDrawingFlags(m_mainwindow, DSDRAW_BLEND);
      else
        m_mainwindow->SetDrawingFlags(m_mainwindow, DSDRAW_NOFX);

      //m_mainwindow->SetColor (m_mainwindow, r, g, b, a);

#if DEBUG
      if (dsbf != DSBLIT_NOFX)
        printf("StrechBlit %d,%d:%d,%d -> %d,%d:%d,%d\n", srcRectangle.x, srcRectangle.y, srcRectangle.w, srcRectangle.h,
                                                        dstRectangle.x, dstRectangle.y, dstRectangle.w, dstRectangle.h);
#endif

      m_mainwindow->StretchBlit(m_mainwindow, egl_textures[m_bound_texture].surface, &srcRectangle, &dstRectangle);
    }
  }
}


void glClearColor (GLclampf red,GLclampf green, GLclampf blue, GLclampf alpha)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif

  m_color_clear_r = (unsigned char)(255.0 * red);
  m_color_clear_g = (unsigned char)(255.0 * green);
  m_color_clear_b = (unsigned char)(255.0 * blue);
  m_color_clear_a = (unsigned char)(255.0 * alpha);
}

void glClear (GLbitfield mask)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif

  DFBCHECK (m_mainwindow->SetColor (m_mainwindow, m_color_clear_r, m_color_clear_g
                                                , m_color_clear_b, m_color_clear_a));

  m_mainwindow->SetDrawingFlags(m_mainwindow, DSDRAW_NOFX);

  DFBCHECK (m_mainwindow->FillRectangle (m_mainwindow, 0, 0, MAINWINDOW_WIDTH, MAINWINDOW_HEIGHT));
}

/* Core */
void glEnable(GLenum cap)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif

  switch(cap)
  {
    case GL_BLEND:      m_drawflags |= GL_BLEND_BIT;      break;
    case GL_TEXTURE_2D: m_drawflags |= GL_TEXTURE_2D_BIT; break;
    default: break;
  }
}

void glDisable(GLenum cap)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif

  switch(cap)
  {
    case GL_BLEND:      m_drawflags &= ~GL_BLEND_BIT;      break;
    case GL_TEXTURE_2D: m_drawflags &= ~GL_TEXTURE_2D_BIT; break;
    default: break;
  }
}

void glFinish (void)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif
  eglSwapBuffers(NULL, NULL);
}

/* Blending */
void glBlendFunc (GLenum sfactor ,GLenum dfactor)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif
}

void glScissor  ( GLint  x /* Specify the lower left corner of the scissor box, in pixels. The initial value is (0, 0).*/
				 ,GLint  y /* Specify the lower left corner of the scissor box, in pixels. The initial value is (0, 0).*/
				 ,GLsizei  width /* Specify the width  of the scissor box. When a GL context is first attached to a surface (e.g. window), width and height are set to the dimensions of that surface. */
				 , GLsizei  height /* Specify the height of the scissor box. When a GL context is first attached to a surface (e.g. window), width and height are set to the dimensions of that surface. */

			)
{
#ifdef DRAWDEBUG
  printf("%s:%s[%d] %d %d %u %u\n", __FILE__, __func__, __LINE__, x, y, width, height);
#endif
}


void glPixelStorei( GLenum pname, GLint param)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif
}

void glReadPixels (GLint x /*Specify the window coordinates of the first pixel that is read from the color buffer. This location is the lower left corner of a rectangular block of pixels.*/
				   , GLint y/*Specify the window coordinates of the first pixel that is read from the color buffer. This location is the lower left corner of a rectangular block of pixels.*/
				   , GLsizei width /*Specify the dimensions of the pixel rectangle. width of one correspond to a single pixel.*/
				   , GLsizei height /* Specify the dimensions of the pixel rectangle.  height of one correspond to a single pixel. */
				   , GLenum format /*Specifies the format of the pixel data. Must be either GL_RGBA or GL_RGB. */
				   , GLenum type /* Specifies the data type of the pixel data. Must be either GL_UNSIGNED_BYTE or GL_BYTE. */
				   , GLvoid *pixels /* Returns the pixel data.*/
				   )
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif
}

void glViewport  ( GLint  x			/* Specify the lower left corner of the viewport box, in pixels. The initial value is (0, 0).*/
				  ,GLint  y			/* Specify the lower left corner of the viewport box, in pixels. The initial value is (0, 0).*/
				  ,GLsizei  width	/* Specify the width  of the viewport box. When a GL context is first attached to a surface (e.g. window), width and height are set to the dimensions of that surface. */
				 ,GLsizei  height /* Specify the height of the viewport box. When a GL context is first attached to a surface (e.g. window), width and height are set to the dimensions of that surface. */
				 )
{
#ifdef DRAWDEBUG
  printf("%s:%s[%d] %d,%d:%d,%d\n", __FILE__, __func__, __LINE__, x, y, width, height);
#endif
}

/* Get */
GLubyte * glGetString(GLenum name)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif

  GLubyte * ret;
  switch(name)
  {
    case GL_VENDOR: return "Unknown Vendor"; //ret = (GLubyte *)strdup("Unknown Vendor"); break;
    case GL_RENDERER: return "Unknown Render"; //ret = (GLubyte *)strdup("Unknown Render"); break;
    case GL_VERSION: return "1.1"; //ret = (GLubyte *)strdup("1.1"); break;
    case GL_EXTENSIONS: return "GL_EXT_texture_format_BGRA8888"; //ret = (GLubyte *)strdup("GL_EXT_texture_format_BGRA8888"); break;
    default: return ""; //ret = (GLubyte *)strdup(""); break;
  }
  return ret;
}

void glGetIntegerv(GLenum pname, GLint * params)
{
  *params = "";
}


