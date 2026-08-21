#ifndef DSA_H
#define DSA_H

#include <assert.h>
#include <string.h>

#define Pair(t1, t2) struct { t1 first; t2 second; }

#ifndef DA_REALLOC
#include <glut.h>
#define DA_REALLOC glut_realloc
#endif

#ifndef DA_FREE
#define DA_FREE glut_free
#endif

#ifndef DA_INIT_CAP
#define DA_INIT_CAP 64
#endif

#define da_append(da, item) \
	do {						\
		if ((da)->count >= (da)->capacity) {	\
			(da)->capacity = (da)->capacity == 0 ?		\
				DA_INIT_CAP : (da)->capacity*2;		\
			(da)->items = DA_REALLOC((da)->items,		\
						 (da)->capacity*	\
						 sizeof(*(da)->items));	\
			assert((da)->items!=NULL&&"Failed to expand"); \
		}							\
		(da)->items[(da)->count++] = (item);		\
	} while (0)

#define da_append_many(da, newItems, newItemsCount) \
	do {								\
		if ((da)->count + (newItemsCount) > (da)->capacity) {	\
			if ((da)->capacity == 0) {			\
				(da)->capacity = DA_INIT_CAP;		\
			}						\
			while ((da)->count + (newItemsCount) > (da)->capacity) { \
				(da)->capacity *= 2;		\
			}						\
			(da)->items = DA_REALLOC((da)->items, (da)->capacity*sizeof(*(da)->items)); \
			assert((da)->items != NULL && "Failed to expand"); \
		}							\
		memcpy((da)->items + (da)->count, (newItems), (newItemsCount)*sizeof(*(da)->items)); \
		(da)->count += (newItemsCount);			\
	} while (0)

#define fa_append(fa, item) \
	do { \
		assert((fa)->count < (fa)->capacity && \
			"Fixed array is full"); \
		(fa)->items[(fa)->count++] = (item); \
	} while (0)

#define fa_append_many(fa, newItems, newItemsCount) \
	do { \
		assert ((fa)->count+(newItemsCount) < (fa)->capacity && \
			"Fixed array is full"); \
		memcpy((fa)->items + (fa)->count, (newItems), \
		       (newItemsCount)*sizeof(*(fa)->items)); \
		(fa)->count += (newItemsCount); \
	} while (0)

#define arr_foreach(type, item, arr) \
	for (type *item = (arr)->items; \
	     item < (arr)->items + (arr)->count; item++)

#endif /* DSA_H */
