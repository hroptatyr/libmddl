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


static void
print_objctv(mddl_clo_t clo, mddl_objective_t n)
{
	/* we could print lang and source as well, good? */
	FILE *out = clo->out;

	fputs(n->Simple, out);
	fputc('\n', out);
	return;
}

static void
search_insdom(mddl_clo_t clo, mddl_instrumentDomain_t id)
{
	for (size_t j = 0; j < id->nobjective; j++) {
		print_objctv(clo, id->objective + j);
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
mddl_cmd_objective(mddl_clo_t clo, mddl_doc_t doc)
{
	const size_t indent = 0;
	mddl_mddl_t tree = doc->tree;

	/* try and find candidates */
        for (size_t i = 0; i < doc->tree->nsnap; i++) {
		search_snap(clo, doc->tree->snap + i);
        }
	return;
}

/* mddl-cmd-objective.c ends here */
