#include <headers.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
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
	int screen_count = XScreenCount( display );

	Window root = DefaultRootWindow(display);
	Window root_return;
	Window parent;
	Window *children = NULL;
	unsigned int nchildren;
	Status status = XQueryTree(display, root, &root_return, &parent, &children, &nchildren);
	XWindowAttributes window_attributes;
	XTextProperty text_property;
	int visible = 0;
	printf("class depth border_width map_state\n");
	for (int i = 0; i < nchildren; i++) {
		Status status = XGetWindowAttributes(display, children[i], &window_attributes);
		printf("%d %d %d %d\n", window_attributes.class, window_attributes.depth, window_attributes.border_width, window_attributes.map_state);
		if ((window_attributes.map_state == IsViewable) && (window_attributes.class == InputOutput)) {
		//if ((window_attributes.class == 1) && (window_attributes.map_state == 2))
			Status inside_status = XGetWMName(display, children[i], &text_property);
			visible++;
		}
	}
	printf("Number of visible windows %d\n", visible);
	XFree(children);
	XCloseDisplay(display);
	return 0;
}