#ifndef  _STDIOEXT_H
#define  _STDIOEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

typedef struct cookie_io_functions_t {
  ssize_t (*read)(void *cookie, char *buf, size_t n);
  ssize_t (*write)(void *cookie, const char *buf, size_t n);
  int (*seek)(void *cookie, off_t *pos, int whence);
  int (*close)(void *cookie);
} cookie_io_functions_t;

FILE *fopencookie(void *cookie, const char *mode, cookie_io_functions_t functions);

FILE *fmemopen(void *buf, size_t size, const char *mode);

FILE *open_memstream(char **buf, size_t *size);

#ifdef __cplusplus
} // extern "C"
#endif

#endif  /* _STDIOEXT_H */
