#ifndef __eglplatform_h__
#define __eglplatform_h__

#ifdef __cplusplus
extern "C" {
#endif

#define EGLAPI
#define EGLAPIENTRY

#include <directfb.h>
#define NativePixmapType  IDirectFBSurface *
#define NativeDisplayType IDirectFBScreen *
#define NativeWindowType  IDirectFBSurface *


/* EGL 1.2 types, renamed for consistency in EGL 1.3 */
typedef NativeDisplayType EGLNativeDisplayType;
typedef NativePixmapType  EGLNativePixmapType;
typedef NativeWindowType  EGLNativeWindowType;




#ifdef __cplusplus
}
#endif
#endif /* eglplatform_h__ */
