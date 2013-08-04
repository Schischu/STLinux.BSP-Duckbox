#include <stdio.h>
#include <unistd.h>
#include <directfb.h>

static IDirectFB             *m_dfb = NULL;
static IDirectFBDisplayLayer *m_displaylayer;
static IDirectFBSurface      *m_mainwindow;
static IDirectFBScreen       *m_screen;

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

#if 0
DFBDisplayLayerCapabilities
     DLCAPS_SCREEN_SIZE       = 0x00200000,  /* The layer size (defined by its source rectangle)
                                                can be scaled to a different size on the screen
                                                (defined by its screen/destination rectangle or
                                                its normalized size) and does not have to be 1:1
                                                with it. */
#endif

int main(int argc, char *argv[])
{
  unsigned long    vsync = 0;
  int screen_width, screen_height;
  int mainwindow_width, mainwindow_height;

  DFBCHECK(DirectFBInit(&argc, &argv));
  DFBCHECK(DirectFBCreate(&m_dfb));

  DFBCHECK (m_dfb->SetCooperativeLevel(m_dfb, DFSCL_FULLSCREEN));

  /* Screen */
  DFBCHECK(m_dfb->GetScreen (m_dfb, DSCID_PRIMARY, &m_screen));
  DFBCHECK(m_screen->GetSize(m_screen, &screen_width, &screen_height));
  printf("Screen Size: %dx%d\n", screen_width, screen_height);
  DFBCHECK(m_screen->GetVSyncCount (m_screen, &vsync));
  printf("Screen Vsync: %u\n", vsync);

  /* DisplayLayer */
  DFBCHECK (m_dfb->GetDisplayLayer(m_dfb, DLID_PRIMARY, &m_displaylayer));
  
  DFBDisplayLayerDescription dlc;
  m_displaylayer->GetDescription(m_displaylayer, &dlc);
  if (dlc.caps & DLCAPS_SCREEN_SIZE == DLCAPS_SCREEN_SIZE)
    printf("SCREEN_SIZE\n"); 
  else
    printf("NO SCREEN_SIZE!!!\n"); 

  /* Surface */
  DFBSurfaceDescription dsc;
  dsc.flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_CAPS;
  dsc.caps = (DFBSurfaceCapabilities) (DSCAPS_PRIMARY | DSCAPS_FLIPPING);
  dsc.width = 1280;
  dsc.height = 720;
  DFBCHECK (m_dfb->CreateSurface(m_dfb, &dsc, &m_mainwindow));

  DFBCHECK (m_mainwindow->GetSize (m_mainwindow, &mainwindow_width, &mainwindow_height));
  printf("MainWindow Size: %dx%d\n", mainwindow_width, mainwindow_height);

  DFBCHECK (m_mainwindow->SetColor (m_mainwindow, 0x80, 0x80, 0xff, 0xff));
  DFBCHECK (m_mainwindow->FillRectangle (m_mainwindow, 0, 0, 640, 720));
  DFBCHECK (m_mainwindow->SetColor (m_mainwindow, 0x80, 0xff, 0x80, 0xff));
  DFBCHECK (m_mainwindow->FillRectangle (m_mainwindow, 640, 0, 640, 720));
  
  DFBCHECK (m_mainwindow->Flip (m_mainwindow, NULL, 0));
  
  sleep(5);
  
  printf("Exiting...\n");
  m_mainwindow->Release(m_mainwindow);
  m_displaylayer->Release(m_displaylayer);
  m_screen->Release(m_screen);
  m_dfb->Release(m_dfb);
  
  return 0;
}

