/* config.h.cmake */
/* ELF size: 32 or 64 */
#define BACKTRACE_ELF_SIZE 64
/* XCOFF size: 32 or 64 */
/* #undef BACKTRACE_XCOFF_SIZE */
/* Define to 1 if you have the __atomic functions */
#define HAVE_ATOMIC_FUNCTIONS 1
/* Define to 1 if you have the `clock_gettime' function. */
#define HAVE_CLOCK_GETTIME 1
/* Define to 1 if you have the declaration of `strnlen', and to 0 if you
   don't. */
#define HAVE_DECL_STRNLEN 1
/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1
/* Define if dl_iterate_phdr is available. */
#define HAVE_DL_ITERATE_PHDR 1
/* Define to 1 if you have the fcntl function */
#define HAVE_FCNTL 1
/* Define if getexecname is available. */
/* #undef HAVE_GETEXECNAME */
/* Define if _Unwind_GetIPInfo is available. */
#define HAVE_GETIPINFO 1
/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1
/* Define to 1 if you have the `z' library (-lz). */
#define HAVE_LIBZ 1
/* Define to 1 if you have the <link.h> header file. */
#define HAVE_LINK_H 1
/* Define if AIX loadquery is available. */
/* #undef HAVE_LOADQUERY */
/* Define to 1 if you have the `lstat' function. */
/* #undef HAVE_LSTAT */
/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1
/* Define to 1 if you have the `readlink' function. */
#define HAVE_READLINK 1
/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1
/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1
/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1
/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1
/* Define to 1 if you have the __sync functions */
#define HAVE_SYNC_FUNCTIONS 1
/* Define to 1 if you have the <sys/ldr.h> header file. */
/* #undef HAVE_SYS_LDR_H */
/* Define to 1 if you have the <sys/mman.h> header file. */
#define HAVE_SYS_MMAN_H 1
/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1
/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1
/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1
/* Define if -lz is available. */
#define HAVE_ZLIB 1

/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
/* #undef _ALL_SOURCE */
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
/* Enable threading extensions on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
/* #undef _POSIX_PTHREAD_SEMANTICS */
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
/* #undef _TANDEM_SOURCE */
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
/* #undef __EXTENSIONS__ */
#endif
