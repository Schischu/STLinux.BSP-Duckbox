
#include <stdio.h>
#include <unistd.h>
#include <directfb.h>

#include <EGL/egl.h>

#include "common.h"

//--------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------

IDirectFB             *m_dfb = NULL;
static IDirectFBDisplayLayer *m_displaylayer;
static IDirectFBScreen       *m_screen;

IDirectFBSurface      *m_mainwindow;


static EGLBoolean m_vsync = EGL_FALSE;

//--------------------------------------------------------------------------------------------------

EGLDisplay 
eglGetDisplay (int display_id)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif
  return (EGLDisplay)1;
}

EGLBoolean
eglInitialize (EGLDisplay  dpy,
               EGLint     *major,
               EGLint     *minor)
{
  int argc = 0;
  char**argv = NULL;

#ifdef DEBUG
  unsigned long    vsync = 0;
  int screen_width, screen_height;
#endif
  
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif

  DFBCHECK(DirectFBInit(&argc, &argv));
  DFBCHECK(DirectFBCreate(&m_dfb));

  DFBCHECK (m_dfb->SetCooperativeLevel(m_dfb, DFSCL_FULLSCREEN));

  /* Screen */
  DFBCHECK(m_dfb->GetScreen (m_dfb, DSCID_PRIMARY, &m_screen));
#ifdef DEBUG
  DFBCHECK(m_screen->GetSize(m_screen, &screen_width, &screen_height));
  printf("Screen Size: %dx%d\n", screen_width, screen_height);
  DFBCHECK(m_screen->GetVSyncCount (m_screen, &vsync));
  printf("Screen Vsync: %lu\n", vsync);
#endif

  /* DisplayLayer */
  DFBCHECK (m_dfb->GetDisplayLayer(m_dfb, DLID_PRIMARY, &m_displaylayer));
#ifdef DEBUG
  DFBDisplayLayerDescription dlc;
  m_displaylayer->GetDescription(m_displaylayer, &dlc);
  if ((dlc.caps & DLCAPS_SCREEN_SIZE) == DLCAPS_SCREEN_SIZE)
    printf("SCREEN_SIZE\n"); 
  else
    printf("NO SCREEN_SIZE!!!\n"); 
#endif



  return EGL_TRUE;
}


EGLBoolean
eglChooseConfig (EGLDisplay    dpy,
                 const EGLint *attrib_list,
                 EGLConfig    *configs,
                 EGLint        config_size,
                 EGLint       *num_config)
{
  EGLBoolean ret = EGL_FALSE;

#ifdef DEBUG
  printf("%s:%s[%d] ->\n", __FILE__, __func__, __LINE__);
#endif

  if (configs == NULL && config_size == 0)
  {
    *num_config = 1;
    ret = EGL_TRUE;
  }
  else
  {
    configs[0] = (EGLConfig)1;
    *num_config = 1;
    ret = EGL_TRUE;
  }
#ifdef DEBUG
  printf("%s:%s[%d] <- ret=%d\n", __FILE__, __func__, __LINE__, ret);
#endif

  return ret;
}


EGLSurface
eglCreateWindowSurface (EGLDisplay        dpy,
                        EGLConfig         config,
                        NativeWindowType  win,
                        const EGLint     *attrib_list)
{
  EGLSurface ret;
  
#ifdef DEBUG
  int mainwindow_width, mainwindow_height;
#endif

#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif

  /* Surface */
  DFBSurfaceDescription dsc;
  dsc.flags  = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_CAPS;
  dsc.caps   = DSCAPS_PRIMARY | DSCAPS_FLIPPING;
  dsc.width  = MAINWINDOW_WIDTH;
  dsc.height = MAINWINDOW_HEIGHT;
  DFBCHECK (m_dfb->CreateSurface(m_dfb, &dsc, &m_mainwindow));

  ret = (EGLSurface)1;

#ifdef DEBUG
  DFBCHECK (m_mainwindow->GetSize (m_mainwindow, &mainwindow_width, &mainwindow_height));
  printf("MainWindow Size: %dx%d\n", mainwindow_width, mainwindow_height);
#endif

  return ret;
}


EGLBoolean
eglBindAPI (EGLenum api)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif

  if (api == EGL_OPENGL_ES_API)
    return EGL_TRUE;
  return EGL_FALSE;
}


EGLContext
eglCreateContext (EGLDisplay    dpy,
                  EGLConfig     config,
                  EGLContext    share_context,
                  const EGLint *attrib_list)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif

  return (EGLContext)1;
}


EGLBoolean
eglMakeCurrent (EGLDisplay dpy,
                EGLSurface draw,
                EGLSurface read,
                EGLContext ctx)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif

  return EGL_TRUE;
}

EGLBoolean
eglSwapInterval (EGLDisplay dpy,
                 EGLint     interval)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif
  
  if (interval > 0) m_vsync = EGL_TRUE;
  else m_vsync = EGL_FALSE;

  // We do not care as we dont render video
  return EGL_TRUE;
}

EGLBoolean
eglSwapBuffers (EGLDisplay dpy,
                EGLSurface surface)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif

  //DFBSurfaceFlipFlags flags;
  
  DFBCHECK (m_mainwindow->Flip (m_mainwindow, NULL, m_vsync==EGL_TRUE?DSFLIP_WAITFORSYNC:0));
  return EGL_TRUE;
}

const char * 
eglQueryString(EGLDisplay dpy, EGLint name)
{
  char * string;
  
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif

  switch(name)
  {
    case EGL_VENDOR: return "Unknown"; //string = strdup("Unknown"); break;
    case EGL_VERSION: return "1.4"; //string = strdup("1.4"); break;
    case EGL_EXTENSIONS: return ""; //string = strdup(""); break;
    case EGL_CLIENT_APIS: return ""; //tring = strdup(""); break;
    default: return ""; //string = strdup(""); break;
  }
  return string;
}

EGLBoolean
eglDestroyContext (EGLDisplay dpy,
                   EGLContext ctx)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif

  return EGL_TRUE;
}

EGLBoolean
eglDestroySurface (EGLDisplay dpy,
                   EGLSurface surface)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif

  m_mainwindow->Release(m_mainwindow);

  return EGL_TRUE;
}

EGLBoolean
eglTerminate (EGLDisplay dpy)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif
  m_displaylayer->Release(m_displaylayer);
  m_screen->Release(m_screen);
  m_dfb->Release(m_dfb);
  
  return EGL_TRUE;
}

EGLBoolean
eglQuerySurface (EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif
  if (attribute == EGL_WIDTH)       *value = 1280;
  else if (attribute == EGL_HEIGHT) *value = 720;
  else return EGL_FALSE;
  
  return EGL_TRUE;
}

EGLint
eglGetError(void)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif
  return EGL_SUCCESS;
}

EGLBoolean
eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value)
{
#ifdef DEBUG
  printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
#endif
  return EGL_FALSE;
}



