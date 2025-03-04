#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <X11/Xatom.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PROPERTY_VALUE_LEN 4096

int main(int argc, char *argv[]) {
	Display * display = XOpenDisplay(NULL);
	if (display == NULL) {
		printf("Could not connect to the display server. Exiting.\n");
		return -1;
	}
	Window root = DefaultRootWindow(display);
	unsigned int size;
	Atom xa_ret_type;
	int ret_format;
	unsigned long ret_nitems;
	unsigned long ret_bytes_after;
	unsigned char *ret_prop;
	
	int number_properties;
	Atom * property_list = XListProperties(display, root, &number_properties);
	for (int i=0; i<number_properties; i++) {
		printf("Property %d %ld\n", i, property_list[i]);
		
	}
/* 
	int property = XGetWindowProperty(display, root,"_NET_CLIENT_LIST", 0, 4096, False,
            XA_WINDOW, &xa_ret_type, &ret_format,     
            &ret_nitems, &ret_bytes_after, &ret_prop); */

	Window root_return;
	Window parent_return;
	Window *children_return;
	unsigned int nchildren_return;
	Status status = XQueryTree(display, root, &root_return, &parent_return, &children_return, &nchildren_return);
	if (status == 0) {
		printf("Fatal error. XQueryTree failed with a status %d\n", status);
	}
	XCloseDisplay(display);
	return 0;
}
