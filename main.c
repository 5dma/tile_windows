#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <glib-2.0/glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SPACING 20

typedef struct Window_Info{
	Window window;
	int x;
	int y;
	int width;
	int height;
} Window_Info;

typedef struct Layout_Info {
	Window_Info *background;
	Window_Info *panel;
	int width;
	int height;
	int current_window_position;
} Layout_Info;


void move_windows(gpointer data,  gpointer user_data) {
	Layout_Info *layout_info = (Layout_Info *) user_data;
	Window_Info *window_info = (Window_Info *)data;
	layout_info->current_window_position++;
	if (window_info == layout_info->background) {
		return;
	}
	if (window_info == layout_info->panel) {
		return;
	}
	printf("%d\t%d\t%d\t%d\t%d\n",layout_info->current_window_position,
							window_info->x,
								window_info->y,
								window_info->width,
								window_info->height);

}




int main(int argc, char *argv[]) {
	Display *display = XOpenDisplay(NULL);
	if (display == NULL) {
		printf("Could not connect to the display server. Exiting.\n");
		return -1;
	}
	Window root = DefaultRootWindow(display);

	Window root_return;
	Window parent_return;
	Window *children_return;
	Status status;
	unsigned int nchildren_return;
	GSList *visible_windows = NULL;
	status = XQueryTree(display, root, &root_return, &parent_return, &children_return, &nchildren_return);
	if (status == 0) {
		printf("Fatal error. XQueryTree failed with a status %d\n", status);
		exit(-1);
	}

	XWindowAttributes x_window_attributes;
	Layout_Info layout_info;
	layout_info.current_window_position = -1;


	for (unsigned int i = 0; i < nchildren_return; i++) {
		status = XGetWindowAttributes(display, children_return[i], &x_window_attributes);
		if (status != 0) {
			if ((x_window_attributes.map_state == IsViewable) && (x_window_attributes.override_redirect == 0)) {
				printf("Found a window %d\n", i);
				printf("%d\t%d\t%d\t%d\n",x_window_attributes.x,
								x_window_attributes.y,
								x_window_attributes.width,
								x_window_attributes.height);
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

	int omg = g_slist_length(visible_windows);
	printf("There are %d elements in this list\n", omg);
	gpointer trash = g_slist_nth_data (visible_windows, 0);
	Window_Info *background_info = (Window_Info *) trash;
	gpointer trash2 = g_slist_nth_data (visible_windows, omg - 1);
	Window_Info *panel_info = (Window_Info *) trash2;
	layout_info.background = background_info;
	layout_info.panel = panel_info;
	layout_info.width = background_info->width;
	layout_info.height = background_info->height - panel_info->height;

	g_slist_foreach (visible_windows, move_windows, &layout_info);

	XCloseDisplay(display);
	return 0;
}
