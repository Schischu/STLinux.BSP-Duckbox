#ifndef __gl_h__
#define __gl_h__

#include "glplatform.h"

#ifdef __cplusplus
extern "C" {
#endif

#if __GNUC__ >= 4
#  define GLESAPI      __attribute__((visibility("default"))) extern
#  define GLESAPIENTRY
#else
#  define GLESAPI
#  define GLESAPIENTRY
#endif


typedef unsigned int GLenum;		/* GL enum. */
typedef unsigned char GLboolean;	/* GL boolean. */
typedef unsigned int GLbitfield;	/* GL bitfield. */
typedef signed char GLbyte;			/* GL byte. */
typedef short GLshort;				/* GL short. */
typedef int GLint;					/* GL int. */
typedef int GLsizei;				/* GL size int. */
typedef unsigned char GLubyte;		/* GL unsigned byte. */
typedef unsigned short GLushort;	/* GL unsigned short. */
typedef unsigned int GLuint;		/* GL unsigned int. */
typedef float GLfloat;				/* GL float. */
typedef float GLclampf;				/* GL float clamped. */
typedef double GLdouble;			/* GL double. */
typedef double GLclampd;			/* GL double clamped. */
typedef void GLvoid;				/* GL void. */


typedef enum
{
	GL_MODELVIEW,
	GL_PROJECTION,
	GL_TEXTURE,
	GL_MATRIX_MODE_LAST
} GLMatrixMode;


/*************************************************************/

/* OpenGL ES core versions */
#define GL_VERSION_ES_CM_1_0          1
#define GL_VERSION_ES_CL_1_0          1
#define GL_VERSION_ES_CM_1_1          1
#define GL_VERSION_ES_CL_1_1          1

/* ClearBufferMask */
#define GL_DEPTH_BUFFER_BIT               0x00000100	/* Has no effect, but must be set in glClear for forward
                                                    	   compatibility.                                        */
#define GL_COLOR_BUFFER_BIT               0x00004000	/* GL AttribMask GL_COLOR_BUFFER_BIT see glClear &amp;nd
                                                    	   glPushAttrib.                                         */

/* Boolean */
#define GL_FALSE                          0
#define GL_TRUE                           1

/* BeginMode */
//#define GL_POINTS                         0x0000
//#define GL_LINES                          0x0001
//#define GL_LINE_LOOP                      0x0002
#define GL_TRIANGLES                      0x0004
#define GL_QUADS                          0x0005

/* AlphaFunction */
#define GL_LESS                           0x0201

/* BlendingFactorDest */
#define GL_ZERO                           0		/* GL ZERO. */
#define GL_ONE                            1		/* GL ONE. */
/* Not implemented. */
/* Not supported. */
#define GL_SRC_COLOR                      0x0300	/* GL_SRC_COLOR see glBlendFunc */
/* Not implemented. */
#define GL_ONE_MINUS_SRC_COLOR            0x0301	/* GL_ONE_MINUS_SRC_COLOR see glBlendFunc */
/* Blending uses alpha source. */
#define GL_SRC_ALPHA                      GL_ONE/*SRC_ALPHA does not work so use ONE 0x0302*/	/* GL_SRC_ALPHA see glBlendFunc */
/* Not implemented. */
#define GL_ONE_MINUS_SRC_ALPHA            0x0303	/* GL_ONE_MINUS_SRC_ALPHA see glBlendFunc */
/* Not implemented. */
#define GL_DST_ALPHA                      0x0304	/* GL_DST_ALPHA see glBlendFunc  */
/* Not implemented. */
#define GL_ONE_MINUS_DST_ALPHA            0x0305	/* GL_ONE_MINUS_DST_ALPHA see glBlendFunc */

/* BlendingFactorSrc */
/* ClipPlaneName */
/* ColorMaterialFace */
/* ColorMaterialParameter */
/* ColorPointerType */
/* CullFaceMode */
/* DepthFunction */

/* EnableCap */
#define GL_TEXTURE_2D                     0x0DE1	/* If enabled, two-dimensional texturing is performed. */
#define GL_CULL_FACE                      0x0B44	/* If enabled, cull polygons based on their winding in window
                                                	   coordinates.                                               */
#define GL_BLEND                          0x0BE2	/* Blending enabled. */
#define GL_DEPTH_TEST                     0x0B71	/* If enabled, primitive will be sorted by the z order. */
#define GL_SCISSOR_TEST                   0x0C11	/* If enabled, discard fragments that are outside the scissor
                                                	   rectangle.                                                 */
#define GL_VERTEX_ARRAY                   0x8074
#define GL_COLOR_ARRAY                    0x8076
#define GL_TEXTURE_COORD_ARRAY            0x8078

/* ErrorCode */
#define GL_NO_ERROR                       0			/* No error has been recorded. The value of this symbolic
                                           			   constant is guaranteed to be 0.                        */
#define GL_INVALID_ENUM                   0x0500	/* Error enum invalid. */
#define GL_INVALID_VALUE                  0x0501    /* Error value invalid. */
#define GL_INVALID_OPERATION              0x0502	/* Error operation invalid. */
#define GL_STACK_OVERFLOW                 0x0503	/* Error stack overflow, see glPushMatrix/glPopMatrix */
#define GL_STACK_UNDERFLOW                0x0504	/* Error stack underflow, see glPushMatrix/glPopMatrix. */
#define GL_OUT_OF_MEMORY                  0x0505	/* Error out of memory. */

/* FogMode */
/* FogParameter */

/* GetPName */
#define GL_CURRENT_COLOR                  0x0B00	/* Current color. */
#define GL_VIEWPORT                       0x0BA2
#define GL_MODELVIEW_STACK_DEPTH          0x0BA3	/* Modelview matrix stack pointer. */
#define GL_MODELVIEW_MATRIX               0x0BA6	/* Modelview matrix stack. */
#define GL_PROJECTION_MATRIX              0x0BA7	/* Projection matrix stack. */
#define GL_BLEND_DST                      0x0BE0	/* Blending destination function. */
#define GL_BLEND_SRC                      0x0BE1	/* Blending source function. */
#define GL_SCISSOR_TEST                   0x0C11	/* If enabled, discard fragments that are outside the scissor
                                                	   rectangle.*/
#define GL_COLOR_CLEAR_VALUE              0x0C22	/* Color-buffer clear value (RGBA mode). */
#define GL_MAX_TEXTURE_SIZE               0x0D33
#define GL_UNPACK_ALIGNMENT               0x0CF5	/* Value of GL_UNPACK_ALIGNMENT see glReadPixels. */
#define GL_MAX_PROJECTION_STACK_DEPTH     0x0D38	/* Maximum projection-matrix stack depth. */
#define GL_MAX_VIEWPORT_DIMS              0x0D3A	/* Maximum viewport dimensions. */

/* GetTextureParameter */
/* HintMode */
/* HintTarget */
/* LightModelParameter */
/* LightParameter */

/* DataType */
#define GL_BYTE                           0x1400	/* GL Data Type GL_BYTE. */
#define GL_UNSIGNED_BYTE                  0x1401	/* GL Data Type GL_UNSIGNED_BYTE. */
#define GL_SHORT                          0x1402	/* GL Data Type GL_SHORT. */
#define GL_UNSIGNED_SHORT                 0x1403	/* GL Data Type GL_UNSIGNED_SHORT. */
#define GL_FLOAT                          0x1406	/* GL Data Type GL_FLOAT. */

/* LogicOp */
/* MaterialFace */
/* MaterialParameter */
/* MatrixMode */
/* NormalPointerType */

/* PixelFormat */
#define GL_ALPHA                          0x1906	/* GL PixelFormat GL_ALPHA. */
#define GL_RGB                            0x1907	/* GL PixelFormat GL_RGB. */
#define GL_RGBA                           0x1908	/* GL PixelFormat GL_RGBA. */
/* Not implemented. */
#define GL_LUMINANCE                      0x1909
/* Not implemented. */
#define GL_LUMINANCE_ALPHA                0x190A

/* PixelStoreParameter */
#define GL_UNPACK_ALIGNMENT               0x0CF5	/* Value of GL_UNPACK_ALIGNMENT see glReadPixels. */

/* PixelType */
/* ShadingModel */
/* StencilFunction */
/* StencilOp */

/* StringName */
#define GL_VENDOR                         0x1F00
#define GL_RENDERER                       0x1F01
#define GL_VERSION                        0x1F02
#define GL_EXTENSIONS                     0x1F03

/* TexCoordPointerType */
/* TextureEnvMode */
/* TextureEnvParameter */
/* TextureEnvTarget */

/* TextureMagFilter */
#define GL_LINEAR                         0x2601

/* TextureMinFilter */

/* TextureParameterName */
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803

/* TextureTarget */

/* TextureUnit */
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1

/* TextureWrapMode */
#define GL_CLAMP_TO_EDGE                  0x812F

/* VertexPointerType */
/* LightName */
/* Buffer Objects */
/* Texture combine + dot3 */





//INTERNAL
#define GL_UNPACK_ROW_LENGTH              0x0CF2	/* Value of GL_UNPACK_ROW_LENGTH see glReadPixels. */
#define GL_UNPACK_SKIP_ROWS               0x0CF3	/* Value of GL_UNPACK_SKIP_ROWS see glReadPixels. */
#define GL_UNPACK_SKIP_PIXELS             0x0CF4	/* Value of GL_UNPACK_SKIP_PIXELS see glReadPixels. */

#define GL_CURRENT_BIT                    0x00000001	/* GL AttribMask GL_CURRENT_BIT see glPushAttrib. */
#define GL_ENABLE_BIT                     0x00002000	/* GL AttribMask GL_CURRENT_BIT see glPushAttrib. */
#define GL_ALL_ATTRIB_BITS                0x000fffff	/* GL all AttribMask see glPushAttrib. */




//http://www.khronos.org/opengles/sdk/1.1/docs/man/

GLESAPI void      GLESAPIENTRY  glGenTextures (GLsizei n, GLuint *textures);
GLESAPI void      GLESAPIENTRY  glDeleteTextures (GLsizei n, GLuint *textures);
GLESAPI void      GLESAPIENTRY  glBindTexture (GLenum target, GLuint texture);
GLESAPI GLboolean GLESAPIENTRY  glIsTexture (GLuint texture);
GLESAPI void GLESAPIENTRY  glActiveTexture(GLenum texture);

GLESAPI void      GLESAPIENTRY  glTexParameteri (GLenum target, GLenum pname, GLint param);
GLESAPI void      GLESAPIENTRY  glTexImage2D (GLenum target, GLint level, GLint internalformat, 
                                                   GLsizei width, GLsizei height, GLint border, 
                                                   GLenum format, GLenum type, const GLvoid *pixels);

GLESAPI void      GLESAPIENTRY  glTexSubImage2D(GLenum target, GLint level, GLint xoffset, 
                                                      GLint yoffset, GLsizei width, GLsizei height, 
                                                      GLenum format, GLenum type, const GLvoid * pixels);

GLESAPI void      GLESAPIENTRY  glColorPointer(GLint size, GLenum type, 
                                                     GLsizei stride, const GLvoid * pointer);
GLESAPI void      GLESAPIENTRY glTexCoordPointer(GLint size, GLenum type, 
                                                       GLsizei stride, const GLvoid * pointer);
GLESAPI void      GLESAPIENTRY glVertexPointer(GLint size, GLenum type, 
                                                     GLsizei stride, const GLvoid * pointer);
GLESAPI void      GLESAPIENTRY glEnableClientState(GLenum array);
GLESAPI void      GLESAPIENTRY glDisableClientState(GLenum array);
GLESAPI void      GLESAPIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count);


GLESAPI GLubyte * GLESAPIENTRY glGetString(GLenum name);

                                               
GLESAPI GLenum GLESAPIENTRY  glGetError (void);
GLESAPI  void  GLESAPIENTRY  glEnable(GLenum cap);
GLESAPI  void  GLESAPIENTRY  glDisable(GLenum cap);
GLESAPI  void  GLESAPIENTRY  glGetIntegerv(GLenum pname,GLint * params);
GLESAPI  void  GLESAPIENTRY  glGetFloatv(GLenum pname,GLfloat * params);
GLESAPI  void  GLESAPIENTRY  glGetBooleanv( GLenum pname, GLboolean *params );
GLESAPI  void  GLESAPIENTRY  glBlendFunc (GLenum sfactor, GLenum dfactor);
GLESAPI  void  GLESAPIENTRY  glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
GLESAPI  void  GLESAPIENTRY  glClear (GLbitfield mask);
GLESAPI  void  GLESAPIENTRY  glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
GLESAPI  void  GLESAPIENTRY  glScissor  ( GLint  x,    GLint  y,    GLsizei  width,    GLsizei  height    );
GLESAPI  void  GLESAPIENTRY  glPixelStorei( GLenum pname, GLint param );
GLESAPI  void  GLESAPIENTRY  glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
GLESAPI	void  GLESAPIENTRY  glPushAttrib(GLbitfield mask	);
GLESAPI	void  GLESAPIENTRY  glPopAttrib (void);

GLESAPI  void GLESAPIENTRY   glMatrixMode(GLMatrixMode mode);
GLESAPI  void GLESAPIENTRY   glLoadMatrix(GLfloat* matrix);
GLESAPI  void  GLESAPIENTRY  glLoadIdentity (void);
GLESAPI  void  GLESAPIENTRY  glTranslatef (GLfloat x, GLfloat y, GLfloat z);
GLESAPI  void GLESAPIENTRY   glMultMatrixf(const GLfloat *m);
GLESAPI  void  GLESAPIENTRY  glRotatef (GLfloat angle,GLfloat x, GLfloat y, GLfloat z);
GLESAPI  void  GLESAPIENTRY  glScalef (GLfloat x, GLfloat y, GLfloat z);
GLESAPI  void  GLESAPIENTRY  glPushMatrix (void);
GLESAPI  void  GLESAPIENTRY  glPopMatrix (void);
GLESAPI  void GLESAPIENTRY   glFinish (void);
GLESAPI  void GLESAPIENTRY   glViewport (GLint x, GLint y, GLsizei width, GLsizei height);

#define GL_FRAMEBUFFER 0
#define GL_CLAMP 0
#define glBindFramebuffer(target, framebuffer) {}

#ifdef __cplusplus
}
#endif

#endif /* gl_h__ */
