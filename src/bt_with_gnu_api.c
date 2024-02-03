
#define BT_SIZE 100

#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>

int addr2line(char *trace);

int gnu_print_backtrace(void)
{
	char **frames = NULL;
	int i = 0, nframes = 0;
	void *buffer[BT_SIZE] = {0};

	/* Fetch backtrace of the caller */
	nframes = backtrace(buffer, BT_SIZE);
	frames = backtrace_symbols(buffer, nframes);
	if (frames == NULL) {
		printf("Oops! backtrace_symbols failed.\n");
		exit(1);
	}

	for (i = 0; i < nframes; i++) {
		//printf("  %s\n", frames[i]);
		addr2line(frames[i]);
	}

	free(frames);

	return 0;
}

int test_fun_5()
{
	printf("Inside %s: %p\n", __func__, test_fun_5);

	printf("\nBacktrace:\n");

	gnu_print_backtrace();

	return 0;
}
