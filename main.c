#include <headers.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_MATCHES 5

int main(int argc, char *argv[]) {
	printf("hello\n");
	FILE *fp;
	char output[1024];
	regex_t *window_regex = make_window_regex();
	if (window_regex == NULL) {
		return (-1);
	}

	fp = popen("wmctrl -l", "r");
	if (fp == NULL) {
		perror("Error opening pipe");
		return -1;
	}

	int regexec_result;
	regmatch_t window_matches[NUMBER_MATCHES];
	char window_title[4096];
	unsigned short int number_windows = 0; 

	while (fgets(output, sizeof(output), fp) != NULL) {
		// printf("%s", output);
		regexec_result = regexec(window_regex, output, NUMBER_MATCHES, window_matches, 0);
		if (regexec_result != 0) {
			printf("Failed on a window_regex match, skipping...\n");
			continue;
		}

		int length = window_matches[4].rm_eo - window_matches[4].rm_so - 1;
		strncpy(window_title, output + window_matches[4].rm_so, length);
		window_title[length] = '\0';
		printf("Window title: %s\n", window_title);
		number_windows++; 
	}
	pclose(fp);
	regfree(window_regex);

	printf("The number of open windows is %d\n", number_windows);

	regex_t *monitor_regex = make_monitor_regex();
	if (monitor_regex == NULL) {
		return (-1);
	}

	fp = popen("xrandr", "r");
	if (fp == NULL) {
		perror("Error opening pipe");
		return -1;
	}
	regmatch_t monitor_matches[NUMBER_MATCHES];
	char monitor_length[4096];
	char monitor_width[4096];

	int keep_going = 1;
	while (keep_going) {
		fgets(output, sizeof(output), fp);
		// printf("%s", output);
		regexec_result = regexec(monitor_regex, output, NUMBER_MATCHES, monitor_matches, 0);
		if (regexec_result != 0) {
			continue;
		}
		keep_going = 0;
	}

	pclose(fp);
	regfree(window_regex);

	int length = monitor_matches[1].rm_eo - monitor_matches[1].rm_so;
	strncpy(monitor_length, output + monitor_matches[1].rm_so, length);
	monitor_length[length] = '\0';

	int width = monitor_matches[2].rm_eo - monitor_matches[2].rm_so;
	strncpy(monitor_width, output + monitor_matches[2].rm_so, width);
	monitor_width[width] = '\0';

	printf("Monitor dimensions: %s x %s\n", monitor_length, monitor_width);
	int real_length = atoi(monitor_length);
	int real_width = atoi(monitor_width);


	return 0;
}