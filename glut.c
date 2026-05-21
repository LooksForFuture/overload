#include <glut.h>

#include <stdio.h>
#include <stdlib.h>

void glut_init(void)
{}

void glut_shutdown(void)
{}

void *glut_malloc_impl(size_t size, const char *file,
		       int line, const char *func)
{
	printf("allocation from file \"%s\", line \"%d\", func \"%s\"\n",
	       file, line, func);
	return malloc(size);
}

void *glut_realloc_impl(void *ptr, size_t size, const char *file,
			int line, const char *func)
{
	printf("reallocation from file \"%s\", line \"%d\", "
	       "func \"%s\"\n",
	       file, line, func);
	return realloc(ptr, size);
}

void glut_free_impl(void *ptr, const char *file,
		    int line, const char *func)
{
	printf("free from file \"%s\", line \"%d\", func \"%s\"\n",
	       file, line, func);
	free(ptr);
}
