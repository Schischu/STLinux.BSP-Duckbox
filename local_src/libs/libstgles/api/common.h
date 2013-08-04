#ifndef _COMMON_H
#define _COMMON_H

//#define DEBUG
//#define DRAWDEBUG

#define MAINWINDOW_WIDTH 1280
#define MAINWINDOW_HEIGHT 720

#define DFBCHECK(x...)                                         \
  {                                                            \
    DFBResult err = x;                                         \
                                                               \
    if (err != DFB_OK)                                         \
      {                                                        \
        fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
        DirectFBErrorFatal( #x, err );                         \
      }                                                        \
  }

#endif
