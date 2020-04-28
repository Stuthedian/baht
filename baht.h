#ifndef BAHT_H
#define BAHT_H

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define BAHT_IS_NULL_ERRNO == NULL ? print_error_message_and_exit(__FILE__,  __LINE__, errno) : 0;

#endif //BAHT_H

#ifdef BAHT_IMPLEMENTATION
#undef BAHT_IMPLEMENTATION

static void print_error_message_and_exit(char* filename, int line, int errnum)
{
	char buf[256];

#if (_POSIX_C_SOURCE >= 200112L) && !  _GNU_SOURCE //XSI version
	int status = strerror_r(errnum, buf, 256);
	if(status != 0)
	{
		if(status > 0)
			fprintf(stderr, "strerror_r failed. Error number is: %d\n", status);
		else if(status == -1)
			fprintf(stderr, "strerror_r failed. Error number is: %d\n", errno);
		else
			fputs("strerror_r failed. Unknown failure", stderr);
    fprintf(stderr, "<%s>,  line %d\n", filename, line);
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "<%s>,  line %d: %s\n", filename, line, buf);
#else//GNU version
	fprintf(stderr, "<%s>,  line %d: %s\n", filename, line, strerror_r(errnum, buf, 256));
#endif
	exit(EXIT_FAILURE);
}

#endif //BAHT_IMPLEMENTATION
