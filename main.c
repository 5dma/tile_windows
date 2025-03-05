#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <X11/Xatom.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PROPERTY_VALUE_LEN 4096
#define MAX_PROPERTY_NAME 4096

int main(int argc, char *argv[]) {
	Display * display = XOpenDisplay(NULL);
	if (display == NULL) {
		printf("Could not connect to the display server. Exiting.\n");
		return -1;
	}
	Window root = DefaultRootWindow(display);
	/*unsigned int size;
	Atom xa_ret_type;
	int ret_format;
	unsigned long ret_nitems;
	unsigned long ret_bytes_after;
	unsigned char *ret_prop;
	
	int number_properties;
	Atom * property_list = XListProperties(display, root, &number_properties);
	char property_name[MAX_PROPERTY_NAME];
	for (int i=0; i<number_properties; i++) {
		strcpy(property_name, XGetAtomName(display, property_list[i]));
		printf("Property %d %ld %s\n", i, property_list[i], property_name);
		
	}*/
/* 
	int property = XGetWindowProperty(display, root,"_NET_CLIENT_LIST", 0, 4096, False,
            XA_WINDOW, &xa_ret_type, &ret_format,     
            &ret_nitems, &ret_bytes_after, &ret_prop); */

	int number_properties;
	char property_name[MAX_PROPERTY_NAME];
	Window root_return;
	Window parent_return;
	Window *children_return;
	Status status;
	unsigned int nchildren_return;
	status = XQueryTree(display, root, &root_return, &parent_return, &children_return, &nchildren_return);
	if (status == 0) {
		printf("Fatal error. XQueryTree failed with a status %d\n", status);
	}
	Window current_window;
	XWindowAttributes x_window_attributes;
	FILE *outfile;
	outfile = fopen("/tmp/stuff.csv","w");
	FILE *property_file = fopen("/tmp/property_file.csv","w");
	fprintf(outfile, "window\tx\ty\twidth\theight\tborder_width\tdepth\troot\tbit_gravity\twin_gravity\tmap_state\tnumber_properties\n");
	fprintf(property_file, "window\tAtom\tName\tactual_type\tactual_format\tnitems_return\tbytes_after_return\tprop_return\n");

	for (unsigned int i = 0; i<nchildren_return; i++) {
		current_window = children_return[i];
		Atom *property_list = XListProperties(display, children_return[i], &number_properties);
		status = XGetWindowAttributes(display, current_window, &x_window_attributes);
		if (status != 0) {
		fprintf(outfile, "%ld\t%d\t%d\t%d\t%d\t%d\t%d\t%ld\t%d\t%d\t%d\t%d\n",
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
				number_properties);
				
				Atom * property_list = XListProperties(display, current_window, &number_properties);
				Atom actual_type_return;
				int actual_format_return;
				unsigned long nitems_return;
				unsigned long bytes_after_return;
				unsigned char *prop_return;
				int get_property;
				for (int i=0; i<number_properties; i++) {
					strcpy(property_name, XGetAtomName(display, property_list[i]));

					get_property = XGetWindowProperty( display, current_window, property_list[i], 0, 32, False, AnyPropertyType, &actual_type_return, &actual_format_return, &nitems_return, &bytes_after_return, &prop_return);
					if (get_property == Success) {
						//printf("%s\n", prop_return);
						fprintf(property_file, "%ld\t%ld\t%s\t%ld\t%d\t%ld\t%ld\t%s\n",
							current_window, 
							property_list[i], 
							property_name,
							actual_type_return,
							actual_format_return,
							nitems_return,
							bytes_after_return,
							prop_return);

					} else {
						printf("Failed on getting a property\n");
					}
				}
		} else {
			printf("Got a status error on XGetWindowAttributes\n");
		}

	}
	XCloseDisplay(display);
	fclose(outfile);
	fclose(property_file);
	return 0;
}
