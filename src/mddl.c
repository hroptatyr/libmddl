/** mddl.c - mddl stubs and snippets */
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "mddl.h"

#if defined __INTEL_COMPILER
# pragma warning (disable:177)
#endif	/* __INTEL_COMPILER */
#if !defined UNUSED
# define UNUSED(_x)	__attribute__((unused)) _x
#endif	/* !UNUSED */

#define ADDF(rt)				\
	mddl_##rt##_t res;			\
	size_t idx;				\
						\
	idx = to->n##rt++;			\
	to->##rt = realloc(			\
		to->##rt,			\
		to->n##rt * sizeof(*to->##rt));	\
	res = to->##rt + idx;			\
	/* initialise the result */		\
	memset(res, 0, sizeof(*res));		\
	return res

#include "mddl-3.0-beta-funs.c"

/* mddl.c ends here */
