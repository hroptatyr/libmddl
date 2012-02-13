/*** mddl-core.c -- libmddl command line utility
 *
 * Copyright (C) 2011 Sebastian Freundt
 *
 * Author:  Sebastian Freundt <sebastian.freundt@ga-group.nl>
 *
 * This file is part of the army of unserding daemons.
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
 ***/

#if defined HAVE_CONFIG_H
# include "config.h"
#endif	/* HAVE_CONFIG_H */
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

#define VER	"mddl v" VERSION

const char version[] = VER;
static const char usage[] = VER "\n\
\n\
Usage: mddl [-h|--help] [-V|--version]\n\
  or : mddl [OPTIONS] COMMAND [COMMAND-OPTIONS]\n\
\n\
Fiddle with mddl files.\n\
\n\
  -h, --help            Print help and exit\n\
  -V, --version         Print version and exit\n\
\n\
Supported commands:\n\
\n\
  print FILE            Read FILE and print in normalised form\n\
\n\
  code [OPTIONS] FILE   Read FILE and print codes\n\
    -s, --scheme=URI    Only print codes that match URI\n\
\n\
  name [OPTIONS] FILE   Read FILE and print names along with their contexts\n\
    -c, --code=CODE     Only print names in sections that contain code tags\n\
                        that match CODE\n\
    -s, --scheme=URI    Only match codes whose scheme is URI\n\
\n\
  objective FILE        Read FILE and print objectives\n\
\n\
  merge FILEs...        Merge FILEs (somewhat) intelligently\n\
\n\
";


/* main routines */
static int
mddl_process(struct __clo_s *clo)
{
	int res = -1;
	mddl_doc_t doc = NULL;

	/* for all commands that need a file */
	switch (clo->cmd) {
	case MDDL_CMD_PRINT:
	case MDDL_CMD_CODE:
	case MDDL_CMD_NAME:
	case MDDL_CMD_OBJECTIVE: {
		const char *f = clo->print->file;
		struct stat st = {0};

		/* special thing so we can process pipes */
		if (f == NULL || (f[0] == '-' && f[1] == '\0')) {
			f = "/dev/stdin";
		} else if (stat(f, &st) < 0 && errno == ENOENT) {
			fprintf(stderr, "Cannot open %s, no such file\n", f);
			res = -1;
			goto out;
		}
		/* just try and parse whatever we've got */
		if ((doc = mddl_cmd_parse(f)) == NULL) {
			fprintf(stderr, "Could not parse %s\n", f);
			res = -1;
			goto out;
		}
		break;
	}
	default:
		break;
	}

	switch (clo->cmd) {
	case MDDL_CMD_PRINT:
		clo->out = stdout;
		mddl_cmd_print(clo, doc);
		res = 0;
		break;
	case MDDL_CMD_CODE:
		clo->out = stdout;
		mddl_cmd_code(clo, doc);
		res = 0;
		break;
	case MDDL_CMD_NAME:
		clo->out = stdout;
		mddl_cmd_name(clo, doc);
		res = 0;
		break;
	case MDDL_CMD_OBJECTIVE:
		clo->out = stdout;
		mddl_cmd_objective(clo, doc);
		res = 0;
		break;
	case MDDL_CMD_MERGE:
		clo->out = stdout;
		res = mddl_cmd_merge(clo);
		break;
	default:
		break;
	}
out:
	return res;
}


/* command line parsing */
static void
pr_unknown(const char *arg)
{
	fprintf(stderr, "unknown option %s\n", arg);
	return;
}

static char*
__get_val(int *i, size_t len, char *argv[])
{
	char *p = argv[*i];

	switch (p[len]) {
	case '\0':
		p = argv[*i + 1];
		argv[*i] = NULL;
		argv[*i + 1] = NULL;
		(*i)++;
		break;
	case '=':
		p += len + 1;
		argv[*i] = NULL;
		break;
	default:
		pr_unknown(argv[*i]);
		p = NULL;
	}
	return p;
}

static void
parse_print_args(struct __clo_s *clo, int argc, char *argv[])
{
	for (int i = 0; i < argc; i++) {
		char *p = argv[i];

		if (clo->print->file == NULL) {
			/* must be a file name then */
			clo->print->file = argv[i];
			argv[i] = NULL;
		}
	}
	return;
}

static void
parse_code_args(struct __clo_s *clo, int argc, char *argv[])
{
	for (int i = 0; i < argc; i++) {
		char *p = argv[i];

		if (p[0] == '-' && p[1] != '\0') {
			/* could be -s or --scheme */
			if (p[1] == 's') {
				clo->code->scheme = __get_val(&i, 2, argv);
			} else if (strncmp(p + 1, "-scheme", 7) == 0) {
				clo->code->scheme = __get_val(&i, 8, argv);
			}
		} else if (clo->code->file == NULL) {
			/* must be a file name then */
			clo->code->file = argv[i];
			argv[i] = NULL;
		}
	}
	return;
}

static void
parse_name_args(struct __clo_s *clo, int argc, char *argv[])
{
	for (int i = 0; i < argc; i++) {
		char *p = argv[i];

		if (p[0] == '-' && p[1] != '\0') {
			/* could be -s or --scheme */
			if (p[1] == 's') {
				clo->name->scheme = __get_val(&i, 2, argv);
			} else if (strncmp(p + 1, "-scheme", 7) == 0) {
				clo->name->scheme = __get_val(&i, 8, argv);
			} else if (p[1] == 'c') {
				clo->name->code = __get_val(&i, 2, argv);
			} else if (strncmp(p + 1, "-code", 5) == 0) {
				clo->name->code = __get_val(&i, 6, argv);
			}
		} else if (clo->name->file == NULL) {
			/* must be a file name then */
			clo->name->file = argv[i];
			argv[i] = NULL;
		}
	}
	return;
}

static void
parse_objctv_args(struct __clo_s *clo, int argc, char *argv[])
{
	for (int i = 0; i < argc; i++) {
		char *p = argv[i];

		if (clo->objctv->file == NULL) {
			/* must be a file name then */
			clo->objctv->file = argv[i];
			argv[i] = NULL;
		}
	}
	return;
}

static void
parse_merge_args(struct __clo_s *clo, int argc, char *argv[])
{
/* extract options we want to pass down to the merge subcommand
 * at the moment there's no additional options, just files */
	clo->merge->nfiles = argc;
	clo->merge->files = argv;
	return;
}

static void
parse_args(struct __clo_s *clo, int argc, char *argv[])
{
	for (int i = 0; i < argc; i++) {
		char *p = argv[i];

		if (p == NULL) {
			/* args may be set to NULL during the course */
			continue;
		}

		switch (*p++) {
		case '-':
			/* global option */
			switch (*p++) {
			case '-':
				/* long opt */
				if (strcmp(p, "help") == 0) {
					clo->helpp = 1;
					argv[i] = NULL;
					continue;
				} else if (strcmp(p, "version") == 0) {
					clo->cmd = MDDL_CMD_VERSION;
					argv[i] = NULL;
					return;
				}
				break;
			case 'V':
				if (*p == '\0') {
					/* it's -V */
					clo->cmd = MDDL_CMD_VERSION;
					argv[i] = NULL;
					return;
				}
				break;
			case 'h':
				if (*p == '\0') {
					/* it's -h */
					clo->helpp = 1;
					argv[i] = NULL;
					continue;
				}
				break;
			}
			break;
		case 'p': {
			/* print */
			int new_argc = argc - i - 1;
			char **new_argv = argv + i + 1;
			if (strcmp(p, "rint") == 0) {
				clo->cmd = MDDL_CMD_PRINT;
				parse_print_args(clo, new_argc, new_argv);
				continue;
			}
			break;
		}
		case 'c': {
			/* code */
			int new_argc = argc - i - 1;
			char **new_argv = argv + i + 1;
			if (strcmp(p, "ode") == 0) {
				clo->cmd = MDDL_CMD_CODE;
				parse_code_args(clo, new_argc, new_argv);
				continue;
			}
			break;
		}
		case 'n': {
			/* name */
			int new_argc = argc - i - 1;
			char **new_argv = argv + i + 1;
			if (strcmp(p, "ame") == 0) {
				clo->cmd = MDDL_CMD_NAME;
				parse_name_args(clo, new_argc, new_argv);
				continue;
			}
			break;
		}
		case 'm': {
			/* merge */
			int new_argc = argc - i - 1;
			char **new_argv = argv + i + 1;
			if (strcmp(p, "erge") == 0) {
				clo->cmd = MDDL_CMD_MERGE;
				parse_merge_args(clo, new_argc, new_argv);
				continue;
			}
			break;
		}
		case 'o': {
			/* objective */
			int new_argc = argc - i - 1;
			char **new_argv = argv + i + 1;
			if (strcmp(p, "bjective") == 0) {
				clo->cmd = MDDL_CMD_OBJECTIVE;
				parse_objctv_args(clo, new_argc, new_argv);
				continue;
			}
			break;
		}
		default:
			break;
		}

		/* if we end up here, something could not be parsed,
		 * make that depend on the actual command we're calling */
		switch (clo->cmd) {
		case MDDL_CMD_MERGE:
			break;
		default:
			pr_unknown(argv[i]);
			break;
		}
	}
	return;
}

static void
print_usage(mddl_cmd_t UNUSED(cmd))
{
	fputs(usage, stdout);
	return;
}

static void
print_version(void)
{
	fputs(VER, stdout);
	fputc('\n', stdout);
	return;
}


int
main(int argc, char *argv[])
{
	struct __clo_s argi = {0};

	/* parse them command line */
	parse_args(&argi, argc - 1, argv + 1);

	if (argi.helpp && argi.cmd == MDDL_CMD_UNK) {
		/* command specific help? la'ers */
		print_usage(argi.cmd);
		return 0;
	} else if (argi.cmd == MDDL_CMD_VERSION) {
		print_version();
		return 0;
	}

	/* now go go go */
	if (UNLIKELY((mddl_process(&argi)) < 0)) {
		return 1;
	}
	return 0;
}

/* mddl-core.c ends here */
