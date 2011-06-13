#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>
#include "mddl.h"
#include "mddl-core.h"

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

static int
safe_strcmp(const char *a, const char *b)
{
	if (a && b) {
		return strcmp(a, b);
	}
	/* a string is always equal to the NULL string :O */
	return 0;
}

static size_t
safe_fputs(const char *s, FILE *whither)
{
	if (s) {
		return fputs(s, whither);
	}
	return 0;
}


static void __attribute__((noinline))
print_code(mddl_clo_t clo, mddl_code_t c)
{
	FILE *out = clo->out;
	const char *scheme = clo->code->scheme;

	if (safe_strcmp(c->scheme, scheme) != 0) {
		return;
	}
	safe_fputs(c->scheme, out);
	fputc('\t', out);
	safe_fputs(c->Enumeration, out);
	if (!__source_null_p(c->source)) {
		fputc('\t', out);
		safe_fputs(c->source->Simple, out);
	}
	fputc('\n', out);
	return;
}

static void
search_mktidn(mddl_clo_t clo, mddl_marketIdentifier_t mi)
{
	for (size_t i = 0; i < mi->ncode; i++) {
		print_code(clo, mi->code + i);
	}
	return;
}

static void
search_insidn(mddl_clo_t clo, mddl_instrumentIdentifier_t ii)
{
	for (size_t i = 0; i < ii->ncode; i++) {
		print_code(clo, ii->code + i);
	}
	for (size_t i = 0; i < ii->nmarketIdentifier; i++) {
		search_mktidn(clo, ii->marketIdentifier + i);
	}
	return;
}

static void
search_insdom(mddl_clo_t clo, mddl_instrumentDomain_t id)
{
	for (size_t j = 0; j < id->ninstrumentIdentifier; j++) {
		search_insidn(clo, id->instrumentIdentifier + j);
	}
	return;
}

static void
search_snap(mddl_clo_t clo, mddl_snap_t s)
{
	for (size_t i = 0; i < s->ninstrumentDomain; i++) {
		search_insdom(clo, s->instrumentDomain + i);
	}
	return;
}


void
mddl_cmd_code(mddl_clo_t clo, mddl_doc_t doc)
{
	const size_t indent = 0;
	mddl_mddl_t tree = doc->tree;

	/* try and find candidates */
        for (size_t i = 0; i < doc->tree->nsnap; i++) {
		search_snap(clo, doc->tree->snap + i);
        }
	return;
}

/* mddl-cmd-code.c ends here */
