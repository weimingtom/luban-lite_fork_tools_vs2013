#ifndef SDL_API_H
#define SDL_API_H

extern int MainFrameGetMsg(void);
extern void MainFrameRefresh(void);
extern void CanvasSetColor(int color);
extern void CanvasDrawPixel(int x1, int y1, int color, int alpha);
extern void MiscTrace(const char *fmt, ... );
extern void MiscSleep(int n);
extern void CanvasLock(void);
extern void CanvasUnlock(void);
extern void CanvasFillRect(int x1, int y1, int x2, int y2, int color);
extern void MainFrameResize(int w, int h);
extern int MouseGetMouseStatus(void);

#ifndef printf
#define printf MiscTrace
#endif

#ifndef WINDOW_BGCOLOR
#define WINDOW_BGCOLOR 0xffffff
#endif


#ifndef PEN_DOWN
#define PEN_DOWN 0xC0
#endif

#ifndef PEN_MOVE
#define PEN_MOVE 0x90
#endif

#ifndef PEN_UP
#define PEN_UP 0x20
#endif

#ifndef PEN_LEAVE
#define PEN_LEAVE 0x8
#endif

#define MY_DISP_HOR_RES 480
#define MY_DISP_VER_RES 320

#endif