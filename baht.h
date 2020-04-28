#ifndef BAHT_H
#define BAHT_H

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ERRNUM 256

#define BAHT_IS_NULL_ERRNO == NULL ? baht_print_error_message_and_exit(__FILE__,  __LINE__, errno) : 0;
#define BAHT_IS_ERRNUM [errnum_array] = 1; baht_find_errnum(__FILE__, __LINE__);

#endif //BAHT_H

#ifdef BAHT_IMPLEMENTATION
#undef BAHT_IMPLEMENTATION

static __thread char errnum_array[MAX_ERRNUM];

static void baht_print_error_message_and_exit(char* filename, int line, int errnum)
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

static void baht_find_errnum(char* filename, int line)
{
  int errnum = -1;

  //0-th element means success
  for(int i = 1; i < MAX_ERRNUM; i++)
  {
    if(errnum_array[i] == 1)
    {
      errnum = i;
      break;
    }
  }
  if(errnum == -1)
  {
    /*this means that we indexed an array element
    out of its bounds - memory corruption occured*/
    if(errnum_array[0] != 1)
    {
      fprintf(stderr, "<%s>,  line %d: Improper use of a macro: memory corruption detected\n", filename, line);
      exit(EXIT_FAILURE);
    }

    errnum_array[0] = 0;
  }
  else
      baht_print_error_message_and_exit(filename, line, errnum);
}

#endif //BAHT_IMPLEMENTATION
