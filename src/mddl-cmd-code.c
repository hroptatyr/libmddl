#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>
#include "mddl.h"

#if defined __INTEL_COMPILER
# pragma warning (disable:869)
# pragma warning (disable:177)
#endif	/* __INTEL_COMPILER */
#if !defined UNUSED
# define UNUSED(_x)	__attribute__((unused)) _x##_unused
#endif	/* !UNUSED */
#if !defined LIKELY
# define LIKELY(_x)	__builtin_expect((_x), 1)
#endif
#if !defined UNLIKELY
# define UNLIKELY(_x)	__builtin_expect((_x), 0)
#endif
#define countof(_x)	(sizeof(_x) / sizeof(*_x))


static void
print_code(FILE *out, mddl_code_t c, const char *scheme)
{
	if (scheme != NULL && strcmp(c->scheme, scheme) != 0) {
		return;
	}
	fputs(c->scheme, out);
	fputc('\t', out);
	fputs(c->Enumeration, out);
	if (!__source_null_p(c->source)) {
		fputs(c->source->Simple, out);
	}
	fputc('\n', out);
	return;
}

static void
search_mktidn(FILE *out, mddl_marketIdentifier_t mi, const char *scheme)
{
	for (size_t i = 0; i < mi->ncode; i++) {
		print_code(out, mi->code + i, scheme);
	}
	return;
}

static void
search_insidn(FILE *out, mddl_instrumentIdentifier_t ii, const char *scheme)
{
	for (size_t i = 0; i < ii->ncode; i++) {
		print_code(out, ii->code + i, scheme);
	}
	for (size_t i = 0; i < ii->nmarketIdentifier; i++) {
		search_mktidn(out, ii->marketIdentifier + i, scheme);
	}
	return;
}

static void
search_insdom(FILE *out, mddl_instrumentDomain_t id, const char *scheme)
{
	for (size_t j = 0; j < id->ninstrumentIdentifier; j++) {
		search_insidn(out, id->instrumentIdentifier + j, scheme);
	}
	return;
}

static void
search_snap(FILE *out, mddl_snap_t s, const char *scheme)
{
	for (size_t i = 0; i < s->ninstrumentDomain; i++) {
		search_insdom(out, s->instrumentDomain + i, scheme);
	}
	return;
}


void
mddl_cmd_code(FILE *out, mddl_doc_t doc, const char *scheme)
{
	const size_t indent = 0;
	mddl_mddl_t tree = doc->tree;

	/* try and find candidates */
        for (size_t i = 0; i < doc->tree->nsnap; i++) {
		search_snap(out, doc->tree->snap + i, scheme);
        }
	return;
}

/* mddl-cmd-code.c ends here */
