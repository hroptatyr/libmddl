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

#define CHECK_SIM(__type, __a, __b)					\
	if ((__a)->n##__type != (__b)->n##__type) {			\
		return false;						\
	}								\
	for (size_t i = 0; i < (__a)->n##__type; i++) {			\
		mddl_##__type##_t __type##a = (__a)->__type + i;	\
		bool subres = false;					\
		for (size_t j = 0; j < (__b)->n##__type; j++) {		\
			mddl_##__type##_t __type##b = (__b)->__type + j; \
			if (__##__type##_sim_p(__type##a, __type##b)) { \
				subres = true;				\
				break;					\
			}						\
		}							\
		if (!subres) {						\
			return false;					\
		}							\
	}

static bool
__insidn_sim_p(mddl_instrumentIdentifier_t a, mddl_instrumentIdentifier_t b)
{
/* return non-FALSE if A and B are similar in some sense
 * - same scopeType(s)
 * - same code scheme(s) and code(s) */
	bool res = false;

	/* check if the scopes match */
	CHECK_SIM(scopeType, a, b);

	if (a->nmarketIdentifier > 0 || b->nmarketIdentifier > 0) {
		CHECK_SIM(code, a, b);
	}
	return true;
}

static mddl_instrumentIdentifier_t
__insdom_insidn(mddl_instrumentDomain_t lis, mddl_instrumentIdentifier_t ii)
{
/* return non-NULL iff LIS has an instrument identifier similar to II
 * Similar here means: see __insidn_sim_p() */
	for (size_t i = 0; i < lis->ninstrumentIdentifier; i++) {
		mddl_instrumentIdentifier_t tii = lis->instrumentIdentifier + i;

		if (__insidn_sim_p(tii, ii)) {
			return tii;
		}
	}
	return NULL;
}

static void
__merge_insidn(mddl_instrumentIdentifier_t tgt, mddl_instrumentIdentifier_t src)
{
	return;
}

static void
__merge_insdom(mddl_instrumentDomain_t tgt, mddl_instrumentDomain_t src)
{
	/* merge instrument identifiers */
	for (size_t i = 0; i < src->ninstrumentIdentifier; i++) {
		mddl_instrumentIdentifier_t ii = src->instrumentIdentifier + i;
		mddl_instrumentIdentifier_t tgtii;

#define __add_insidn	mddl_instrumentDomain_add_instrumentIdentifier
		if ((tgtii = __insdom_insidn(tgt, ii)) == NULL) {
			tgtii = __add_insidn(tgt);
			memcpy(tgtii, ii, sizeof(*ii));
		}
#undef __add_insidn
	}
	return;
}

static void
__merge_snap(mddl_snap_t tgt, mddl_snap_t src)
{
	/* merge instrument domain */
	if (tgt->ninstrumentDomain == 1 && src->ninstrumentDomain >= 1) {
		/* first one gets merged ... */
		__merge_insdom(tgt->instrumentDomain, src->instrumentDomain);
	}
	/* ... and others just get appended */
	for (size_t i = 1; i < src->ninstrumentDomain; i++) {
		mddl_instrumentDomain_t newdom =
			mddl_snap_add_instrumentDomain(tgt);
		mddl_instrumentDomain_t sdom = src->instrumentDomain + i;
		memcpy(newdom, sdom, sizeof(*sdom));
	}
	return;
}

static mddl_doc_t
__merge(mddl_doc_t tgtsrc, mddl_doc_t src)
{
	mddl_mddl_t troot;
	mddl_mddl_t sroot;

	if ((troot = tgtsrc->tree) == NULL) {
		return NULL;
	} else if ((sroot = src->tree) == NULL) {
		return tgtsrc;
	}
	/* make sure there's (at least) one snap in troot */
	if (troot->nsnap == 0) {
		troot->snap = mddl_mddl_add_snap(troot);
		/* don't bother merging in this case */
		if (sroot->nsnap > 0) {
			memcpy(troot->snap, sroot->snap, sizeof(*sroot->snap));
		}
	} else if (sroot->nsnap > 0) {
		/* if sroot also has a snap, the first ones get merged ... */
		__merge_snap(troot->snap, sroot->snap);
	}
	/* ... others just appended */
	for (size_t i = 1; i < sroot->nsnap; i++) {
		mddl_snap_t newsn = mddl_mddl_add_snap(troot);
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
