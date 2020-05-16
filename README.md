baht([bäxt] - 'luck' in [talysh](https://en.wikipedia.org/wiki/Talysh_language) language) is single-file library to ease 
development process by providing a set of error-checking macros. Also, baht includes a signal handler, to suspend program 
execution on signal arrival.

## Usage
Before using any of library's macro call baht_init() to initialize library's internal data.
Make sure that baht_init() called in single-threaded context!
Then, in one of your files that includes the header define `BAHT_IMPLEMENTATION` like so:
```c
#define BAHT_IMPLEMENTATION
#include "baht.h"
```

### Example
Using these macros is very simple. Write them after function call. You can omit semicolon if you want
```c
foo() BAHT_IS_NULL_ERRNO;
```

## Available macros
| Macro        | Triggered if return value equals to|  Uses `errno`  | Functions to check  |
| ------------- |:-------------:| :-----:| ----|
| `BAHT_IS_NEG_1_ERRNO`      | -1 | yes | most c standard library functions  |
| `BAHT_IS_ERRNUM`       | > 0      |   no | pthread_* |
| `BAHT_IS_EOF_ERRNO` |   EOF    |    yes | putc, puts, fclose |
| `BAHT_IS_NULL_ERRNO` | NULL      |    yes | malloc and friends|
| `BAHT_IS_NULL` | NULL    |    no | ncurses derwin, subwin |
| `BAHT_IS_ERR` | ERR    |    no | most ncurses functions |
             
To brought macro `BAHT_IS_ERR` define `BAHT_NCURSES`

All macros in case of error will print diagnostic message and call `abort()` which could be caught by `baht_catch_signal(SIGABRT)`

## Signal handlers
How many times you got a segfault and then spent time trying to reproduce problem in the debugger?
No more of this hassle!
* Call baht_catch_signal to set a signal handler. Most of the time you would want to catch SIGSEGV and SIGABRT:
```c
baht_init();
baht_catch_signal(SIGABRT);
baht_catch_signal(SIGSEGV);
```
* When signal is caught you would see message "Received signal..."
* Now it is time to launct gdb, just do ``sudo gdb -p `pgrep <process_name>` ``
* By default you will be in depth of system calls. Type `bt` to show backtrace
* Then type `frame <frame_number>` to select interesting stack frame
* Now you are ready to investigate an issue. Good luck and happy hacking!

### SIGINT == reset
Side-effect for ncurses users — sending SIGINT (or SIGTERM) to the program when it is in signal pending
state causes terminal screen to be cleared — much like calling `reset` in shell
