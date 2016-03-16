#ifndef __dbg_h__
#define __dbg_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef NDEBUG
    #define debug(M, ...)
    #define debug_check(A, M, ...)
#else
    #define debug(M, ...) fprintf(stderr, "Profinet Plugin [DEBUG] %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
    #define debug_check(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; }
#endif

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...) fprintf(stderr, ANSI_COLOR_RED "Profinet Plugin [ERROR] (%s:%d: errno: %s) " M ANSI_COLOR_RESET "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(stderr, ANSI_COLOR_YELLOW "Profinet Plugin [WARN] (%s:%d: errno: %s) " M ANSI_COLOR_RESET "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...) fprintf(stderr, "Profinet Plugin [INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define goto_error(L) goto L

#define check_to(A, L, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto_error(L); }

#define check(A, M, ...) check_to((A), error, M, ##__VA_ARGS__)

#define sentinel(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define check_mem(A) check((A), "Out of memory.")

#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; }

#endif
