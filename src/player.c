#include "player.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

void stdout_endl() {
	printf("\n");
	fflush(stdout);
}

double rand_uniform() { return (double) rand() / (double) RAND_MAX; }

int rand_int(int l, int r) { return rand() % (r - l) + l; }

void exit_on(int exitCode, const char * why) {
	perror(why);
	exit(exitCode);
}

bool exit_if(bool condition, int exitCode, const char * why) {
	if (condition) { exit_on(exitCode, why); }
	return condition;
}

void todo_impl(const char * why) { exit_on(EXIT_FAILURE, why); }

char * get_current_time_str() {
	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	char * s = asctime(timeinfo);
	s[strlen(s) - 1] = '\0';
	return s;
}

size_t min_size(size_t a, size_t b) {
	if (a > b) {
		return b;
	} else {
		return a;
	}
}