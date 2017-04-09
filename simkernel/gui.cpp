#include "gui.h"
#include <stdio.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include "address.h"

#if _MULTI_THREAD_BUILD
pthread_mutex_t mutex_fb = PTHREAD_MUTEX_INITIALIZER;
#endif

ADDR fb_base;
ADDR fb_last;
Gui* fb_gui;
BOOL repaint_display;

#if _SIMULATE_LCD
void *paint(void *pParams) {
	INT width = fb_gui->width();
	INT height = fb_gui->height();
	UINT pixel = 0;
	Display* dpy = fb_gui->display();
	Window	win = fb_gui->win();
	GC gc = fb_gui->gc();
	UINT flag = 0;

  	while(fb_gui->stop()==FALSE)
  	{ 
  		flag = fb_gui->getWord(FB_POU_READ_CTRL_OFFSET);
//  		printf("%d\n", flag);
  		if(CHECK_POU_READ(flag)) {
	  		INT i = fb_gui->fbBase();
			for (INT y=0; y<height; y++) {
		    	for (INT x=0; x<width; x++) {
		    		pixel = X11_COLOUR(fb_gui->getWord(i));
		    		XSetForeground(dpy, gc, pixel);
		    		XDrawPoint(dpy, win, gc, x, y);
		    		i+=WORD_BYTE;
		    	}
			}
			SET_POU_READ(flag)
  		}
   		usleep(PAINT_SLEEP_TIME); 
  	}	
  	pthread_exit(NULL);
}

void *paint_vnc(void *pParams) {
	INT width = fb_gui->width();
	INT height = fb_gui->height();
	UINT pixel = 0;
	Display* dpy = fb_gui->display();
	Window	win = fb_gui->win();
	GC gc = fb_gui->gc();
	UINT flag = 0;

  	while(fb_gui->stop()==FALSE)
  	{ 
  		flag = fb_gui->getWord(FB_POU_READ_CTRL_OFFSET);
//  		printf("%d\n", flag);
  		if(CHECK_POU_READ(flag)) {
	  		INT i = fb_gui->fbBase();
			for (INT y=0; y<height; y++) {
		    	for (INT x=0; x<width; x++) {
		    		pixel = X11_COLOUR_VNC(fb_gui->getWord(i));
		    		XSetForeground(dpy, gc, pixel);
		    		XDrawPoint(dpy, win, gc, x, y);
					i+=WORD_BYTE;
		    	}
			}
			SET_POU_READ(flag)
  		}
   		usleep(PAINT_SLEEP_TIME); 
  	}	
  	pthread_exit(NULL);
}
#endif

Gui::Gui(Memory* mem, ADDR start, UINT size) : InterfaceHandler(mem, start, size) {
	fb_gui = this;
	_width = 0;
	_height = 0;		
}

BOOL Gui::trunOn(void) { 
	if(stop()==TRUE) {
		start();
		return TRUE;
	}
	return FALSE;
}
BOOL Gui::trunOff(void) { 
	if(stop()==FALSE) {
		end();
		return TRUE;
	}
	return FALSE;
}	

void Gui::clear() {
	INT i = fbBase();	
	for (INT y=0; y<_height; y++) {
    	for (INT x=0; x<_width; x++) {
    		setWord(i, defaultVal());
    		i+=WORD_BYTE;
    	}
	}	
}

void Gui::init(INT w, INT h) {
	_width = w;
	_height = h;	
	stop(TRUE);
	setWord(FB_POU_READ_CTRL_OFFSET, 0);
	setWord(FB_BASE_OFFSET, FB_BASE_START);
	setWord(FB_WIDTH_OFFSET, w);
	setWord(FB_HEIGHT_OFFSET, h);
	setWord(FB_DRAWABLE_WIDTH_OFFSET, w);
	setWord(FB_DRAWABLE_HEIGHT_OFFSET, h);
	setWord(FB_LCD_ON_OFFSET, 0);

	_fbBase = offsetAddr(FB_BASE_START);
	clear();
}

void Gui::start() {   
	stop(FALSE);	
	repaint_display = FALSE;
#if _SIMULATE_LCD

	char name[80];
	sprintf(name,"SL GUI emulator ver 1.0 (%d * %d * %d bpp)", _width, _height, BITS_PER_PIXEL);
	_display = XOpenDisplay(NULL);

	if (_display == NULL) {
		fprintf(stderr, "Failed to open display\n");
	}

	_screen = DefaultScreen(_display);
	_depth = DefaultDepth(_display, _screen);
	_colormap = DefaultColormap(_display, _screen);

	_rootwin = RootWindow(_display, _screen);
	_win = XCreateSimpleWindow(_display, _rootwin, 10, 10, _width, _height, 2,
	BlackPixel(_display, _screen), BlackPixel(_display, _screen));

	XSetWindowColormap(_display, _win, _colormap);

	_gc = XCreateGC(_display, _win, 0, &_values);
	
	_sizehints.flags = PSize | PMinSize | PMaxSize;
	_sizehints.min_width = _width;
	_sizehints.max_width = _width;
	_sizehints.min_height = _height;
	_sizehints.max_height = _height;
	XSetStandardProperties(_display, _win, name, NULL, None, 0, 0, &_sizehints);

	XSelectInput(_display, _win, ButtonPressMask | KeyPressMask);
	XMapWindow(_display, _win);
	XSync(_display, 0);	
	
	setWord(FB_LCD_ON_OFFSET, 1);
	
	if(strncmp(":0", getenv("DISPLAY"), 2)==0) {
		if(pthread_create(&_thread_lcd, NULL,  paint, (void*) NULL)!=0) {
			printf("cannot create lcd thread");
			exit(0);
		}
	}
	else {
		if(pthread_create(&_thread_lcd, NULL,  paint_vnc, (void*) NULL)!=0) {
			printf("cannot create lcd thread");
			exit(0);
		}			
	}
#endif
}

void Gui::end() {   
	stop(TRUE);	
#if _SIMULATE_LCD
	INT retval;
	while(pthread_join(_thread_lcd, (void**) &retval))
		break;
	XCloseDisplay(_display);
	setWord(FB_LCD_ON_OFFSET, 0);
#endif	
}

