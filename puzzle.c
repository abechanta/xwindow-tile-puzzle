/*
// puzzle.c
*/

/* compile command */
/* % cc -Aa -I/usr/include/X11R5 -L/usr/lib/X11R5 puzzle.c -o puzzle -lX11 */

/* ### Include Files ### */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include "puzzle.h"

/* ### Constant Define ### */

#define FONT_X        8                             /* for window infomation */
#define FONT_Y       16
#define W0_WIDTH    600
#define W0_HEIGHT   400
#define P_WIDTH     100
#define P_HEIGHT    100

#define SPACE1      100                                          /* for pix0 */
#define SPACE2       80                    /* W0_HEIGHT/5 */
#define SPACE3      (SPACE1-FONT_X*9)/2
#define SPACE4      (SPACE2+FONT_Y)/2

#define LINES        10                                          /* for pix1 */
#define SPACE5       50
#define SPACE6      360
#define SPACE7      (W0_WIDTH-FONT_X*59)/2
#define SPACE8      (SPACE6-SPACE5)/(LINES+1)
#define SPACE9      (W0_WIDTH-FONT_X*9)/2
#define SPACE10     (W0_HEIGHT-SPACE6+FONT_Y)/2+SPACE6

#define P0           30                                     /* for character */
#define P1           70
#define P2           50

#define POINT_R_X   SPACE1+P_WIDTH*2+P2                    /* for map rotate */
#define POINT_R_Y1  P0
#define POINT_R_Y2  P1
#define POINT_U_X1  SPACE1+P_WIDTH*1+P0
#define POINT_U_X2  SPACE1+P_WIDTH*1+P1
#define POINT_U_Y   P_HEIGHT+P2

#define HEAD          0                                        /* for map ID */
#define TAIL          1
#define TRI_UP        2
#define TRI_RIGHT     3
#define NONE          4

#define STAGE         9                                   /* for game system */
#define SW_MAX       32

/* ### Global Variables ### */

/* for X-Window */
Display *dpy;
Window  w0;
Pixmap  pix0, pix1;
Pixmap  p_head, p_tail, p_tri_up, p_tri_right, p_none;
unsigned long   black, white;
GC      gc, gc_b, gc_w;
XEvent  event;
XSizeHints hint;
Font    fon1, fon2;

static char     fontname1[] = "hp8.8x16";                /* constant strings */
static char     fontname2[] = "vrb-35";
static char     title[]     = "Head and Tail";
static char     cansel[]    = "   OK!   ";
static char     memo[]      = " M E M O ";
static char     name[]      = "puzzle";

static char     mes0[]      = "You should";                      /* messages */
static char     mes1[]      = "try again!";
static char     mes2[]      = "Push RESET";
static char     mes3[]      = "  ANSWER  ";
static char     mes4[]      = " STAGE    ";
static char     mes5[]      = "    CLEAR ";
static char     mes6[]      = " Too many ";
static char     mes7[]      = "  steps ! ";
static char     mes8[]      = "Try again ";
static char     mes9[]      = " without  ";
static char     mes10[]     = "'PLAYBACK'";

static char     box_w0[5][10] = {                        /* icon name (pix0) */
  " H E L P ",
  "S T A G E",
  "R E S E T",
  "PLAY BACK",
  " ANSWER  " };

static char     box_w1[LINES][60] = {                 /* help message (pix1) */
 /*12345678901234567890123456789012345678901234567890123456789*/
  "#### Rules ####                                            ",
  "    There are 9 tiles in the game field, and it's not found",
  "that each tile faces 'head' or 'tail' toward you. You must ",
  "make all tiles faced 'head' by using triangle buttons. Then",
  "you push your 'mouse 1(left) button', tiles are upside-down",
  "in the same row. Kuwasii-koto ha document-file wo mite ne. ",
  "#### Control ####                                          ",
  "    MOUSE 1(LEFT) BUTTON  ->  Rotate, Command Decidion, ...",
  "    MOUSE  OTHER  BUTTON  ->  Program Abort                ",
  "#### Version ####                          Ver.1.0         " };

static XSegment seg0[] = {                               /* icon line (pix0) */
         SPACE1-1, 0,          SPACE1-1, W0_HEIGHT-1,
  W0_WIDTH-SPACE1, 0,   W0_WIDTH-SPACE1, W0_HEIGHT-1,
  W0_WIDTH-SPACE1, SPACE2*1, W0_WIDTH-1, SPACE2*1,
  W0_WIDTH-SPACE1, SPACE2*2, W0_WIDTH-1, SPACE2*2,
  W0_WIDTH-SPACE1, SPACE2*3, W0_WIDTH-1, SPACE2*3,
  W0_WIDTH-SPACE1, SPACE2*4, W0_WIDTH-1, SPACE2*4,
  W0_WIDTH-SPACE1+4, SPACE2*0+4, W0_WIDTH-4, SPACE2*0+4,
  W0_WIDTH-SPACE1+4, SPACE2*1-4, W0_WIDTH-4, SPACE2*1-4,
  W0_WIDTH-SPACE1+4, SPACE2*1+4, W0_WIDTH-4, SPACE2*1+4,
  W0_WIDTH-SPACE1+4, SPACE2*2-4, W0_WIDTH-4, SPACE2*2-4,
  W0_WIDTH-SPACE1+4, SPACE2*2+4, W0_WIDTH-4, SPACE2*2+4,
  W0_WIDTH-SPACE1+4, SPACE2*3-4, W0_WIDTH-4, SPACE2*3-4,
  W0_WIDTH-SPACE1+4, SPACE2*3+4, W0_WIDTH-4, SPACE2*3+4,
  W0_WIDTH-SPACE1+4, SPACE2*4-4, W0_WIDTH-4, SPACE2*4-4,
  W0_WIDTH-SPACE1+4, SPACE2*4+4, W0_WIDTH-4, SPACE2*4+4,
  W0_WIDTH-SPACE1+4, SPACE2*5-4, W0_WIDTH-4, SPACE2*5-4,
};

static XSegment seg1[] = {                               /* help line (pix1) */
  0, SPACE5, W0_WIDTH-1, SPACE5,
  0, SPACE6, W0_WIDTH-1, SPACE6 };

static int      point[19][2] = {                     /* cos & sin for rotate */
  {  120*1.00, 12*0.00 },                  /*   0�K*/
  {  120*0.98, 12*0.17 },                  /*  10�K*/
  {  120*0.94, 12*0.34 },                  /*  20�K*/
  {  120*0.87, 12*0.50 },                  /*  30�K*/
  {  120*0.77, 12*0.64 },                  /*  40�K*/
  {  120*0.64, 12*0.77 },                  /*  50�K*/
  {  120*0.50, 12*0.87 },                  /*  60�K*/
  {  120*0.34, 12*0.94 },                  /*  70�K*/
  {  120*0.17, 12*0.98 },                  /*  80�K*/
  {  120*0.00, 12*1.00 },                  /*  90�K*/
  { -120*0.17, 12*0.98 },                  /* 100�K*/
  { -120*0.34, 12*0.94 },                  /* 110�K*/
  { -120*0.50, 12*0.87 },                  /* 120�K*/
  { -120*0.64, 12*0.77 },                  /* 130�K*/
  { -120*0.77, 12*0.64 },                  /* 140�K*/
  { -120*0.87, 12*0.50 },                  /* 150�K*/
  { -120*0.94, 12*0.34 },                  /* 160�K*/
  { -120*0.98, 12*0.17 },                  /* 170�K*/
  { -120*1.00, 12*0.00 } };                /* 180�K*/

static int      indexr[3][4][2] = {                 /* constant for H_rotate */
  { { -6,  6 }, { -4,  4 }, { -4, -4 }, { -6, -6 } },
  { { -1,  1 }, {  1, -1 }, {  1,  1 }, { -1, -1 } },
  { {  4, -4 }, {  6, -6 }, {  6,  6 }, {  4,  4 } } };

static int      indexu[3][4][2] = {                 /* constant for V_rotate */
  { { -6, -6 }, {  6, -6 }, {  4, -4 }, { -4, -4 } },
  { { -1, -1 }, {  1, -1 }, { -1,  1 }, {  1,  1 } },
  { {  4,  4 }, { -4,  4 }, { -6,  6 }, {  6,  6 } } };

/* for Game */
int     stage, max_stage;                                /* for stage select */
int     sw, sw_data[SW_MAX], sw_data_of, f_playback;        /* for play back */
int     array[4][4];                                               /* buffer */
static int      q[STAGE][4][4] = {                               /* question */
  {                                                               /* stage 0 */
    { TRI_RIGHT, HEAD     , TAIL     , HEAD      },
    { TRI_RIGHT, TAIL     , HEAD     , TAIL      },
    { TRI_RIGHT, HEAD     , TAIL     , HEAD      },
    { NONE     , TRI_UP   , TRI_UP   , TRI_UP    } },
  {                                                               /* stage 1 */
    { TRI_RIGHT, HEAD     , HEAD     , HEAD      },
    { TRI_RIGHT, TAIL     , HEAD     , HEAD      },
    { TRI_RIGHT, HEAD     , HEAD     , HEAD      },
    { NONE     , TRI_UP   , TRI_UP   , TRI_UP    } },
  {                                                               /* stage 2 */
    { TRI_RIGHT, HEAD     , TAIL     , HEAD      },
    { TRI_RIGHT, TAIL     , TAIL     , TAIL      },
    { TRI_RIGHT, TAIL     , TAIL     , TAIL      },
    { NONE     , TRI_UP   , TRI_UP   , TRI_UP    } },
  {                                                               /* stage 3 */
    { TRI_RIGHT, HEAD     , TAIL     , HEAD      },
    { TRI_RIGHT, TAIL     , HEAD     , HEAD      },
    { TRI_RIGHT, HEAD     , TAIL     , HEAD      },
    { NONE     , TRI_UP   , TRI_UP   , TRI_UP    } },
  {                                                               /* stage 4 */
    { TRI_RIGHT, TAIL     , HEAD     , HEAD      },
    { TRI_RIGHT, HEAD     , TAIL     , TAIL      },
    { TRI_RIGHT, TAIL     , TAIL     , HEAD      },
    { NONE     , TRI_UP   , TRI_UP   , TRI_UP    } },
  {                                                               /* stage 5 */
    { TRI_RIGHT, HEAD     , HEAD     , HEAD      },
    { TRI_RIGHT, TAIL     , HEAD     , HEAD      },
    { TRI_RIGHT, TAIL     , HEAD     , TAIL      },
    { NONE     , TRI_UP   , TRI_UP   , TRI_UP    } },
  {                                                               /* stage 6 */
    { TRI_RIGHT, HEAD     , TAIL     , TAIL      },
    { TRI_RIGHT, TAIL     , TAIL     , HEAD      },
    { TRI_RIGHT, HEAD     , TAIL     , TAIL      },
    { NONE     , TRI_UP   , TRI_UP   , TRI_UP    } },
  {                                                               /* stage 7 */
    { TRI_RIGHT, TAIL     , HEAD     , TAIL      },
    { TRI_RIGHT, TAIL     , HEAD     , HEAD      },
    { TRI_RIGHT, TAIL     , HEAD     , TAIL      },
    { NONE     , TRI_UP   , TRI_UP   , TRI_UP    } },
  {                                                               /* stage 8 */
    { TRI_RIGHT, HEAD     , TAIL     , HEAD      },
    { TRI_RIGHT, HEAD     , TAIL     , HEAD      },
    { TRI_RIGHT, TAIL     , TAIL     , TAIL      },
    { NONE     , TRI_UP   , TRI_UP   , TRI_UP    } } };

static int      a[STAGE][8] = {                                    /* answer */
  {  1,  4, -1, -1, -1, -1, -1, -1 },                             /* stage 0 */
  {  0,  3,  0, -1, -1, -1, -1, -1 },                             /* stage 1 */
  {  0,  3,  1,  2,  5, -1, -1, -1 },                             /* stage 2 */
  {  2,  5,  2,  4,  1, -1, -1, -1 },                             /* stage 3 */
  {  2,  3,  1, -1, -1, -1, -1, -1 },                             /* stage 4 */
  {  5,  0,  3,  0,  5, -1, -1, -1 },                             /* stage 5 */
  {  4,  1,  5,  1, -1, -1, -1, -1 },                             /* stage 6 */
  {  4,  0,  1,  3,  2, -1, -1, -1 },                             /* stage 7 */
  {  2,  3,  4,  0,  3, -1, -1, -1 } };                           /* stage 8 */

static int      num_a[STAGE] = { 2, 3, 5, 5, 3, 5, 4, 5, 5 };/* answer steps */

/* =-=-=-=-=-=-=-=-= */
int main()
{

  XInit();                             /* initialize X-Window & Window Raise */
  PixInit();                        /* initialize pixmap & copy to Window(0) */
  F_Reset(stage = max_stage = 0);                      /* initial stage is 0 */
  Draw_Field();                                           /* draw game field */
  Redraw(pix0);

  while (1) {                                                   /* main loop */
    if (XCheckMaskEvent(dpy, ExposureMask | ButtonPressMask, &event)) {
      if (event.type == Expose) {                                 /* re_draw */
	Redraw(pix0);
      }
      if (event.type == ButtonPress) {
	if (event.xbutton.button == Button1) {                   /* decidion */
	  if ((event.xmotion.x > W0_WIDTH-SPACE1)&&
	      (event.xmotion.y % SPACE2 != 0) ) {                    /* icon */
	    switch (event.xmotion.y / SPACE2) {
	    case 0:                                                  /* help */
	      Flush(pix0, W0_WIDTH-SPACE1, SPACE2*0, SPACE1, SPACE2+1);
	      F_Help();
	      break;
	    case 1:                                                 /* stage */
	      Flush(pix0, W0_WIDTH-SPACE1, SPACE2*1, SPACE1, SPACE2+1);
	      F_Stage();
	      break;
	    case 2:                                                 /* reset */
	      Flush(pix0, W0_WIDTH-SPACE1, SPACE2*2, SPACE1, SPACE2+1);
	      F_Reset(stage);
	      break;
	    case 3:                                             /* play back */
	      Flush(pix0, W0_WIDTH-SPACE1, SPACE2*3, SPACE1, SPACE2+1);
	      F_PlayBack();
	      break;
	    case 4:                                                /* answer */
	      Flush(pix0, W0_WIDTH-SPACE1, SPACE2*4, SPACE1, SPACE2+1);
	      F_Answer(stage);
	      break;
	    }
	  } else if (event.xmotion.x < SPACE1-1) {                /* a.so.bi */
	    XDrawString(dpy, w0, gc,
			event.xmotion.x-FONT_X/2, event.xmotion.y+FONT_Y/2-4,
			"+", 1);
	  } else {                                             /* tri_switch */
	    if ((SPACE1+P0 < event.xmotion.x)&&                 /* tri_right */
		(event.xmotion.x < SPACE1+P1)&&
		(P0 < event.xmotion.y % P_HEIGHT)&&
		(event.xmotion.y % P_HEIGHT < P1)) {
	      switch (event.xmotion.y / P_HEIGHT) {
	      case 0:                                       /* switch_ID = 0 */
		Flush(pix0,
		      SPACE1+P0, P0+P_HEIGHT*0, P1-P0+1, P1-P0+1);
		R_Rotate(0, 1);
		Check();
		break;
	      case 1:                                       /* switch_ID = 1 */
		Flush(pix0,
		      SPACE1+P0, P0+P_HEIGHT*1, P1-P0+1, P1-P0+1);
		R_Rotate(1, 1);
		Check();
		break;
	      case 2:                                       /* switch_ID = 2 */
		Flush(pix0,
		      SPACE1+P0, P0+P_HEIGHT*2, P1-P0+1, P1-P0+1);
		R_Rotate(2, 1);
		Check();
		break;
	      }
	    } else if ((P_HEIGHT*3+P0 < event.xmotion.y)&&         /* tri_up */
		       (event.xmotion.y < P_HEIGHT*3+P1)&&
		       (P0 < (event.xmotion.x-SPACE1) % P_WIDTH)&&
		       ((event.xmotion.x-SPACE1) % P_WIDTH < P1)) {
	      switch ((event.xmotion.x-SPACE1) / P_WIDTH) {
	      case 1:                                       /* switch_ID = 3 */
		Flush(pix0,
		      SPACE1+P_WIDTH*1+P0, P0+P_HEIGHT*3, P1-P0+1, P1-P0+1);
		U_Rotate(3, 1);
		Check();
		break;
	      case 2:                                       /* switch_ID = 4 */
		Flush(pix0,
		      SPACE1+P_WIDTH*2+P0, P0+P_HEIGHT*3, P1-P0+1, P1-P0+1);
		U_Rotate(4, 1);
		Check();
		break;
	      case 3:                                       /* switch_ID = 5 */
		Flush(pix0,
		      SPACE1+P_WIDTH*3+P0, P0+P_HEIGHT*3, P1-P0+1, P1-P0+1);
		U_Rotate(5, 1);
		Check();
		break;
	      }
	    }
	  }
	} else {                                                    /* abort */
	  Destroy();
	  exit(0);
	}
      }
    }
  }

} /* end of main */


/**/


void XInit()
{
  if ((dpy = XOpenDisplay(NULL)) == NULL) {                  /* Open Display */
    printf("Can't open display !\n");
    exit(0);
  }

  black = BlackPixel(dpy, 0);                              /* Default Pixels */
  white = WhitePixel(dpy, 0);

  w0 = XCreateSimpleWindow(dpy, RootWindow(dpy, 0),      /* Top-level Window */
			   100, 100, W0_WIDTH, W0_HEIGHT,
			   1, black, white);
  pix0 = XCreatePixmap(dpy, w0,
		       W0_WIDTH, W0_HEIGHT, DefaultDepth(dpy, 0));
  pix1 = XCreatePixmap(dpy, w0,
		       W0_WIDTH, W0_HEIGHT, DefaultDepth(dpy, 0));
  p_head      = XCreatePixmap(dpy, w0,
			      P_WIDTH, P_HEIGHT, DefaultDepth(dpy, 0));
  p_tail      = XCreatePixmap(dpy, w0,
			      P_WIDTH, P_HEIGHT, DefaultDepth(dpy, 0));
  p_tri_up    = XCreatePixmap(dpy, w0,
			      P_WIDTH, P_HEIGHT, DefaultDepth(dpy, 0));
  p_tri_right = XCreatePixmap(dpy, w0,
			      P_WIDTH, P_HEIGHT, DefaultDepth(dpy, 0));
  p_none      = XCreatePixmap(dpy, w0,
			      P_WIDTH, P_HEIGHT, DefaultDepth(dpy, 0));


  hint.flags = PMaxSize | PMinSize;                         /* Resize -> off */
  hint.max_width  = hint.min_width  = W0_WIDTH;
  hint.max_height = hint.min_height = W0_HEIGHT;
  XSetStandardProperties(dpy, w0, title, name, None, NULL, 0, &hint);

  gc   = XCreateGC(dpy, pix0, 0, NULL);                            /* gc Set */
  gc_b = XCreateGC(dpy, pix0, 0, NULL);
  gc_w = XCreateGC(dpy, pix0, 0, NULL);
  XSetFunction(dpy, gc, GXxor);
  XSetForeground(dpy, gc  , white);
  XSetForeground(dpy, gc_b, black);
  XSetForeground(dpy, gc_w, white);

  fon1 = XLoadFont(dpy, fontname1);                           /* font select */
  fon2 = XLoadFont(dpy, fontname2);
  XSetFont(dpy, gc  , fon1);
  XSetFont(dpy, gc_b, fon1);
  XSetFont(dpy, gc_w, fon1);

  XSelectInput(dpy, w0, ExposureMask | ButtonPressMask);     /* Select Input */

  XMapWindow(dpy, w0);                                          /* Map Raise */
  XFlush(dpy);

} /* end of XInit */


void PixInit()
{
  int  i;

  XFillRectangle(dpy, pix0, gc_w, 0, 0, W0_WIDTH, W0_HEIGHT);   /* Pix clear */
  XFillRectangle(dpy, pix1, gc_w, 0, 0, W0_WIDTH, W0_HEIGHT);
  XFillRectangle(dpy, pix1, gc_b, 0, 0, W0_WIDTH, SPACE5-1);
  XDrawSegments(dpy, pix0, gc_b, seg0, 16);                 /* Draw Box Line */
  XDrawSegments(dpy, pix1, gc_b, seg1, 2);

  XDrawString(dpy, pix0, gc_b,                         /* Draw Text for pix0 */
	      SPACE3, SPACE5,
	      memo, sizeof(memo)-1);
  for (i = 0; i < 5; i++) {
    XDrawString(dpy, pix0, gc_b,
		W0_WIDTH-SPACE1+SPACE3, SPACE2*i+SPACE4,
		box_w0[i], sizeof(box_w0[i])-1);
  }

  XSetFont(dpy, gc_w, fon2);                           /* Draw Text for pix1 */
  XDrawString(dpy, pix1, gc_w,
	      SPACE3, SPACE5,
	      title, sizeof(title)-1);
  XSetFont(dpy, gc_w, fon1);
  for (i = 0; i < LINES; i++) {
    XDrawString(dpy, pix1, gc_b,
		SPACE7, SPACE5+(i+1)*SPACE8,
		box_w1[i], sizeof(box_w1[i])-1);
  }
  XDrawString(dpy, pix1, gc_b,
	      SPACE9, SPACE10,
	      cansel, sizeof(cansel)-1);

                                                            /* Tile_Pix Init */
  XFillRectangle(dpy, p_head     , gc_w, 0, 0, P_WIDTH, P_HEIGHT);
  XFillRectangle(dpy, p_tail     , gc_w, 0, 0, P_WIDTH, P_HEIGHT);
  XFillRectangle(dpy, p_tri_up   , gc_w, 0, 0, P_WIDTH, P_HEIGHT);
  XFillRectangle(dpy, p_tri_right, gc_w, 0, 0, P_WIDTH, P_HEIGHT);
  XFillRectangle(dpy, p_none     , gc_w, 0, 0, P_WIDTH, P_HEIGHT);

  XDrawRectangle(dpy, p_head     , gc_b, P0  , P0  , P1-P0  , P1-P0  );
  XDrawRectangle(dpy, p_head     , gc_b, P0  , P0  , P1-P0-1, P1-P0-1);

  XFillRectangle(dpy, p_tail     , gc_b, P0  , P0  , P1-P0+1, P1-P0+1);
  XFillRectangle(dpy, p_tail     , gc  , P0+5, P0+5, P1-P0-9, P1-P0-9);
  XDrawLine(dpy, p_tail      , gc  , P0, P0, P1, P1);
  XDrawLine(dpy, p_tail      , gc  , P1, P0, P0, P1);
  XDrawString(dpy, p_tail, gc, 35, 85, "TAIL", sizeof("TAIL")-1);

  XDrawRectangle(dpy, p_tri_up    , gc_b, P0  , P0  , P1-P0  , P1-P0  );
  XDrawRectangle(dpy, p_tri_up    , gc_b, P0  , P0  , P1-P0+1, P1-P0+1);
  XDrawLine(dpy, p_tri_up    , gc_b, P0+2, P1-2, P1-2, P1-2);
  XDrawLine(dpy, p_tri_up    , gc_b, P2  , P0+2, P0+2, P1-2);
  XDrawLine(dpy, p_tri_up    , gc_b, P2  , P0+2, P1-2, P1-2);
  XDrawLine(dpy, p_tri_up    , gc_b, P2  , P2  , P2  , P0+2);

  XDrawRectangle(dpy, p_tri_right , gc_b, P0  , P0  , P1-P0  , P1-P0  );
  XDrawRectangle(dpy, p_tri_right , gc_b, P0  , P0  , P1-P0+1, P1-P0+1);
  XDrawLine(dpy, p_tri_right , gc_b, P0+2, P0+2, P0+2, P1-2);
  XDrawLine(dpy, p_tri_right , gc_b, P1-2, P2  , P0+2, P0+2);
  XDrawLine(dpy, p_tri_right , gc_b, P1-2, P2  , P0+2, P1-2);
  XDrawLine(dpy, p_tri_right , gc_b, P2  , P2  , P1-2, P2  );

  XFlush(dpy);

} /* end of PixInit */


void Redraw(Pixmap px)
{
  XCopyArea(dpy, px, w0, gc_b, 0, 0, W0_WIDTH, W0_HEIGHT, 0, 0);
  XFlush(dpy);

} /* end of Redraw */


void Destroy()
{
  XFreeGC(dpy, gc  );
  XFreeGC(dpy, gc_b);
  XFreeGC(dpy, gc_w);
  XDestroyWindow(dpy, w0);
  XCloseDisplay(dpy);

} /* end of Destroy */


void Flush(Pixmap px, int p_x, int p_y, int width, int height)
{
  int  i;

  for (i = 0; i < 6; i++) {
    XFillRectangle(dpy, px, gc, p_x, p_y, width, height);
    Redraw(px);
    Wait(1);
  }

} /* end of Flush */


void Wait(int intr)
{
  int  i, j;

  for (j = 0; j < intr; j++) {
    for (i = 0; i < 1000; i++) {
      XCheckMaskEvent(dpy, ExposureMask | ButtonPressMask, &event);
    }
  }

} /* end of Wait */


void Wait2()
{
  while (1) {
    if (XCheckMaskEvent(dpy, ExposureMask | ButtonPressMask, &event)) {
      break;
    }
  }

} /* end of Wait2 */


void F_Help()
{
  int  flag;

  flag = 0;
  Redraw(pix1);                                          /* draw help window */
  while (flag == 0) {                                           /* main loop */
    if (XCheckMaskEvent(dpy, ExposureMask | ButtonPressMask, &event)) {
      if (event.type == Expose) {                                 /* re_draw */
	Redraw(pix1);
      }
      if (event.type == ButtonPress) {
	if (event.xbutton.button == Button1) {                   /* decidion */
	  if (event.xmotion.y > SPACE6) {                          /* cansel */
	    flag++;
	    Flush(pix1, 0, SPACE6, W0_WIDTH, W0_HEIGHT-SPACE6);
	  } else {                                                /* a.so.bi */
	    XFillArc(dpy, w0, gc, event.xmotion.x-10, event.xmotion.y-10,
		     10*2-1, 10*2-1, 0*64, 360*64);
	    XDrawArc(dpy, w0, gc, event.xmotion.x-12, event.xmotion.y-12,
		     12*2-1, 12*2-1, 0*64, 360*64);
	  }
	} else {                                                    /* abort */
	  Destroy();
	  exit();
	}
      }
    }
  }
  Redraw(pix0);                                           /* draw game field */

} /* end of F_Help */


void F_Stage()
{
  if (stage < max_stage) {                                  /* to next stage */
    F_Reset(++stage);
  } else {                                                 /* to first stage */
    F_Reset(stage = 0);
  }

} /* end of F_Stage */


void F_Reset(int stg)
{
  int  i, j;

  sw_data_of = f_playback = 0;
  sw = 0;
  if ((0 <= stg)&&                                      /* initialize buffer */
      (stg < STAGE)) {
    for (i = 0; i < 4; i++) {
      for (j = 0; j < 4; j++) {
	array[i][j] = q[stg][i][j];
      }
    }
  }
                                                            /* message clear */
  XFillRectangle(dpy, p_none     , gc_w, 0, 0, P_WIDTH, P_HEIGHT);
  Draw_Field();
  Redraw(pix0);

} /* end of F_Reset */


void F_PlayBack()
{
  if (sw != 0) {                                            /* playback O.K. */
    f_playback = 1;
    if (sw_data[sw-1] <= 2) {
      R_Rotate(sw_data[--sw], 0);
    } else {
      U_Rotate(sw_data[--sw], 0);
    }
  } else if (sw_data_of == 1) {           /* playback array (sw_data) is o/f */
    XFillRectangle(dpy, p_none, gc_b,
		   5, 5, P_WIDTH-5*2, P_HEIGHT-5*2);
    XDrawString(dpy, p_none, gc,
		P2-FONT_X*5, P2+FONT_X*0,
		mes2, sizeof(mes2)-1);
  }

  Draw_Field();
  Redraw(pix0);

} /* end of F_PlayBack */


void F_Answer(int stg)
{
  int  i;

  i = 0;
  F_Reset(stg);
  XFillRectangle(dpy, p_none, gc_b,                               /* message */
		 5, 5, P_WIDTH-5*2, P_HEIGHT-5*2);
  XDrawString(dpy, p_none, gc,
	      P2-FONT_X*5, P2+FONT_X*0,
	      mes3, sizeof(mes3)-1);
  Draw_Field();
  Redraw(pix0);

  Wait(50);
  while ((a[stg][i] != -1)&&(i < 8)) {                       /* answer phase */
    if (a[stg][i] <= 2) {
      R_Rotate(a[stg][i++], 0);
    } else {
      U_Rotate(a[stg][i++], 0);
    }
    Wait(10);
  }
  Flush(pix0, SPACE1, P_HEIGHT*3, P_WIDTH, P_HEIGHT);
  Flush(pix0, SPACE1, P_HEIGHT*3, P_WIDTH, P_HEIGHT);

  F_Reset(stg);

} /* end of F_Answer */


void Draw_Field()
{
  int  i, j;

  for (j = 0; j < 4; j++) {
    for (i = 0; i < 4; i++) {
      switch (array[j][i]) {
      case HEAD:
	XCopyArea(dpy, p_head     , pix0, gc_b, 0, 0, P_WIDTH, P_HEIGHT,
		  SPACE1+P_WIDTH*i, P_HEIGHT*j);
	break;
      case TAIL:
	XCopyArea(dpy, p_tail     , pix0, gc_b, 0, 0, P_WIDTH, P_HEIGHT,
		  SPACE1+P_WIDTH*i, P_HEIGHT*j);
	break;
      case TRI_UP:
	XCopyArea(dpy, p_tri_up   , pix0, gc_b, 0, 0, P_WIDTH, P_HEIGHT,
		  SPACE1+P_WIDTH*i, P_HEIGHT*j);
	break;
      case TRI_RIGHT:
	XCopyArea(dpy, p_tri_right, pix0, gc_b, 0, 0, P_WIDTH, P_HEIGHT,
		  SPACE1+P_WIDTH*i, P_HEIGHT*j);
	break;
      case NONE:
	XCopyArea(dpy, p_none     , pix0, gc_b, 0, 0, P_WIDTH, P_HEIGHT,
		  SPACE1+P_WIDTH*i, P_HEIGHT*j);
	break;
      }
    }
  }

} /* end of Draw_Field */


void Rotate(int rotate_ID)
{
  int  dummy1, dummy2, dummy3;

  if ((0 <= rotate_ID)&&                                         /* H_rotate */
      (rotate_ID <= 2)) {
    dummy1 = array[rotate_ID][3];
    dummy2 = array[rotate_ID][2];
    dummy3 = array[rotate_ID][1];
    array[rotate_ID][1] = dummy1 == HEAD ? TAIL : HEAD;
    array[rotate_ID][2] = dummy2 == HEAD ? TAIL : HEAD;
    array[rotate_ID][3] = dummy3 == HEAD ? TAIL : HEAD;
  }
  if ((3 <= rotate_ID)&&                                         /* V_rotate */
      (rotate_ID <= 5)) {
    dummy1 = array[2][rotate_ID-2];
    dummy2 = array[1][rotate_ID-2];
    dummy3 = array[0][rotate_ID-2];
    array[0][rotate_ID-2] = dummy1 == HEAD ? TAIL : HEAD;
    array[1][rotate_ID-2] = dummy2 == HEAD ? TAIL : HEAD;
    array[2][rotate_ID-2] = dummy3 == HEAD ? TAIL : HEAD;
  }

} /* end of Rotate */


void R_Rotate(int rotate_ID, int mode)
{
  int  i, j, p;
  XPoint po[5];

  for (i = 0; i < 19; i++) {
    XFillRectangle(dpy, pix0, gc_w,
		   SPACE1+P_WIDTH, P_HEIGHT*rotate_ID, P_WIDTH*3, P_HEIGHT);

    for (j = 0; j < 3; j++) {
      po[0].x = (short)(POINT_R_X +
			point[i][0]*indexr[j][0][0]/6);
      po[0].y = (short)(POINT_R_Y1+
			point[i][1]*indexr[j][0][1]/6+P_HEIGHT*rotate_ID);
      po[1].x = (short)(POINT_R_X +
			point[i][0]*indexr[j][1][0]/6);
      po[1].y = (short)(POINT_R_Y1+
			point[i][1]*indexr[j][1][1]/6+P_HEIGHT*rotate_ID);
      po[2].x = (short)(POINT_R_X +
			point[i][0]*indexr[j][2][0]/6);
      po[2].y = (short)(POINT_R_Y2+
			point[i][1]*indexr[j][2][1]/6+P_HEIGHT*rotate_ID);
      po[3].x = (short)(POINT_R_X +
			point[i][0]*indexr[j][3][0]/6);
      po[3].y = (short)(POINT_R_Y2+
			point[i][1]*indexr[j][3][1]/6+P_HEIGHT*rotate_ID);
      po[4].x = (short)(POINT_R_X +
			point[i][0]*indexr[j][0][0]/6);
      po[4].y = (short)(POINT_R_Y1+
			point[i][1]*indexr[j][0][1]/6+P_HEIGHT*rotate_ID);
      p = 5;
      if (array[rotate_ID][j+1] == (i <= 9 ? HEAD : TAIL)) {
	XDrawLines(dpy, pix0, gc_b, po, p, CoordModeOrigin);
      } else {
	XFillPolygon(dpy, pix0, gc_b, po, p, Convex, CoordModeOrigin);
	XDrawLine(dpy, pix0, gc,
		  POINT_R_X +point[i][0]*indexr[j][0][0]/6,
		  POINT_R_Y1+point[i][1]*indexr[j][0][1]/6+P_HEIGHT*rotate_ID,
		  POINT_R_X +point[i][0]*indexr[j][2][0]/6,
		  POINT_R_Y2+point[i][1]*indexr[j][2][1]/6+P_HEIGHT*rotate_ID);
	XDrawLine(dpy, pix0, gc,
		  POINT_R_X +point[i][0]*indexr[j][1][0]/6,
		  POINT_R_Y1+point[i][1]*indexr[j][1][1]/6+P_HEIGHT*rotate_ID,
		  POINT_R_X +point[i][0]*indexr[j][3][0]/6,
		  POINT_R_Y2+point[i][1]*indexr[j][3][1]/6+P_HEIGHT*rotate_ID);
      }
    }
    Redraw(pix0);
    Wait(1);
  }

  Rotate(rotate_ID);
  if (mode == 1) {                  /* mode==1 -> store rotate_ID to sw_data */
    SW_Add(rotate_ID);
  }                                               /* mode==0 -> force Rotate */
  Draw_Field();
  Redraw(pix0);

} /* end of R_Rotate */


void U_Rotate(int rotate_ID, int mode)
{
  int  i, j, p;
  XPoint po[5];

  for (i = 0; i < 19; i++) {
    XFillRectangle(dpy, pix0, gc_w,
		   SPACE1+P_WIDTH*(rotate_ID-2), 0, P_WIDTH, P_HEIGHT*3);

    for (j = 0; j < 3; j++) {
      po[0].x = (short)(POINT_U_X1+
			point[i][1]*indexu[j][0][0]/6+P_WIDTH*(rotate_ID-3));
      po[0].y = (short)(POINT_U_Y +
			point[i][0]*indexu[j][0][1]/6);
      po[1].x = (short)(POINT_U_X2+
			point[i][1]*indexu[j][1][0]/6+P_WIDTH*(rotate_ID-3));
      po[1].y = (short)(POINT_U_Y +
			point[i][0]*indexu[j][1][1]/6);
      po[2].x = (short)(POINT_U_X2+
			point[i][1]*indexu[j][2][0]/6+P_WIDTH*(rotate_ID-3));
      po[2].y = (short)(POINT_U_Y +
			point[i][0]*indexu[j][2][1]/6);
      po[3].x = (short)(POINT_U_X1+
			point[i][1]*indexu[j][3][0]/6+P_WIDTH*(rotate_ID-3));
      po[3].y = (short)(POINT_U_Y +
			point[i][0]*indexu[j][3][1]/6);
      po[4].x = (short)(POINT_U_X1+
			point[i][1]*indexu[j][0][0]/6+P_WIDTH*(rotate_ID-3));
      po[4].y = (short)(POINT_U_Y +
			point[i][0]*indexu[j][0][1]/6);
      p = 5;
      if (array[j][rotate_ID-2] == (i <= 9 ? HEAD : TAIL)) {
	XDrawLines(dpy, pix0, gc_b, po, p, CoordModeOrigin);
      } else {
	XFillPolygon(dpy, pix0, gc_b, po, p, Convex, CoordModeOrigin);
	XDrawLine(dpy, pix0, gc,
		POINT_U_X1+point[i][1]*indexu[j][0][0]/6+P_WIDTH*(rotate_ID-3),
		POINT_U_Y +point[i][0]*indexu[j][0][1]/6,
		POINT_U_X2+point[i][1]*indexu[j][2][0]/6+P_WIDTH*(rotate_ID-3),
		POINT_U_Y +point[i][0]*indexu[j][2][1]/6);
	XDrawLine(dpy, pix0, gc,
		POINT_U_X2+point[i][1]*indexu[j][1][0]/6+P_WIDTH*(rotate_ID-3),
		POINT_U_Y +point[i][0]*indexu[j][1][1]/6,
		POINT_U_X1+point[i][1]*indexu[j][3][0]/6+P_WIDTH*(rotate_ID-3),
		POINT_U_Y +point[i][0]*indexu[j][3][1]/6);
      }
    }
    Redraw(pix0);
    Wait(1);
  }

  Rotate(rotate_ID);
  if (mode == 1) {                  /* mode==1 -> store rotate_ID to sw_data */
    SW_Add(rotate_ID);
  }                                               /* mode==0 -> force Rotate */
  Draw_Field();
  Redraw(pix0);

} /* end of U_Rotate */


void SW_Add(int rotate_ID)
{
  int  i;

  if (sw < SW_MAX) {                                      /* store rotate_ID */
    sw_data[sw++] = rotate_ID;
  } else {                                        /* shift & store rotate_ID */
    sw_data_of = 1;
    for (i = 0; i < SW_MAX-1; i++) {
      sw_data[i] = sw_data[i+1];
    }
    sw_data[i] = rotate_ID;

    XFillRectangle(dpy, p_none, gc_b,                 /* and warning message */
		   5, 5, P_WIDTH-5*2, P_HEIGHT-5*2);
    XDrawString(dpy, p_none, gc,
		P2-FONT_X*5, P2-FONT_Y*1+4,
		mes0, sizeof(mes0)-1);
    XDrawString(dpy, p_none, gc,
		P2-FONT_X*5, P2+FONT_Y*1+4,
		mes1, sizeof(mes1)-1);
  }

} /* end of SW_Add */


void Check()
{
  int  i, j, f_check;

  f_check = 0;
  for (i = 0; i < 3; i++) {                     /* Do all tiles face 'HEAD'? */
    for (j = 0; j < 3; j++) {
      f_check += array[i][j+1];
    }
  }

  if (f_check == 0) {                               /* all tiles face "HEAD" */
    if ((sw_data_of == 0)&&
	(f_playback == 0)) {
      if (sw <= num_a[stage]) {                               /* stage clear */
	stage = stage < STAGE-1 ? stage+1 : 0;              /* to next stage */
	max_stage = max_stage < stage ? stage : max_stage;
	XFillRectangle(dpy, p_none, gc_b,
		       5, 5, P_WIDTH-5*2, P_HEIGHT-5*2);
	XDrawString(dpy, p_none, gc,
		    P2-FONT_X*5, P2-FONT_Y*1+4,
		    mes4, sizeof(mes4)-1);
	XDrawString(dpy, p_none, gc,
		    P2-FONT_X*5, P2+FONT_Y*1+4,
		    mes5, sizeof(mes5)-1);
	Draw_Field();
	Redraw(pix0);
	Flush(pix0, SPACE1, P_HEIGHT*3, P_WIDTH, P_HEIGHT);
	Flush(pix0, SPACE1, P_HEIGHT*3, P_WIDTH, P_HEIGHT);
	Flush(pix0, SPACE1, P_HEIGHT*3, P_WIDTH, P_HEIGHT);
      } else {                                             /* too many steps */
	XFillRectangle(dpy, p_none, gc_b,
		       5, 5, P_WIDTH-5*2, P_HEIGHT-5*2);
	XDrawString(dpy, p_none, gc,
		    P2-FONT_X*5, P2-FONT_Y*1+4,
		    mes6, sizeof(mes6)-1);
	XDrawString(dpy, p_none, gc,
		    P2-FONT_X*5, P2+FONT_Y*1+4,
		    mes7, sizeof(mes7)-1);
      }
    } else {                                        /* used play_back button */
      XFillRectangle(dpy, p_none, gc_b,
		     5, 5, P_WIDTH-5*2, P_HEIGHT-5*2);
      XDrawString(dpy, p_none, gc,
		  P2-FONT_X*5, P2-FONT_Y*2+4,
		  mes8, sizeof(mes8)-1);
      XDrawString(dpy, p_none, gc,
		  P2-FONT_X*5, P2+FONT_Y*0+4,
		  mes9, sizeof(mes9)-1);
      XDrawString(dpy, p_none, gc,
		  P2-FONT_X*5, P2+FONT_Y*2+4,
		  mes10, sizeof(mes10)-1);
    }
    Draw_Field();
    Redraw(pix0);

    Wait(1);
    Wait2();
    F_Reset(stage);
  }

} /* end of Check */
