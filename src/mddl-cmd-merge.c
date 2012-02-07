/*** mddl-cmd-merge.c -- merge two (or more) mddl files
 *
 * Copyright (C) 2012 Sebastian Freundt
 *
 * Author:  Sebastian Freundt <freundt@ga-group.nl>
 *
 * This file is part of libmddl.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the author nor the names of any contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **/
/**
 * Merging as defined by us is always pairwise and type-locked, so a
 * snap can be merged to another snap, but not to 2 snap structures at
 * once nor can an instrumentDomain be merge *into* an existing snap
 *
 * The exact arithmetics are yet to be worked out and they may be more
 * intelligent in the future allowing for mergers of best matches.
 * Right now the decision what to merge onto what is order-dependent
 * and the currently used algorithm is:
 *
 * snap + snap:
 *   merge first instrument domains
 *   append other instrument domains
 *   merge first objectives
 *   append other objectives
 *   ...
 *
 * ins dom + ins dom:
 *   merge first instrument identifiers
 *   append other instrument identifiers
 *   ...
 *
 * many more to be documented. */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
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

static mddl_doc_t
__read_file(const char *f)
{
	struct stat st = {0};
	mddl_doc_t res = NULL;

	/* special thing so we can process pipes */
	if (f == NULL) {
		/* we shouldn't be here */
		goto out;
	} else if (f[0] == '-' && f[1] == '\0') {
		f = "/dev/stdin";
	} else if (stat(f, &st) < 0 && errno == ENOENT) {
		fprintf(stderr, "Cannot open %s, no such file\n", f);
		goto out;
	}
	/* just try and parse whatever we've got */
	if ((res = mddl_cmd_parse(f)) == NULL) {
		fprintf(stderr, "Could not parse %s\n", f);
		goto out;
	}
out:
	return res;
}


static bool
__scopeType_sim_p(mddl_scopeType_t a, mddl_scopeType_t b)
{
	if (strcmp(a->Enumeration, b->Enumeration) == 0) {
		return true;
	}
	return false;
}

static bool
__code_sim_p(mddl_code_t a, mddl_code_t b)
{
	if (strcmp(a->scheme, b->scheme) == 0 &&
	    strcmp(a->Enumeration, b->Enumeration) == 0) {
		return true;
	}
	return false;
}

static bool
__name_sim_p(mddl_name_t a, mddl_name_t b)
{
	if (strcmp(a->Simple, b->Simple) == 0) {
		return true;
	}
	return false;
}

#define DEFALLSIM(__par, __type)					\
static bool								\
__##__type##_allsim_p(mddl_##__par##_t a, mddl_##__par##_t b)		\
{									\
	if (a->n##__type != b->n##__type) {				\
		return false;						\
	}								\
	for (size_t i = 0; i < a->n##__type; i++) {			\
		mddl_##__type##_t __type##a = a->__type + i;		\
		bool subres = false;					\
		for (size_t j = 0; j < b->n##__type; j++) {		\
			mddl_##__type##_t __type##b = b->__type + j;	\
			if (__##__type##_sim_p(__type##a, __type##b)) { \
				subres = true;				\
				break;					\
			}						\
		}							\
		if (!subres) {						\
			return false;					\
		}							\
	}								\
	return true;							\
}

DEFALLSIM(instrumentIdentifier, scopeType); 
DEFALLSIM(instrumentIdentifier, code); 

static bool
__insidn_sim_p(mddl_instrumentIdentifier_t a, mddl_instrumentIdentifier_t b)
{
/* return non-FALSE if A and B are similar in some sense
 * - same scopeType(s)
 * - same code scheme(s) and code(s) */
	bool res = false;

	/* check if the scopes match */
	__scopeType_allsim_p(a, b);

	if (a->nmarketIdentifier > 0 || b->nmarketIdentifier > 0) {
		__code_allsim_p(a, b);
	}
	return true;
}


static int
__merge_insidn(mddl_instrumentIdentifier_t tgt, mddl_instrumentIdentifier_t src)
{
	/* for ins idns to be merged the scopes must match ... */
	if (__insidn_sim_p(tgt, src)) {
		return 0;
	}
	return -1;
}

static int
__merge_objective(mddl_objective_t tgt, mddl_objective_t src)
{
/* objectives can be merged if they're 100% identical */
	return strcmp(tgt->Simple, src->Simple) == 0 ? 0 : -1;
}

static int
__merge_insdom(mddl_instrumentDomain_t tgt, mddl_instrumentDomain_t src)
{
#define __add_insidn	mddl_instrumentDomain_add_instrumentIdentifier
#define __add_objective	mddl_instrumentDomain_add_objective
	size_t nmrg = 1;

	/* make sure there's instrument identifiers */
	if (tgt->ninstrumentIdentifier == 0 && src->ninstrumentIdentifier > 0) {
		/* no merge now */
		nmrg = 0;
	} else if (src->ninstrumentIdentifier > 0) {
		/* first gets merged ... */
		mddl_instrumentIdentifier_t tgtii = tgt->instrumentIdentifier;
		if (__merge_insidn(tgtii, src->instrumentIdentifier) < 0) {
			/* error merging them, so append */
			nmrg = 0;
		}
	}
	/* ... append the rest */
	for (size_t i = nmrg; i < src->ninstrumentIdentifier; i++) {
		mddl_instrumentIdentifier_t tii = __add_insidn(tgt);
		mddl_instrumentIdentifier_t sii = src->instrumentIdentifier + i;
		memcpy(tii, sii, sizeof(*sii));
	}

	/* same for objectives, no merge for them */
	nmrg = 1;
	if (tgt->nobjective == 0 && src->nobjective > 0) {
		/* no merge now */
		nmrg = 0;
	} else if (src->nobjective > 0) {
		/* try a merge ... */
		mddl_objective_t tobj = tgt->objective;
		if (__merge_objective(tobj, src->objective) < 0) {
			/* error merging them, so append */
			nmrg = 0;
		}
	}
	for (size_t i = 0; i < src->nobjective; i++) {
		mddl_objective_t tobj = __add_objective(tgt);
		mddl_objective_t sobj = src->objective + i;
		memcpy(tobj, sobj, sizeof(*sobj));
	}
	return 0;
}

static int
__merge_snap(mddl_snap_t tgt, mddl_snap_t src)
{
#define __add_insdom	mddl_snap_add_instrumentDomain
	size_t nmrg = 1;

	/* make sure there's at least one insdom on both sides */
	if (tgt->ninstrumentDomain == 0 && src->ninstrumentDomain > 0) {
		/* no merge in this case */
		nmrg = 0;
	} else if (src->ninstrumentDomain > 0) {
		/* first one gets merged ... */
		mddl_instrumentDomain_t tdom = tgt->instrumentDomain;
		if (__merge_insdom(tdom, src->instrumentDomain) < 0) {
			/* error merging them, so append */
			nmrg = 0;
		}
	}
	/* ... and others just get appended */
	for (size_t i = nmrg; i < src->ninstrumentDomain; i++) {
		mddl_instrumentDomain_t tdom = __add_insdom(tgt);
		mddl_instrumentDomain_t sdom = src->instrumentDomain + i;
		memcpy(tdom, sdom, sizeof(*sdom));
	}
	return 0;
}

static mddl_doc_t
__merge(mddl_doc_t tgtsrc, mddl_doc_t src)
{
#define __add_snap	mddl_mddl_add_snap
	mddl_mddl_t troot;
	mddl_mddl_t sroot;
	size_t nmrg = 1;

	if ((troot = tgtsrc->tree) == NULL) {
		return NULL;
	} else if ((sroot = src->tree) == NULL) {
		return tgtsrc;
	}
	/* make sure there's (at least) one snap in troot */
	if (troot->nsnap == 0 && sroot->nsnap > 0) {
		/* don't bother merging in this case */
		nmrg = 0;
	} else if (sroot->nsnap > 0) {
		/* if sroot also has a snap, the first ones get merged ... */
		if (__merge_snap(troot->snap, sroot->snap) < 0) {
			nmrg = 0;
		}
	}
	/* ... others just appended */
	for (size_t i = nmrg; i < sroot->nsnap; i++) {
		mddl_snap_t newsn = __add_snap(troot);
		mddl_snap_t ssnap = sroot->snap + i;
		memcpy(newsn, ssnap, sizeof(*ssnap));
	}
	return tgtsrc;
}


/* help stuff */
static const char sub_usage[] = "  merge FILEs...";
static const char expl_1ln[] = "Merge FILEs (somewhat) intelligently";

static void
print_usage(mddl_clo_t UNUSED(clo))
{
	fputs(version, stdout);
	fputc('\n', stdout);
	fputc('\n', stdout);
	fputs("Usage: mddl [OPTIONS]", stdout);
	fputs(sub_usage + 1, stdout);
	fputc('\n', stdout);
	fputc('\n', stdout);
	fputs(expl_1ln, stdout);
	fputc('\n', stdout);
	fputc('\n', stdout);
	return;
}


int
mddl_cmd_merge(mddl_clo_t clo)
{
	const size_t indent = 0;
	mddl_doc_t res_doc;
	int res = 0;

	/* check if it's just a help request */
	if (clo->helpp) {
		print_usage(clo);
		goto out;
	}
	/* we destructively modify the first parsed document */
	if (clo->merge->nfiles == 0 ||
	    (res_doc = __read_file(clo->merge->files[0])) == NULL) {
		res = -1;
		goto out;
	}
	/* otherwise we have a valid target doc ... */
	for (size_t i = 1; i < clo->merge->nfiles; i++) {
		/* ... and merge the stuff one by one */
		const char *f = clo->merge->files[i];
		mddl_doc_t tmp;

		if (f != NULL && (tmp = __read_file(f)) != NULL) {
			(void)__merge(res_doc, tmp);
		}
	}
	/* fiddle with clo and print the result */
	clo->print->file = NULL;
	mddl_cmd_print(clo, res_doc);
out:
	return res;
}

/* mddl-cmd-merge.c ends here */
