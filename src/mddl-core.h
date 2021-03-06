/*** mddl-core.h -- our universe of commands
 *
 * Copyright (C) 2010 - 2012  Sebastian Freundt
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

#if !defined INCLUDED_mddl_core_h_
#define INCLUDED_mddl_core_h_

#include <stdio.h>

typedef enum __cmd_e mddl_cmd_t;
typedef struct __clo_s *mddl_clo_t;

/* commands we support */
enum __cmd_e {
	MDDL_CMD_UNK,
	MDDL_CMD_VERSION,
	MDDL_CMD_PRINT,
	MDDL_CMD_CODE,
	MDDL_CMD_NAME,
	MDDL_CMD_OBJECTIVE,
	MDDL_CMD_MERGE,
};

/* new_pf specific options */
struct __print_clo_s {
	const char *file;
};

struct __code_clo_s {
	const char *file;
	const char *scheme;
};

struct __name_clo_s {
	const char *file;
	const char *scheme;
	const char *code;
};

struct __objctv_clo_s {
	const char *file;
};

struct __merge_clo_s {
	size_t nfiles;
	char *const *files;
};

/* command line options */
struct __clo_s {
	int helpp;

	mddl_cmd_t cmd;
	union {
		struct __print_clo_s print[1];
		struct __code_clo_s code[1];
		struct __name_clo_s name[1];
		struct __objctv_clo_s objctv[1];
		struct __merge_clo_s merge[1];
	};

	/* output options */
	FILE *out;
};


extern mddl_doc_t mddl_cmd_parse(const char *file);
extern void mddl_cmd_print(mddl_clo_t, mddl_doc_t);
extern void mddl_cmd_code(mddl_clo_t, mddl_doc_t);
extern void mddl_cmd_name(mddl_clo_t, mddl_doc_t);
extern void mddl_cmd_objective(mddl_clo_t, mddl_doc_t);

/**
 * merge FILE [FILEs...]
 * (somewhat) intelligently merge FILE and FILEs */
extern int mddl_cmd_merge(mddl_clo_t);

/* helper for usage output */
extern const char version[];

#endif	/* INCLUDED_mddl_core_h_ */
