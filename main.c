#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <glib-2.0/glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct Window_Info{
	Window window;
	int x;
	int y;
	int width;
	int height;
} Window_Info;



void move_windows(gpointer data,  gpointer user_data) {
	printf("Top level\n");
	Window_Info *window_info = (Window_Info *)data;
	printf("%d\t%d\t%d\t%d\n",window_info->x,
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
	Window current_window;
	XWindowAttributes x_window_attributes;

	XWindowAttributes *background;
	XWindowAttributes *panel;

	for (unsigned int i = 0; i < nchildren_return; i++) {
		status = XGetWindowAttributes(display, children_return[i], &x_window_attributes);
		if (status != 0) {
			/* 			fprintf(outfile, "%ld\t%d\t%d\t%d\t%d\t%d\t%d\t%ld\t%d\t%d\t%d\t%d\n",
								current_window,
								x_window_attributes.x,
								x_window_attributes.y,
								x_window_attributes.width,
								x_window_attributes.height,
								x_window_attributes.border_width,
								x_window_attributes.depth,
								x_window_attributes.root,
								x_window_attributes.bit_gravity,
								x_window_attributes.win_gravity,
								x_window_attributes.map_state,
								x_window_attributes.override_redirect);
			 */
			if ((x_window_attributes.map_state == IsViewable) && (x_window_attributes.override_redirect == 0)) {
				printf("Found a window\n");
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
			/* 			if (current_window == 18876012) {
							XMoveWindow(display, current_window, 10, 10);
						} */
		} else {
			printf("Got a status error on XGetWindowAttributes\n");
		}
	}
	g_slist_foreach (visible_windows, move_windows, NULL);

	XCloseDisplay(display);
	/* fclose(outfile);
	fclose(property_file); */
	return 0;
}
