#if !defined INCLUDED_mddl_core_h_
#define INCLUDED_mddl_core_h_

#include <stdio.h>

typedef enum __cmd_e mddl_cmd_t;
typedef struct __clo_s *mddl_clo_t;

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

/* command line options */
struct __clo_s {
	int helpp;

	mddl_cmd_t cmd;
	union {
		struct __print_clo_s print[1];
		struct __code_clo_s code[1];
		struct __name_clo_s name[1];
	};

	/* output options */
	FILE *out;
};


extern mddl_doc_t mddl_cmd_parse(const char *file);
extern void mddl_cmd_print(mddl_clo_t, mddl_doc_t);
extern void mddl_cmd_code(mddl_clo_t, mddl_doc_t);
extern void mddl_cmd_name(mddl_clo_t, mddl_doc_t);

#endif	/* INCLUDED_mddl_core_h_ */
