# Print backtrace/stacktrace programmatically:

In this article, we'll explore how to develop self-debuggable apps in Linux using the unwind and backtrace mechanisms.

The sample C programs in this demo are built with `-g` and `-no-pie` options to export debug symbols and disable dynamically linked position independent executable (address randomisation).

Explore the repository [here](https://github.com/kribakarans/backtrace).

## 1. Backtrace with GCC library (`libbacktrace`)
### API Reference:
```c
int full_callback(void *data , uintptr_t pc, const char *filename, int lineno, const char *function) {
	if (filename != NULL || function != NULL) {
		fprintf(stdout, "%p  %s:%d  %s()\n", (void*)pc, filename, lineno, function);
	} else {
		fprintf(stdout, "%p  ??:??  ??\n", (void*)pc);
	}

	return 0;
}

void gcc_print_backtrace(void) {
	struct backtrace_state *lbstate = NULL;

	lbstate = backtrace_create_state (NULL, 0, error_callback, NULL);

	/* Print formatted backtrace */
	backtrace_full(lbstate, 0, full_callback, error_callback, 0);

	return;
}
```
**Compilation:**

	gcc -g -no-pie src/bt_with_gcc_api.c src/user.c -lbacktrace

**Execution:**

	$ ./a.out
	  Backtrace:
	  0x401564  src/bt_with_gcc_api.c:27  gcc_print_backtrace()
	  0x40159f  src/bt_with_gcc_api.c:39  test_fun_5()
	  0x4015d7  src/user.c:9   test_fun_4()
	  0x40160f  src/user.c:18  test_fun_3()
	  0x401647  src/user.c:27  test_fun_2()
	  0x40167f  src/user.c:36  test_fun_1()
	  0x4016b7  src/user.c:45  main()

Verify the source file and line with `addr2line` command:

	$ addr2line -sfpe ./a.out 0x401564
	  gcc_print_backtrace at bt_with_gcc_api.c:27

## 2. Backtrace with Unwind library (`libunwind`)
### API Reference:
```c
void unw_print_backtrace(void) {
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
		if (unw_get_proc_name(&cursor, symbol, sizeof(symbol), &offset) == 0) {
			printf("  %s+0x%lx [0x%lx]\n", symbol, offset, (long)pc);
			asprintf(&trace, "%s(0x%lx) [0x%lx]", UWEXE, offset, (long)pc); /* FIXME: Get exe name at runtime with dli_fname */
			addr2line(trace);
			free(trace);
		} else {
			printf("  0x%lx: ???\n", (long)pc);
		}
	}

	return;
}
```
**Compilation:**

	gcc -g -no-pie src/bt_with_unwind_api.c src/addr2line.c src/user.c -lunwind -lunwind-generic

**Execution:**

	$ ./a.out
	  Backtrace:
	  test_fun_5+0x38 [0x4014ca]
	  test_fun_4+0x31 [0x401602]
	  test_fun_3+0x31 [0x40163a]
	  test_fun_2+0x31 [0x401672]
	  test_fun_1+0x31 [0x4016aa]
	  main+0x31 [0x4016e2]

**Verification:**

	$ addr2line -sfpe ./a.out 0x401602
	  test_fun_4 at user.c:11

**Address mapping with `addr2line` API:**

	Backtrace:
	test_fun_5 at bt_with_unwind_api.c:84
	test_fun_4 at user.c:11
	test_fun_3 at user.c:20
	test_fun_2 at user.c:29
	test_fun_1 at user.c:38
	main at user.c:47

## 3. Backtrace with C library
### API Reference:
```c
int gnu_print_backtrace(void) {
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
		printf("  %s\n", frames[i]);
		addr2line(frames[i]);
	}

	free(frames);

	return 0;
}
```
**1. Compilation: (with `-rdynamic` option)**

The `-rdynamic` option instructs the linker to add all symbols.

	gcc -g -no-pie src/bt_with_gnu_api.c src/addr2line.c src/user.c -rdynamic

**Execution:**

	$ ./a.out
	  Backtrace:
	  ./a.out(gnu_print_backtrace+0x68) [0x4012fe]
	  ./a.out(test_fun_5+0x38) [0x4013f6]
	  ./a.out(test_fun_4+0x31) [0x40159f]
	  ./a.out(test_fun_3+0x31) [0x4015d7]
	  ./a.out(test_fun_2+0x31) [0x40160f]
	  ./a.out(test_fun_1+0x31) [0x401647]
	  ./a.out(main+0x31) [0x40167f]

**Verification:**

	$ addr2line -sfpe ./a.out 0x4012fe
	  gnu_print_backtrace at bt_with_gnu_api.c:17

**Address mapping with `addr2line` API:**

	Backtrace:
	gnu_print_backtrace at bt_with_gnu_api.c:17
	test_fun_5 at bt_with_gnu_api.c:42
	test_fun_4 at user.c:11
	test_fun_3 at user.c:20
	test_fun_2 at user.c:29
	test_fun_1 at user.c:38
	main at user.c:47

**2. Compilation: (without `-rdynamic` option)**

	gcc -g -no-pie src/bt_with_gnu_api.c src/addr2line.c src/user.c

**Execution:**

	$ ./a.out
	  Backtrace:
	  ./a.out() [0x4012fe]
	  ./a.out() [0x4013f6]
	  ./a.out() [0x40159f]
	  ./a.out() [0x4015d7]
	  ./a.out() [0x40160f]
	  ./a.out() [0x401647]
	  ./a.out() [0x40167f]

**Verification:**

	$ addr2line -sfpe ./a.out 0x4012fe
	  gnu_print_backtrace at bt_with_gnu_api.c:17
