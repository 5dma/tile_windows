#include <headers.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>

#define NUMBER_MATCHES 5

int main(int argc, char *argv[]) {
	printf("hello\n");
	Display * display = XOpenDisplay(NULL);
	if (display == NULL) {
		printf("Could not connect to the display server. Exiting.\n");
		return -1;
	}

	int screen = XDefaultScreen(display);
	int display_height = DisplayHeight(display, screen); /* Display height/width still needed for hiding clients */
    int display_width = DisplayWidth(display, screen);

	XCloseDisplay(display);
	return 0;
}