#ifndef __egl_h__
#define __egl_h__
#include <EGL/eglplatform.h>

#ifdef __cplusplus
extern "C" {
#endif

#if __GNUC__ >= 4
#  define EGLAPI      __attribute__((visibility("default"))) extern
#  define EGLAPIENTRY
#else
#  define EGLAPI
#  define EGLAPIENTRY
#endif

typedef int             EGLint;			/* EGL int. */
typedef int             EGLenum;		/* EGL enum. */
typedef unsigned int    EGLBoolean;		/* EGL boolean. */
typedef int             EGLConfig;		/* EGL config handle. */
typedef void*           EGLContext;		/* EGL context handle. */
typedef void*           EGLDisplay;		/* EGL display handle. */
typedef void *          EGLSurface;		/* EGL Surface Handle. */


/* Out-of-band attribute value. */
#define EGL_DONT_CARE				((EGLint)-1)

/* QuerySurface / CreatePbufferSurface targets */

#define EGL_HEIGHT					0x3056 /* EGL height. */
#define EGL_WIDTH					0x3057 /* EGL width  */

#define EGL_TEXTURE_FORMAT			0x3080/* Not supported. */
#define EGL_TEXTURE_TARGET			0x3081 /* Not supported. */
#define EGL_MIPMAP_TEXTURE			0x3082 /* Not supported. */
#define EGL_MIPMAP_LEVEL			0x3083 /* Not supported. */
#define EGL_RENDER_BUFFER			0x3086 /* Not supported. */
#define EGL_COLORSPACE				0x3087  /* Not supported. */
#define EGL_ALPHA_FORMAT			0x3088 /* Not supported. */


/* EGL_RENDER_BUFFER values / BindTexImage / ReleaseTexImage buffer targets */
#define EGL_BACK_BUFFER				0x3084 /* Surface double buffered. */
#define EGL_SINGLE_BUFFER			0x3085 /* Surface simple buffering. */


/* OpenVG color spaces */
#define EGL_COLORSPACE_sRGB			0x3089	/* EGL_COLORSPACE value. */
#define EGL_COLORSPACE_LINEAR		0x308A	/* EGL_COLORSPACE value. */

/* OpenVG alpha formats */
#define EGL_ALPHA_FORMAT_NONPRE		0x308B	/* EGL_ALPHA_FORMAT value. */
#define EGL_ALPHA_FORMAT_PRE		0x308C	/* EGL_ALPHA_FORMAT value. */


/* Errors / GetError return values */

#define EGL_SUCCESS					0x3000 /* Function succeeded. */
#define EGL_NOT_INITIALIZED			0x3001 /* EGL is not initialized, or could not be initialized, for the
                           			          specified display.                                           */
#define EGL_BAD_ACCESS				0x3002 /* EGL cannot access a requested resource (for example, a
                      				          context is bound in another thread).                   */
#define EGL_BAD_ALLOC				0x3003 /* EGL failed to allocate resources for the requested operation. */
#define EGL_BAD_ATTRIBUTE			0x3004 /* An unrecognized attribute or attribute value was passed in an attribute list.*/
#define EGL_BAD_CONFIG				0x3005 /* An EGLConfig argument does not name a valid EGLConfig.*/
#define EGL_BAD_CONTEXT				0x3006 /* An EGLContext argument does not name a valid EGLContext.*/
#define EGL_BAD_CURRENT_SURFACE		0x3007 /* The current surface of the calling thread is a window, pbuffer, or pixmap that is no longer valid.*/
#define EGL_BAD_DISPLAY				0x3008 /* An EGLDisplay argument does not name a valid EGLDisplay; or, EGL is not initialized on the specified EGLDisplay.*/
#define EGL_BAD_MATCH				0x3009 /* Arguments are inconsistent; for example, an otherwise valid context requires buffers (e.g. depth or stencil) not allocated by an otherwise valid surface. */
#define EGL_BAD_NATIVE_PIXMAP		0x300A /* An EGLNativePixmapType argument does not refer to a valid native pixmap.*/
#define EGL_BAD_NATIVE_WINDOW		0x300B /* An EGLNativeWindowType argument does not refer to a valid native window.*/
#define EGL_BAD_PARAMETER			0x300C  /* One or more argument values are invalid. */
#define EGL_BAD_SURFACE				0x300D  /* An EGLSurface argument does not name a valid surface (window, pbuffer, or pixmap) configured for rendering.*/
#define EGL_CONTEXT_LOST			0x300E	/* EGL 1.1 - IMG_power_management. */


/* EGL aliases */
#define EGL_FALSE					0 /* FALSE = 0 */
#define EGL_TRUE					1 /* TRUE  = 0 */

/* BindAPI/QueryAPI targets */
#define EGL_OPENGL_ES_API			0x30A0	/* EGL binded to OpenGL ES. */
#define EGL_OPENVG_API				0x30A1	/* EGL binded to OVG (not supported ). */


/* Config attributes */

#define EGL_BUFFER_SIZE				0x3020 /* Depth bits of the color buffer. */
#define EGL_ALPHA_SIZE				0x3021 /* Alpha Bits in the color buffer. */
#define EGL_BLUE_SIZE				0x3022 /* Blue Bits in the color buffer. */
#define EGL_GREEN_SIZE				0x3023 /* Green bits in the color buffer. */
#define EGL_RED_SIZE				0x3024 /* Red bits of the color buffer */
#define EGL_DEPTH_SIZE				0x3025 /* Not supported. */
#define EGL_STENCIL_SIZE			0x3026 /* Not supported. */
#define EGL_CONFIG_CAVEAT			0x3027 /* Enum any caveats for the configuration (always EGL_NONE). */
#define EGL_CONFIG_ID				0x3028 	/* Unique EGLConfig identifier. */
#define EGL_LEVEL					0x3029  /* Not supported. */
#define EGL_MAX_PBUFFER_HEIGHT		0x302A  /* Not supported. */
#define EGL_MAX_PBUFFER_PIXELS		0x302B  /* Not supported. */
#define EGL_MAX_PBUFFER_WIDTH		0x302C  /* Not supported. */
#define EGL_NATIVE_RENDERABLE		0x302D  /* Not supported. */
#define EGL_NATIVE_VISUAL_ID		0x302E  /* Enum type of transparency supported (always EGL_NONE). */
#define EGL_NATIVE_VISUAL_TYPE		0x302F  /* Integer native visual type of the associated visual. */

#define EGL_SAMPLES					0x3031 /* Not supported. */
#define EGL_SAMPLE_BUFFERS			0x3032 /* Not supported. */
#define EGL_SURFACE_TYPE			0x3033 /* All types are always supported. */
#define EGL_TRANSPARENT_TYPE		0x3034 /* Not supported. */
#define EGL_TRANSPARENT_BLUE_VALUE	0x3035 /* Not supported. */
#define EGL_TRANSPARENT_GREEN_VALUE	0x3036 /* Not supported. */
#define EGL_TRANSPARENT_RED_VALUE	0x3037 /* Not supported. */
#define EGL_NONE					0x3038	/* Attrib list terminator */
#define EGL_BIND_TO_TEXTURE_RGB		0x3039 /* Not supported. */
#define EGL_BIND_TO_TEXTURE_RGBA	0x303A /* Not supported. */
#define EGL_MIN_SWAP_INTERVAL		0x303B /* Integer minimum swap interval (always 1). */
#define EGL_MAX_SWAP_INTERVAL		0x303C /* integer maximum swap interval (always 1) */
#define EGL_LUMINANCE_SIZE			0x303D /* Not supported. */
#define EGL_ALPHA_MASK_SIZE			0x303E /* Not supported. */
#define EGL_COLOR_BUFFER_TYPE		0x303F /* Enum color buffer type (always EGL_RGB_BUFFER) */
#define EGL_RENDERABLE_TYPE			0x3040 /* Not supported #define EGL_MATCH_NATIVE_PIXMAP 0x3041 */


/* Config attribute mask bits */
#define EGL_PBUFFER_BIT				0x01	/* EGL_SURFACE_TYPE mask bits. */
#define EGL_PIXMAP_BIT				0x02	/* EGL_SURFACE_TYPE mask bits. */
#define EGL_WINDOW_BIT				0x04	/* EGL_SURFACE_TYPE mask bits */
#define EGL_OPENGL_ES_BIT			0x01	/* EGL_RENDERABLE_TYPE mask bits. */
#define EGL_OPENVG_BIT				0x02	/* EGL_RENDERABLE_TYPE mask bits. */
#define EGL_OPENGL_ES2_BIT			0x04	/* EGL_RENDERABLE_TYPE mask bits. */



/* Out-of-band handle values */
/* These values may vary depending on semantics of native concepts */
#define EGL_DEFAULT_DISPLAY			((EGLDisplay)(NativeDisplayType)1) /* Default Display. */
#define EGL_NO_CONTEXT				((EGLContext)0) /* EGL No Context. */
#define EGL_NO_DISPLAY				((EGLDisplay)0) /* EGL No Display. */
#define EGL_NO_SURFACE				((EGLSurface)0)	/* EGL No Surface. */


/* Config attribute values */
#define EGL_SLOW_CONFIG				0x3050	/* EGL_CONFIG_CAVEAT value. */
#define EGL_NON_CONFORMANT_CONFIG	0x3051	/* EGL_NON_CONFORMANT_CONFIG value. */
#define EGL_TRANSPARENT_RGB			0x3052	/* EGL_TRANSPARENT_TYPE value. */
#define EGL_RGB_BUFFER				0x308E	/* EGL_COLOR_BUFFER_TYPE value. */
#define EGL_LUMINANCE_BUFFER		0x308F	/* EGL_COLOR_BUFFER_TYPE value. */

/* GetCurrentSurface targets */
#define EGL_DRAW					0x3059	/* Selects the Draw buffer. */
#define EGL_READ					0x305A  /* Selects the Read buffer. */

/* QueryString targets */
#define EGL_VENDOR			0x3053
#define EGL_VERSION			0x3054
#define EGL_EXTENSIONS			0x3055
#define EGL_CLIENT_APIS			0x308D

#define EGL_CONTEXT_CLIENT_VERSION 0x3098


EGLAPI const char * EGLAPIENTRY eglQueryString(EGLDisplay dpy, EGLint name);
EGLAPI EGLDisplay EGLAPIENTRY eglGetDisplay(int display_id);
EGLAPI EGLBoolean EGLAPIENTRY eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor);
EGLAPI EGLBoolean EGLAPIENTRY eglTerminate(EGLDisplay dpy);
EGLAPI EGLint     EGLAPIENTRY eglGetError(void);
EGLAPI EGLBoolean EGLAPIENTRY eglBindAPI(EGLenum api);
EGLAPI EGLBoolean EGLAPIENTRY eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config);
EGLAPI EGLSurface EGLAPIENTRY eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, NativeWindowType win, const EGLint *attrib_list);
EGLAPI EGLSurface EGLAPIENTRY eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config, NativePixmapType pixmap, const EGLint *attrib_list);
EGLAPI EGLBoolean EGLAPIENTRY eglDestroySurface(EGLDisplay dpy, EGLSurface surface);
EGLAPI EGLContext EGLAPIENTRY eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list);
EGLAPI EGLBoolean EGLAPIENTRY eglDestroyContext(EGLDisplay dpy, EGLContext ctx);
EGLAPI EGLBoolean EGLAPIENTRY eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx);
EGLAPI EGLContext EGLAPIENTRY eglGetCurrentContext(void);
EGLAPI EGLSurface EGLAPIENTRY eglGetCurrentSurface(EGLint readdraw);
EGLAPI EGLBoolean EGLAPIENTRY eglSwapBuffers(EGLDisplay dpy, EGLSurface surface);
EGLAPI EGLBoolean EGLAPIENTRY eglQuerySurface(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value);
EGLAPI EGLBoolean EGLAPIENTRY eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value);
EGLAPI EGLBoolean EGLAPIENTRY eglSwapInterval(EGLDisplay dpy, EGLint interval);


#ifdef __cplusplus
}
#endif


#endif /* egl_h__ */
