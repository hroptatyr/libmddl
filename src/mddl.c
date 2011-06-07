/** mddl.c - mddl stubs and snippets */
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "mddl.h"

#if defined __INTEL_COMPILER
# pragma warning (disable:177)
# pragma warning (disable:869)
#endif	/* __INTEL_COMPILER */
#if !defined UNUSED
# define UNUSED(_x)	__attribute__((unused)) _x
#endif	/* !UNUSED */

static bool
__wsp(char c)
{
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

static char*
strndup_sans_ws(const char *buf, size_t bsz)
{
/* like strndup() but skip leading whitespace */
	while (__wsp(*buf)) {
		buf++;
		bsz--;
	}
	while (bsz > 0 && __wsp(buf[bsz - 1])) {
		bsz--;
	}
	return bsz ? strndup(buf, bsz) : NULL;
}

#define ADDF(rt)				\
	mddl_##rt##_t res;			\
	size_t idx;				\
						\
	idx = to->n##rt++;			\
	to->rt = realloc(			\
		to->rt,				\
		to->n##rt * sizeof(*to->rt));	\
	res = to->rt + idx;			\
	/* initialise the result */		\
	memset(res, 0, sizeof(*res));		\
	return res

#define REMF(rt)						\
	size_t idx = (what - from->rt) / sizeof(*what);		\
								\
	if (idx >= from->n##rt) {				\
		return 0;					\
	}							\
	memmove(from->rt + idx, from->rt + idx + 1,		\
		(from->n##rt - (idx + 1)) * sizeof(*what));	\
	from->n##rt--;						\
	from->rt = realloc(					\
		from->rt,					\
		from->n##rt * sizeof(*from->rt));		\
	return 1

/* for strings */
#define SET_S_F(slot)				\
	size_t len = strlen(from);		\
						\
	if (to->slot) {			\
		free(to->slot);		\
	}					\
	to->slot =				\
		strndup_sans_ws(from, len);	\
	return

/* for integers */
#define SET_I_F(slot)				\
	to->slot = from;			\
	return

/* for doubles */
#define SET_D_F(slot)				\
	to->slot = from;			\
	return

/* for time_t's */
#define SET_t_F(slot)				\
	to->slot = from;			\
	return

/* for structs */
#define SET_struct_F(slot)			\
	*to->slot = *from;			\
	return

#define SET_mddl_mdString_t_F		SET_S_F
#define SET_xsd_string_t_F		SET_S_F
#define SET_xsd_ID_t_F			SET_S_F
#define SET_mddl_mdUri_t_F		SET_S_F
#define SET_xsd_anyURI_t_F		SET_S_F

#define SET_mddl_QualityEnumeration_t_F	SET_I_F
#define SET_mddl_mdInteger_t_F		SET_I_F
#define SET_xsd_integer_t_F		SET_I_F
#define SET_xsd_boolean_t_F		SET_I_F

#define SET_mddl_AnyDateTime_t_F	SET_t_F
#define SET_xsd_duration_t_F		SET_t_F

#define SET_xsd_decimal_t_F		SET_D_F
#define SET_mddl_NonNegativeDecimal_t_F	SET_D_F

#include "mddl-3.0-beta-funs.c"

/* more stuff that would normally go to a header but for pipeline reasons
 * is best kept here */
DEFUN bool
__header_null_p(mddl_header_t h)
{
	return __dateTime_null_p(h->dateTime) &&
		__source_null_p(h->source) &&
		__query_null_p(h->query) &&
		h->nschemeInfo == 0;
}

/* mddl.c ends here */
