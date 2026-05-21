#ifndef DSA_H
#define DSA_H

#include <assert.h>

#define Pair(t1, t2) struct { t1 first; t2 second; }

#ifndef DA_REALLOC
#include <glut.h>
#define DA_REALLOC glut_realloc
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

#define da_foreach(type, item, da) \
	for (type *item = (da)->items; \
	     item < (da)->items + (da)->count; item++)

#endif /* DSA_H */
