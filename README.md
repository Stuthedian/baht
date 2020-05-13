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

