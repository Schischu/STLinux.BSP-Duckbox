

#include <EGL/egl.h>
#include <GLES/gl.h>

static EGLDisplay            m_display;
  EGLConfig             m_config;
  EGLSurface            m_surface;

  EGLNativeWindowType   m_nativeWindow;
  EGLNativeDisplayType  m_nativeDisplay;
  EGLContext            m_context;
  char*            m_eglext;


int main(int argc, char *argv[])
{
  EGLBoolean eglStatus;
  EGLint     configCount;
  EGLConfig* configList = NULL;  

  EGLDisplay nativeDisplay = EGL_DEFAULT_DISPLAY;



  /* Display */
  m_display = eglGetDisplay(nativeDisplay);
  if (m_display == EGL_NO_DISPLAY) 
  {
    printf("EGL failed to obtain display");
    return false;
  }

  /* Initialize */
  if (!eglInitialize(m_display, 0, 0)) 
  {
    printf("EGL failed to initialize");
    return false;
  } 


  /* Configuration */
  EGLint configAttrs[] = {
        EGL_RED_SIZE,        8,
        EGL_GREEN_SIZE,      8,
        EGL_BLUE_SIZE,       8,
        EGL_DEPTH_SIZE,     16,
        EGL_STENCIL_SIZE,    0,
        EGL_SAMPLE_BUFFERS,  0,
        EGL_SAMPLES,         0,
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT, 
        EGL_NONE
  };

  // Find out how many configurations suit our needs
  eglStatus = eglChooseConfig(m_display, configAttrs, NULL, 0, &configCount);
  if (!eglStatus || !configCount) 
  {
    printf("EGL failed to return any matching configurations: %d", eglStatus);
    return false;
  }
    
  // Allocate room for the list of matching configurations
  configList = (EGLConfig*)malloc(configCount * sizeof(EGLConfig));
  if (!configList) 
  {
    printf("kdMalloc failure obtaining configuration list");
    return false;
  }

  // Obtain the configuration list from EGL
  eglStatus = eglChooseConfig(m_display, configAttrs,
                                configList, configCount, &configCount);
  if (!eglStatus || !configCount) 
  {
    printf("EGL failed to populate configuration list: %d", eglStatus);
    return false;
  }
  
  // Select an EGL configuration that matches the native window
  m_config = configList[0];


  /* Surface */
  m_surface = eglCreateWindowSurface(m_display, m_config, m_nativeWindow, NULL);
  if (!m_surface)
  { 
    printf("EGL couldn't create window surface");
    return false;
  }

  /* Bind */
  eglStatus = eglBindAPI(EGL_OPENGL_ES_API);
  if (!eglStatus) 
  {
    printf("EGL failed to bind API: %d", eglStatus);
    return false;
  }

  /* Context */
  EGLint contextAttrs[] = 
  {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
  };

  // Create an EGL context
  if (m_context == EGL_NO_CONTEXT)
  {
    m_context = eglCreateContext(m_display, m_config, NULL, contextAttrs);
    if (!m_context)
    {
      printf("EGL couldn't create context");
      return false;
    }
  }

  /* Make Current */
  // Make the context and surface current to this thread for rendering
  eglStatus = eglMakeCurrent(m_display, m_surface, m_surface, m_context);
  if (!eglStatus) 
  {
    printf("EGL couldn't make context/surface current: %d", eglStatus);
    return false;
  }
 
  free(configList);

  /* Clear Screen */
  eglSwapInterval(m_display, 0);

  // For EGL backend, it needs to clear all the back buffers of the window
  // surface before drawing anything, otherwise the image will be blinking
  // heavily.  The default eglWindowSurface has 3 gdl surfaces as the back
  // buffer, that's why glClear should be called 3 times.
  glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
  glClear (GL_COLOR_BUFFER_BIT);
  eglSwapBuffers(m_display, m_surface);

  glClear (GL_COLOR_BUFFER_BIT);
  eglSwapBuffers(m_display, m_surface);

  glClear (GL_COLOR_BUFFER_BIT);
  eglSwapBuffers(m_display, m_surface);

  m_eglext = eglQueryString(m_display, EGL_EXTENSIONS);
  printf("EGL extensions:%s", m_eglext);

  // setup for vsync disabled
  eglSwapInterval(m_display, 0);

  printf("EGL window and context creation complete");

  sleep(5);

  return 0;
}

