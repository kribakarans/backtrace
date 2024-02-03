
#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int addr2line(char *trace)
{
	int retval = -1;
	char *cmd = NULL;
	char *exe = trace;
	char *offset = NULL;

	do {
		if (trace == NULL) {
			fprintf(stderr, "Oops! trace is NULL\n");
			retval = -1;
			break;
		}

		offset = strchr(trace, '[') + 1; /* Construct offset  */
		*(strchr(trace, ']')) = '\0';    /* Finish offset end */
		*(strchr(trace, '(')) = '\0';    /* Finish exe end    */

		asprintf(&cmd, "addr2line -sfpe %s %s", exe, offset);
		//printf("%s\n", cmd);
		system(cmd);
		free(cmd);
	} while(0);

	return retval;
}
