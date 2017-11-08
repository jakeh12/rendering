#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include "font.h"

#define FONT_WIDTH 8
#define FONT_HEIGHT 8

Display *display;
Window win;
GC gc;
int screen_num;

void render_character(char character, int x, int y) {
    for (int row = 0; row < FONT_HEIGHT; row++) {
        char pixel_row = font_map[character][row];
        for (int column = 0; column < FONT_WIDTH; column++) {
            if ((pixel_row >> column) & 0x01) {
                XSetForeground(display, gc, WhitePixel(display, screen_num));
            }
            else {
                XSetForeground(display, gc, BlackPixel(display, screen_num));
            }
            XDrawPoint(display, win, gc, x + column, y + row);
        }
    }
}

void render_string(char* string, int x, int y) {
    int original_x = x;
    int original_y = y;
    while(*string != '\0') {
        switch(*string) {
            case '\n':
                y = y + FONT_HEIGHT;
                // x = original_x - FONT_WIDTH;
                break;
            case '\r':
                x = original_x - FONT_WIDTH;
                break;
            case '\t':
                x = x + (FONT_WIDTH * 8);
                break;
            default:
                break;
        }
        render_character(*string++, x, y);
        x = x + FONT_WIDTH;
    }
}

int main(int argc,char **argv)
{
	display = XOpenDisplay(NULL);
	screen_num = DefaultScreen(display);
	Window root = RootWindow(display, screen_num);

	win = XCreateSimpleWindow(display, root, 0, 0, 640, 480, 2,
		WhitePixel(display, screen_num),BlackPixel(display, screen_num));

	XMapWindow(display, win);
	XSelectInput(display, win, ExposureMask | KeyPressMask);
	gc = XCreateGC(display, win, 0, NULL);
	XEvent event;
	char *string = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n\rAliquam luctus tristique tellus, id pulvinar felis. Maecenas non metus.";
	while(1)
	{
		XNextEvent(display, &event);
		switch(event.type)
		{
			case Expose:
				XSetForeground(display, gc, WhitePixel(display, screen_num));
				render_string(string, 0, 0);
				break;
			case KeyPress:
				if(event.xkey.keycode == XKeysymToKeycode(display, XK_Escape))
				{
					XCloseDisplay(display);
					exit(EXIT_SUCCESS);
				}
		}
	}
	return 0;
}

