/** mddl.h - mddl stubs and snippets */

#if !defined INCLUDED_mddl_h_
#define INCLUDED_mddl_h_

#undef DECLF
#undef DEFUN
#if defined STATIC_GUTS
# define DEFUN	static
# define DECLF	static
#else  /* !STATIC_GUTS */
# define DEFUN
# define DECLF	extern
#endif	/* STATIC_GUTS */

typedef struct __mddl_s *mddl_doc_t;

struct __mdString_s {
	char *value;
};

struct __string_s {
	char *value;
};

struct __mdDecimal_s {
	double value;
};

struct __mdNonNegativeDecimal_s {
	double value;
};

struct __mdUri_s {
	char *value;
};

struct __anyURI_s {
	char *value;
};

struct __ID_s {
	char *value;
};

struct __mdInteger_s {
	long int value;
};

struct __integer_s {
	long int value;
};

struct __mdDateTime_s {
	time_t value;
};

struct __mdDuration_s {
	time_t value;
};

struct __mdBoolean_s {
	int value;
};

struct __QualityEnumeration_s {
	enum {
		QE_UNK,
		QE_VALID,
		QE_UNAVAILABLE,
	} value;
};

/* autogenerated files */
#include "mddl-3.0-beta-spec.h"
#include "mddl-3.0-beta-funs.h"

/* print service */
DECLF void mddl_print(void *out, mddl_doc_t doc);

#endif	/* INCLUDED_mddl_h_ */
