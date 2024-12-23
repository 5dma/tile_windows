#include <regex.h>

#define LONGEST_STRING 1024
typedef struct  {
	char title[1024];
	unsigned short int current_length;
	unsigned short int current_width;
	struct WindowStruct *next;
} WindowStruct;

regex_t *make_window_regex();
regex_t *make_monitor_regex();