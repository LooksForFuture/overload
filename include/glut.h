#ifndef GLUT_H
#define GLUT_H

#include <stddef.h>

#define glut_malloc(size) glut_malloc_impl(size, __FILE__,\
					   __LINE__, __func__)

#define glut_realloc(ptr, size) glut_realloc_impl(ptr, size, __FILE__, \
						  __LINE__, __func__)

#define glut_free(ptr) glut_free_impl(ptr, __FILE__, __LINE__, __func__)

void glut_init(void);

void glut_shutdown(void);

void *glut_malloc_impl(size_t, const char *, int, const char *);

void *glut_realloc_impl(void *, size_t, const char *, int, const char *);

void glut_free_impl(void *, const char *, int, const char *);

#endif //GLUT_H
