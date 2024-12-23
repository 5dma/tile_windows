#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_MATCHES 5

regex_t *make_regex() {
	regex_t *regex = (regex_t *)malloc(sizeof(regex_t));
	int value = regcomp(regex, "^(\\S*) +(\\S*) +(\\S*) +(.*)$", REG_EXTENDED);
	if (value != 0) {
		printf("Compilation error of regex\n");
		return NULL;
	}
	return regex;
}

int main(int argc, char *argv[]) {
	printf("hello\n");
	FILE *fp;
	char output[1024];
	regex_t *regex = make_regex();
	if (regex == NULL) {
		return(-1);
	}

	fp = popen("wmctrl -l", "r");
	if (fp == NULL) {
		perror("Error opening pipe");
		return -1;
	}

	int regexec_result;
	regmatch_t matches[NUMBER_MATCHES];
	char window_title[4096];

	while (fgets(output, sizeof(output), fp) != NULL) {
		//printf("%s", output);
		regexec_result = regexec (regex, output, NUMBER_MATCHES, matches, 0);
		if (regexec_result != 0) {
			printf("Failed on a regex match, skipping...\n");
			continue;
		}

		int length = matches[4].rm_eo - matches[4].rm_so - 1;
		strncpy(window_title,output+matches[4].rm_so, length);
		window_title[length] = '\0';
		printf("Window title: %s\n", window_title);
		}
	pclose(fp);
	regfree(regex);
	return 0;
}