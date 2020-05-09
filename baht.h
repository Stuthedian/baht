#ifndef BAHT_H
#define BAHT_H

#if !defined _POSIX_C_SOURCE || _POSIX_C_SOURCE < 2
#define _POSIX_C_SOURCE 2
#endif

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BAHT_MAX_ERRNUM 256
#define BAHT_MAX_DESC_LEN 256
#define BAHT_MAX_SIGNAL_NAME_LEN 32
#define BAHT_COMMAND_BUFFER_SIZE 32

#define BAHT_IS_NEG_1_ERRNO == -1 ? baht_print_error_message(__FILE__,  __LINE__, errno) : 0;
#define BAHT_IS_NULL_ERRNO == NULL ? baht_print_error_message(__FILE__,  __LINE__, errno) : 0;
#define BAHT_IS_EOF_ERRNO == EOF ? baht_print_error_message(__FILE__,  __LINE__, errno) : 0;
#define BAHT_IS_NULL == NULL ? baht_print_custom_error_message(__FILE__,  __LINE__, "NULL pointer") : 0;
#define BAHT_IS_ERRNUM [errnum_array] = 1; baht_find_errnum(__FILE__, __LINE__);

#ifdef BAHT_NCURSES
#define BAHT_IS_ERR == ERR ? baht_print_custom_error_message(__FILE__,  __LINE__, "Ncurses error") : 0;
#endif //BAHT_NCURSES

extern __thread char errnum_array[BAHT_MAX_ERRNUM];

void baht_init();
void baht_print_error_message(char* filename, int line, int errnum);
void baht_print_custom_error_message(char* filename, int line, char* error_message);
void baht_find_errnum(char* filename, int line);
void baht_catch_signal(int signum);
void baht_handle_signal(int signum);

#endif //BAHT_H

#ifdef BAHT_IMPLEMENTATION
#undef BAHT_IMPLEMENTATION

char errnum_desc_array[BAHT_MAX_ERRNUM][BAHT_MAX_DESC_LEN];
__thread char errnum_array[BAHT_MAX_ERRNUM];

void baht_init()
{
  char* error_desc;

  memset(errnum_array, 0, BAHT_MAX_ERRNUM);
  memset(errnum_desc_array, 0, BAHT_MAX_ERRNUM * BAHT_MAX_DESC_LEN);

  for(int i = 0; i < BAHT_MAX_ERRNUM; i++)
  {
    error_desc = strerror(i);
    strncpy(errnum_desc_array[i], error_desc, BAHT_MAX_DESC_LEN);
    errnum_desc_array[i][BAHT_MAX_DESC_LEN - 1] = '\0';
  }
}

void baht_print_error_message(char* filename, int line, int errnum)
{
  if(errnum >= 0 && errnum < BAHT_MAX_ERRNUM)
    fprintf(stderr, "<%s>,  line %d: %s\n", filename, line,
        errnum_desc_array[errnum]);
  else
    fprintf(stderr, "<%s>,  line %d: Unknown error %d\n", filename, line,
        errnum);

  abort();
}

void baht_print_custom_error_message(char* filename, int line, char* error_message)
{
  fprintf(stderr, "<%s>,  line %d: %s\n", filename, line, error_message);
  abort();
}

void baht_find_errnum(char* filename, int line)
{
  int errnum = -1;

  //0-th element means success
  for(int i = 1; i < BAHT_MAX_ERRNUM; i++)
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
      baht_print_error_message(filename, line, errnum);
}

void baht_catch_signal(int signum)
{
  struct sigaction catch_signal = {0};
  catch_signal.sa_handler = baht_handle_signal;
  sigaction(signum, &catch_signal, NULL) BAHT_IS_NEG_1_ERRNO;
}

void baht_handle_signal(int signum)
{
  sigaction(signum, NULL, NULL);

  int is_get_signal_name_succedeed = 0;
  char cmd[BAHT_COMMAND_BUFFER_SIZE] = {0};
  char signal_name[BAHT_MAX_SIGNAL_NAME_LEN] = {0};

  snprintf(cmd, BAHT_COMMAND_BUFFER_SIZE, "kill -l %d", signum);

  FILE* pipe = popen(cmd, "r");

  if(pipe == NULL)
    goto EXIT;

  if(fgets(signal_name, BAHT_MAX_SIGNAL_NAME_LEN, pipe) == NULL)
    goto EXIT;

  if(pclose(pipe) == -1)
    goto EXIT;

  is_get_signal_name_succedeed = 1;
  for(int i = 0; i < BAHT_MAX_SIGNAL_NAME_LEN; i++)
  {
    if(signal_name[i] == '\n')
    {
      signal_name[i] = '\0';
      break;
    }
  }

EXIT:
  if(is_get_signal_name_succedeed)
    fprintf(stderr, "Received signal: SIG%s. %s\n", signal_name,
        "Waiting for any signal to arrive");
  else
    fprintf(stderr, "Received signal. Signal number is: %d. %s\n",
        signum, "Waiting for any signal to arrive");

  sigset_t set;
  sigemptyset(&set);
  sigsuspend(&set);
}

#endif //BAHT_IMPLEMENTATION
