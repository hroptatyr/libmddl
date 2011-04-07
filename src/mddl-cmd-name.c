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
print_name(FILE *out, mddl_name_t n, const char *ctx)
{
	fputs(ctx, out);
	fputc('\t', out);
	fputs(n->Simple, out);
	fputc('\t', out);
	if (!__source_null_p(n->source)) {
		fputs(n->source->Simple, out);
	}
	fputc('\n', out);
	return;
}

static void
search_mktidn(FILE *out, mddl_marketIdentifier_t mi)
{
	for (size_t i = 0; i < mi->nname; i++) {
		print_name(out, mi->name + i, "mi");
	}
	return;
}

static void
search_insidn(FILE *out, mddl_instrumentIdentifier_t ii)
{
	for (size_t i = 0; i < ii->nname; i++) {
		print_name(out, ii->name + i, "ii");
	}
	for (size_t i = 0; i < ii->nmarketIdentifier; i++) {
		search_mktidn(out, ii->marketIdentifier + i);
	}
	return;
}

static void
search_insdom(FILE *out, mddl_instrumentDomain_t id)
{
	for (size_t j = 0; j < id->ninstrumentIdentifier; j++) {
		search_insidn(out, id->instrumentIdentifier + j);
	}
	return;
}

static void
search_snap(FILE *out, mddl_snap_t s)
{
	for (size_t i = 0; i < s->ninstrumentDomain; i++) {
		search_insdom(out, s->instrumentDomain + i);
	}
	return;
}


void
mddl_cmd_name(FILE *out, mddl_doc_t doc)
{
	const size_t indent = 0;
	mddl_mddl_t tree = doc->tree;

	/* try and find candidates */
        for (size_t i = 0; i < doc->tree->nsnap; i++) {
		search_snap(out, doc->tree->snap + i);
        }
	return;
}

/* mddl-cmd-name.c ends here */
