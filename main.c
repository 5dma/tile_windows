#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <glib-2.0/glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SPACING 20 /* Spacing between each window's top-left corner. */

/* A record containing pertinent information about a visible window. */
typedef struct Window_Info{
	Window window;
	int x;
	int y;
	int width;
	int height;
} Window_Info;

/* A struct containing overall layout information for the tiled windows. */
typedef struct Layout_Info {
	Window_Info *background; /* Pointer to the background window. */
	Window_Info *panel; /* Pointer to the panel. */
	int height; /* Net height available for adjusting window sizes, background height - panel height. */
	int current_window_position; /* Counter for the current window. Multiplied by SPACING to get the actual position. */
	Display *display; /* XWindow display. */
} Layout_Info;


void move_resize_windows(gpointer data,  gpointer user_data) {
	Layout_Info *layout_info = (Layout_Info *) user_data;
	Window_Info *window_info = (Window_Info *)data;
	layout_info->current_window_position++;
	/* Don't move the background or the panel. */
	if (window_info == layout_info->background) {
		return;
	}
	if (window_info == layout_info->panel) {
		return;
	}
	
	unsigned int spacing = layout_info->current_window_position * SPACING;
	XMoveWindow(layout_info->display, window_info->window,spacing, spacing);
	/* If the moved window is wider than the background width, make it narrower. */
	if (window_info->width + (spacing) > layout_info->background->width) {
		XResizeWindow(layout_info->display, window_info->window,  layout_info->background->width - spacing, window_info->height);
	}
}


int main(int argc, char *argv[]) {
	/* Passed as user data to move_resize_windows(). */
	Layout_Info layout_info;
	layout_info.current_window_position = -1;

	/* Get the local display. */
	layout_info.display = XOpenDisplay(NULL);
	if (layout_info.display == NULL) {
		printf("Could not connect to the display server. Exiting.\n");
		return -1;
	}
	
	/* Get the root window. */
	Window root = DefaultRootWindow(layout_info.display);
	
	/* Set up variables passed to XQueryTree(). */
	Window root_return;
	Window parent_return;
	Window *children_return;
	Status status;
	unsigned int nchildren_return;
	
	status = XQueryTree(layout_info.display, root, &root_return, &parent_return, &children_return, &nchildren_return);
	if (status == 0) {
		printf("Fatal error. XQueryTree failed with a status %d. Exiting.\n", status);
		exit(-1);
	}

	/* Holds a GSList of visible windows. */
	GSList *visible_windows = NULL;
	/* Holds attributes retrieved from a window. */
	XWindowAttributes x_window_attributes;

	/* Loop over all windows discovered by XQueryTree, which is typically quite a bit more than the visible windows. */
	for (unsigned int i = 0; i < nchildren_return; i++) {
		status = XGetWindowAttributes(layout_info.display , children_return[i], &x_window_attributes);
		if (status != 0) {
			
			/* Filter for only those windows that are visible. See 
				https://stackoverflow.com/questions/78701376/xquerytree-returns-wrong-number-of-child-windows */
			if ((x_window_attributes.map_state == IsViewable) && (x_window_attributes.override_redirect == 0)) {
				/* Create a new Window_Info for this visible window, and add it to the GS List. */
				Window_Info *window_info = (Window_Info *) g_malloc(sizeof(Window_Info));
				window_info->window = children_return[i];
				window_info->x = x_window_attributes.x;
				window_info->y = x_window_attributes.y;
				window_info->height = x_window_attributes.height;
				window_info->width = x_window_attributes.width;
				visible_windows = g_slist_append(visible_windows, window_info);
			}
		} else {
			printf("Got a status error on XGetWindowAttributes\n");
		}
	}

	/* Save information about the first window in the list (Background) and the
	last window in the list (panel). */
	gpointer trash = g_slist_nth_data (visible_windows, 0);
	Window_Info *background_info = (Window_Info *) trash;
	/* Using g_slist_last() here doesn't work; why not? */
	unsigned int list_length = g_slist_length(visible_windows);
	gpointer trash2 = g_slist_nth_data (visible_windows, list_length - 1);
	Window_Info *panel_info = (Window_Info *) trash2;
	layout_info.background = background_info;
	layout_info.panel = panel_info;
	layout_info.height = background_info->height - panel_info->height;

	/* Go move the windows.*/
	g_slist_foreach (visible_windows, move_resize_windows, &layout_info);

	XCloseDisplay(layout_info.display );
	return 0;
}
