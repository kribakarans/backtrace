
#include <backtrace.h>

static void error_callback(void *data, const char *msg, int errnum)
{
	printf("Something went wrong in libbacktrace: %s\n", msg);
}

static int full_callback(void *data , uintptr_t pc, const char *filename, int lineno, const char *function)
{
	if (filename != NULL || function != NULL) {
		fprintf(stdout, "%p  %s:%d  %s()\n", (void*)pc, filename, lineno, function);
	} else {
		fprintf(stdout, "%p  ??:??  ??\n", (void*)pc);
	}

	return 0;
}

static void gcc_print_backtrace(void)
{
	struct backtrace_state *lbstate = NULL;

	lbstate = backtrace_create_state (NULL, 0, error_callback, NULL);

	/* Print formatted backtrace */
	backtrace_full(lbstate, 0, full_callback, error_callback, 0);

	/* Print with inbuilt backtrace api */
	//backtrace_print(lbstate, 0, stdout);
}

int test_fun_5()
{
	printf("Inside %s: %p\n", __func__, test_fun_5);

	printf("\nBacktrace:\n");

	gcc_print_backtrace();

	return 0;
}
