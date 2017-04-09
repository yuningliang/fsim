
#ifndef GUI_H_
#define GUI_H_


#include "simdefs.h"
#include "address.h"
#include "ifhandler.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define _POU_CONTROL 1

#ifndef _SIMULATE_LCD
	#define _SIMULATE_LCD 0
#endif

#define BITS_PER_PIXEL 32
#define POU_READ_TOGGLE	0x1
#define EVENT_SLEEP_TIME 4000
#define PAINT_SLEEP_TIME 5000

#if _POU_CONTROL
#define CHECK_POU_READ(flag)  ((flag&POU_READ_TOGGLE)>0)
#define SET_POU_READ(flag) fb_gui->setWord(FB_POU_READ_CTRL_OFFSET, (flag&(~POU_READ_TOGGLE)));
#else
#define CHECK_POU_READ(flag) 1
#define SET_POU_READ(flag)
#endif

#define X11_COLOUR(colour) ((UINT) (((colour&0xff000000)>>24)|((colour&0xff0000)>>8)|((colour&0xff00)<<8)|0))
#define X11_COLOUR_VNC(colour) ((UINT) (((colour&0xff000000)>>8)|((colour&0xff0000)>>8)|((colour&0xff00)>>8)|0))
#define FB_COLOUR(colour) ((UINT) (((colour&0xff)<<24)|((colour&0xff00)<<8)|((colour&0xff0000)>>8)|0))

class Gui : public InterfaceHandler {
	private:
		Display 	*_display;
		Colormap    _colormap;
		GC 		    _gc;
		Window		_win;
		Window		_rootwin;
		INT 		_depth;
		INT			_screen;
		BOOL		_stop;
		INT			_width;
		INT			_height;
		pthread_t	_thread_lcd;
		pthread_t	_thread_event;
		XSizeHints 	_sizehints;
		XGCValues 	_values;
		
		ADDR _fbBase;
		UINT _defaultVal;

	public:
		Gui(Memory* mem, ADDR start, UINT size);
		BOOL trunOn(void);
		BOOL trunOff(void);
		void init(INT, INT);
		Display* display() { return _display; }
		Colormap colormap() { return _colormap; }
		GC gc() { return _gc; }
		Window win() { return _win; }
		Window	rootwin() { return _rootwin; }
		INT depth() { return _depth; }
		INT	screen() { return _screen; }
		BOOL stop() { return _stop; }
		void stop(BOOL t) { _stop = t; }
		INT width() { return _width; }
		INT height() { return _height; }
		
		void writeFrameBuffer(INT x, INT y, WORD colour);
		WORD readFrameBuffer(INT x, INT y);
		void triggerRefresh();
	
		void start();	
		void end();

		void readHandler(const ADDR addr, UINT value) {}; 
		void writeHandler(const ADDR addr, UINT value, WORD data) {}; 
	
		//framebuffer related
		BOOL closeFB(void);
		ADDR fbBase(void) { return _fbBase; }
		UINT defaultVal(void) { return _defaultVal; }
		void defaultVal(UINT d) { _defaultVal = d; }
		void clear(void);
		void setRange(ADDR start, ADDR end) {
//			InterfaceHandler::setRange(start, end);
			init(FB_LCD_WIDTH, FB_LCD_HEIGHT);
		}		
};


#endif /*GUI_H_*/
