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
	if (f == NULL || (f[0] == '-' && f[1] == '\0')) {
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


int
mddl_cmd_merge(mddl_clo_t clo)
{
	const size_t indent = 0;
	mddl_doc_t doc_from;
	mddl_doc_t doc_to;
	mddl_mddl_t from;
	mddl_mddl_t to;
	int res = 0;

	if ((doc_from = __read_file(clo->merge->file1)) == NULL ||
	    (doc_to = __read_file(clo->merge->file2)) == NULL) {
		res = -1;
		goto out;
	}

out:
	return res;
}

/* mddl-cmd-merge.c ends here */
