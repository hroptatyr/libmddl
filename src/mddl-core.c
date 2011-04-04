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

extern mddl_doc_t mddl_cmd_parse(const char *file);
extern mddl_doc_t mddl_cmd_print(FILE *file, mddl_doc_t);

typedef enum {
	MDDL_CMD_UNK,
	MDDL_CMD_VERSION,
	MDDL_CMD_PRINT,
} mddl_cmd_t;

/* new_pf specific options */
struct __print_clo_s {
	const char *file;
};

/* command line options */
struct __clo_s {
	int helpp;

	mddl_cmd_t cmd;
	union {
		struct __print_clo_s print[1];
	};
};

#define VER	"mddl v" VERSION "\n"

static const char usage[] = VER "\
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
  print [OPTIONS] FILE       Read FILE and print in normalised form\n\
\n\
";


/* main routines */
static int
mddl_process(struct __clo_s *clo)
{
	int res = -1;

	switch (clo->cmd) {
	case MDDL_CMD_PRINT: {
		const char *f = clo->print->file;
		struct stat st = {0};
		mddl_doc_t doc;

		if (f == NULL) {
			fputs("print command needs FILE argument.\n", stderr);
		} else if (stat(f, &st) < 0 && errno == ENOENT) {
			fprintf(stderr, "Cannot open %s, no such file\n", f);
		} else if ((doc = mddl_cmd_parse(f)) == NULL) {
			fprintf(stderr, "Could not parse %s\n", f);
		} else {
			mddl_cmd_print(stdout, doc);
			res = 0;
		}
		break;
	}
	default:
		break;
	}
	return res;
}


/* command line parsing */
static void
pr_unknown(const char *arg)
{
	fprintf(stderr, "unknown option %s\n", arg);
	return;
}

static void
parse_print_args(struct __clo_s *clo, int argc, char *argv[])
{
/* also used for new-pf */
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
					continue;
				} else if (strcmp(p, "version") == 0) {
					clo->cmd = MDDL_CMD_VERSION;
					return;
				}
				break;
			case 'V':
				if (*p == '\0') {
					/* it's -V */
					clo->cmd = MDDL_CMD_VERSION;
					return;
				}
				break;
			case 'h':
				if (*p == '\0') {
					/* it's -h */
					clo->helpp = 1;
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
		default:
			break;
		}

		/* if we end up here, something could not be parsed */
		pr_unknown(argv[i]);
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
	return;
}


int
main(int argc, char *argv[])
{
	struct __clo_s argi = {0};

	/* parse them command line */
	parse_args(&argi, argc - 1, argv + 1);

	if (argi.helpp) {
		/* command specific help? la'ers */
		print_usage(argi.cmd);
		return 0;
	}

	switch (argi.cmd) {
	case MDDL_CMD_UNK:
	default:
		print_usage(argi.cmd);
		return 1;
	case MDDL_CMD_VERSION:
		print_version();
		return 0;
	case MDDL_CMD_PRINT:
		break;
	}

	/* now go go go */
	if (UNLIKELY((mddl_process(&argi)) < 0)) {
		return 1;
	}
	return 0;
}

/* mddl-core.c ends here */
