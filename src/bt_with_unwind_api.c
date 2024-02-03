
/*
 Compilation:
 cc -g -no-pie src/bt_with_unwind_api.c src/user.c -lunwind -lunwind-generic
 (or)
 cc -g -no-pie -DUSE_DLADDR src/bt_with_unwind_api.c src/user.c -lunwind -lunwind-generic -ldl
*/

#define _GNU_SOURCE

#ifdef USE_DLADDR
#define _GNU_SOURCE
#include <dlfcn.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libunwind.h>

#define UWEXE "./uwbt.out"

int addr2line(char *trace);

void unw_print_backtrace(void)
{
	#ifdef USE_DLADDR
	Dl_info info = {0};
	#endif

	int rc = -1;
	char *trace = NULL;
	unw_cursor_t cursor;
	unw_context_t context;
	unw_word_t offset, pc;
	char symbol[512] = {0};

	/* Step 1: Initialize the unwind context */
	unw_getcontext(&context);
	unw_init_local(&cursor, &context);

	/* Step 2: Iterate through the call stack frames */
	while (unw_step(&cursor) > 0) {
		memset(symbol, 0x00, sizeof(symbol));

		/* Step 3: Get register contents */
		unw_get_reg(&cursor, UNW_REG_IP, &pc);

		/* Step 4: Get name of current procedure */
		#ifdef USE_DLADDR
		rc = dladdr((void*)pc ,&info);
		if (rc == 0 ) {
			printf("  0x%lx: ???\n", (long)pc);
		} else {
			//printf(" ==> 0x%-13lx: %25s (in %s)\n", pc, info.dli_sname, info.dli_fname);
			asprintf(&trace, "%s(0x0000) [0x%lx]", info.dli_fname, pc);
			addr2line(trace);
			free(trace);
		}
		#else
		if (unw_get_proc_name(&cursor, symbol, sizeof(symbol), &offset) == 0) {
			//printf("  %s+0x%lx [0x%lx]\n", symbol, offset, (long)pc);
			asprintf(&trace, "%s(0x%lx) [0x%lx]", UWEXE, offset, (long)pc); /* FIXME: Get exe name at runtime with info.dli_fname */
			addr2line(trace);
			free(trace);
		} else {
			printf("  0x%lx: ???\n", (long)pc);
		}
		#endif
	}

	return;
}


int test_fun_5()
{
	printf("Inside %s: %p\n", __func__, test_fun_5);

	printf("\nBacktrace:\n");

	unw_print_backtrace();

	return 0;
}
