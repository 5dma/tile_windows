#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_MATCHES 5

regex_t *make_window_regex() {
	regex_t *regex = (regex_t *)malloc(sizeof(regex_t));
	int value = regcomp(regex, "^(\\S*) +(\\S*) +(\\S*) +(.*)$", REG_EXTENDED);
	if (value != 0) {
		printf("Compilation error of regex\n");
		return NULL;
	}
	return regex;
}


regex_t *make_monitor_regex() {
	regex_t *regex = (regex_t *)malloc(sizeof(regex_t));
	int value = regcomp(regex, "^^ +(\\S*)x(\\S*).*\\*.*$", REG_EXTENDED);
	if (value != 0) {
		printf("Compilation error of regex\n");
		return NULL;
	}
	return regex;
}