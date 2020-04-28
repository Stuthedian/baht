#ifndef BAHT_H
#define BAHT_H

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define MAX_ERRNUM 256

#define BAHT_IS_NULL_ERRNO == NULL ? baht_print_error_message_and_exit(__FILE__,  __LINE__, errno) : 0;
#define BAHT_IS_ERRNUM [errnum_array] = 1; baht_find_errnum(__FILE__, __LINE__);

static void baht_handle_sigabort(int signum);
static void baht_handle_sigsegv(int signum);

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
    abort();
	}
	fprintf(stderr, "<%s>,  line %d: %s\n", filename, line, buf);
#else//GNU version
	fprintf(stderr, "<%s>,  line %d: %s\n", filename, line, strerror_r(errnum, buf, 256));
#endif
  abort();
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
      abort();
    }

    errnum_array[0] = 0;
  }
  else
      baht_print_error_message_and_exit(filename, line, errnum);
}

static void baht_catch_sigabort()
{
  struct sigaction catch_signal;
  catch_signal.sa_handler = baht_handle_sigabort;
  sigaction(SIGABRT, &catch_signal, NULL);//check return value
}

static void baht_catch_sigsegv()
{
  struct sigaction catch_signal;
  catch_signal.sa_handler = baht_handle_sigsegv;
  sigaction(SIGSEGV, &catch_signal, NULL);//check return value
}

static void baht_handle_sigabort(int signum)
{
  sigaction(SIGABRT, NULL, NULL);
  puts("Received sigabort signal");
  puts("Waiting for any signal to arrive");
  sigset_t set;
  sigemptyset(&set);
  sigsuspend(&set);
}

static void baht_handle_sigsegv(int signum)
{
  sigaction(SIGSEGV, NULL, NULL);
  puts("Received sigsegv signal");
  puts("Waiting for any signal to arrive");
  sigset_t set;
  sigemptyset(&set);
  sigsuspend(&set);
}

#endif //BAHT_IMPLEMENTATION
