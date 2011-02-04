/** mddl.c - mddl stubs and snippets */
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "mddl.h"

#if defined __INTEL_COMPILER
# pragma warning (disable:177)
#endif	/* __INTEL_COMPILER */
#if !defined UNUSED
# define UNUSED(_x)	__attribute__((unused)) _x
#endif	/* !UNUSED */

typedef struct __g_domains_s *mddl_dom_t;
typedef struct __g_snap_choi_s *mddl_g_snap_choi_t;
typedef struct __g_mddl_choi_s *mddl_g_mddl_choi_t;


#define add_p(res, obj, prop)			\
	do {					\
		size_t idx;			\
						\
		idx = (obj)->n##prop##++;	\
		(obj)->prop = realloc(		\
			(obj)->prop,		\
			(obj)->n##prop *	\
			sizeof(*(obj)->prop));	\
		res = (obj)->prop + idx;	\
		/* initialise res somehow */	\
		memset(res, 0, sizeof(*res));	\
	} while (0)


static mddl_g_mddl_choi_t
__mddl_find_choice(mddl_doc_t m, enum mddl_choi_e mtype)
{
	mddl_g_mddl_choi_t p = m->choice;
	if (p->mddl_choi_gt == mtype) {
		return p;
	}
	return NULL;
}

static mddl_g_mddl_choi_t
__mddl_add_choice(mddl_doc_t m, enum mddl_choi_e mtype)
{
	mddl_g_mddl_choi_t p;

	/* already alloc'd */
	p = m->choice;
	/* initialise p somehow, we need a named enum here it seems */
	memset(p, 0, sizeof(*p));
	p->mddl_choi_gt = mtype;
	return p;
}

DEFUN mddl_snap_t
mddl_add_snap(mddl_doc_t doc)
{
	mddl_g_mddl_choi_t choi;
	mddl_snap_t snap;
	size_t idx;

	if (!(choi = __mddl_find_choice(doc, MDDL_CHOICE_SNAP)) &&
	    !(choi = __mddl_add_choice(doc, MDDL_CHOICE_SNAP))) {
		return NULL;
	}

	idx = choi->nmddl_choi++;
	choi->snap = realloc(
		choi->snap, choi->nmddl_choi * sizeof(*choi->snap));
	snap = choi->snap + idx;
	/* intialise snap */
	memset(snap, 0, sizeof(*snap));
	return snap;
}

static mddl_dom_t
__snap_choi_find_domain(mddl_g_snap_choi_t snch, enum domains_e dom_type)
{
	for (size_t i = 0; i < snch->nsnap_choi; i++) {
		mddl_dom_t p = snch->domains + i;
		if (p->domains_gt == dom_type) {
			return p;
		}
	}
	return NULL;
}

static mddl_dom_t
__snap_choi_add_domain(mddl_g_snap_choi_t snch, enum domains_e dom_type)
{
	mddl_dom_t p;
	size_t idx = snch->nsnap_choi;

	snch->domains = realloc(
		snch->domains, (++snch->nsnap_choi) * sizeof(*snch->domains));
	p = snch->domains + idx;
	/* initialise p somehow, we need a named enum here it seems */
	memset(p, 0, sizeof(*p));
	p->domains_gt = dom_type;
	p->ndomains = 0;
	return p;
}

static mddl_g_snap_choi_t
__snap_find_snap_choice(mddl_snap_t snap, enum snap_choi_e snap_choice)
{
	mddl_g_snap_choi_t p = snap->choice;
	if (p->snap_choi_gt == snap_choice) {
		return p;
	}
	return NULL;
}

static mddl_g_snap_choi_t
__snap_add_snap_choice(mddl_snap_t snap, enum snap_choi_e snap_choice)
{
	mddl_g_snap_choi_t p;

	/* already alloc'd */
	p = snap->choice;
	/* initialise p somehow, we need a named enum here it seems */
	memset(p, 0, sizeof(*p));
	p->snap_choi_gt = snap_choice;
	return p;
}

DEFUN mddl_dom_instr_t
mddl_snap_add_dom_instr(mddl_snap_t snap)
{
	mddl_g_snap_choi_t choi;
	mddl_dom_t dom;
	mddl_dom_instr_t res = NULL;
	size_t idx;

	if (!(choi = __snap_find_snap_choice(snap, MDDL_SNAP_CHOICE_DOMAINS)) &&
	    !(choi = __snap_add_snap_choice(snap, MDDL_SNAP_CHOICE_DOMAINS))) {
		return NULL;
	}

	if (!(dom = __snap_choi_find_domain(choi, MDDL_DOM_INSTRUMENT)) &&
	    !(dom = __snap_choi_add_domain(choi, MDDL_DOM_INSTRUMENT))) {
		return NULL;
	}

	idx = dom->ndomains++;
	dom->instrument = realloc(
		dom->instrument,
		dom->ndomains * sizeof(*dom->instrument));
	res = dom->instrument + idx;
	/* initialise the result */
	memset(res, 0, sizeof(*res));
	return res;
}

static struct __g_basic_idents_s*
__insdom_find_basic_ident(mddl_dom_instr_t idom, enum basic_idents_e ident_type)
{
	for (size_t i = 0; i < idom->nbasic_idents; i++) {
		struct __g_basic_idents_s *p = idom->basic_idents + i;
		if (p->basic_idents_gt == ident_type) {
			return p;
		}
	}
	return NULL;
}

static struct __g_basic_idents_s*
__insdom_add_basic_ident(mddl_dom_instr_t idom, enum basic_idents_e ident_type)
{
	struct __g_basic_idents_s *p;

	add_p(p, idom, basic_idents);
	p->basic_idents_gt = ident_type;
	p->nbasic_idents = 0;
	return p;
}

DEFUN mddl_p_instr_ident_t
mddl_dom_instr_add_instr_ident(mddl_dom_instr_t insdom)
{
	mddl_p_instr_ident_t res = NULL;
	struct __g_basic_idents_s *bi = NULL;
	size_t idx;

	if (!(bi = __insdom_find_basic_ident(
		      insdom, MDDL_BASIC_IDENT_INSTR_IDENT)) &&
	    !(bi = __insdom_add_basic_ident(
		      insdom, MDDL_BASIC_IDENT_INSTR_IDENT))) {
		return NULL;
	}

	idx = bi->nbasic_idents++;
	bi->instr_ident = realloc(
		bi->instr_ident,
		bi->nbasic_idents * sizeof(*bi->instr_ident));
	res = bi->instr_ident + idx;
	/* initialise res somehow */
	memset(res, 0, sizeof(*res));
	return res;
}

DEFUN mddl_p_issue_data_t
mddl_dom_instr_add_issue_data(mddl_dom_instr_t insdom)
{
	mddl_p_issue_data_t res = NULL;
	struct __g_basic_idents_s *bi = NULL;
	size_t idx;

	if (!(bi = __insdom_find_basic_ident(
		      insdom, MDDL_BASIC_IDENT_ISSUE_DATA)) &&
	    !(bi = __insdom_add_basic_ident(
		      insdom, MDDL_BASIC_IDENT_ISSUE_DATA))) {
		return NULL;
	}

	idx = bi->nbasic_idents++;
	bi->issue_data = realloc(
		bi->issue_data,
		bi->nbasic_idents * sizeof(*bi->issue_data));
	res = bi->issue_data + idx;
	/* initialise res somehow */
	memset(res, 0, sizeof(*res));
	return res;
}

DEFUN mddl_p_objective_t
mddl_dom_instr_add_objective(mddl_dom_instr_t insdom)
{
	mddl_p_objective_t res;

	add_p(res, insdom, objective);
	return res;
}

static struct __g_code_name_s*
__instr_ident_find_code_name(mddl_p_instr_ident_t iid, enum code_name_e type)
{
	for (size_t i = 0; i < iid->ncode_name; i++) {
		struct __g_code_name_s *p = iid->code_name + i;
		if (p->code_name_gt == type) {
			return p;
		}
	}
	return NULL;
}

static struct __g_code_name_s*
__instr_ident_add_code_name(mddl_p_instr_ident_t iid, enum code_name_e type)
{
	struct __g_code_name_s *p;

	add_p(p, iid, code_name);
	p->code_name_gt = type;
	p->ncode_name = 0;
	return p;
}

DEFUN mddl_p_name_t
mddl_instr_ident_add_name(mddl_p_instr_ident_t iid)
{
	mddl_p_name_t res = NULL;
	struct __g_code_name_s *cn = NULL;
	size_t idx;

	if (!(cn = __instr_ident_find_code_name(iid, MDDL_CODE_NAME_NAME)) &&
	    !(cn = __instr_ident_add_code_name(iid, MDDL_CODE_NAME_NAME))) {
		return NULL;
	}

	idx = cn->ncode_name++;
	cn->name = realloc(cn->name, cn->ncode_name * sizeof(*cn->name));
	res = cn->name + idx;
	/* initialise res somehow */
	memset(res, 0, sizeof(*res));
	return res;
}

DEFUN mddl_p_code_t
mddl_instr_ident_add_code(mddl_p_instr_ident_t iid)
{
	mddl_p_code_t res = NULL;
	struct __g_code_name_s *cn = NULL;
	size_t idx;

	if (!(cn = __instr_ident_find_code_name(iid, MDDL_CODE_NAME_CODE)) &&
	    !(cn = __instr_ident_add_code_name(iid, MDDL_CODE_NAME_CODE))) {
		return NULL;
	}

	idx = cn->ncode_name++;
	cn->code = realloc(cn->code, cn->ncode_name * sizeof(*cn->code));
	res = cn->code + idx;
	/* initialise res somehow */
	memset(res, 0, sizeof(*res));
	return res;
}

DEFUN mddl_p_instr_data_t
mddl_instr_ident_add_instr_data(mddl_p_instr_ident_t iid)
{
	mddl_p_instr_data_t res = NULL;

	add_p(res, iid, instr_data);
	return res;
}

DEFUN mddl_p_issuer_ref_t
mddl_issue_data_add_issuer_ref(mddl_p_issue_data_t id)
{
	mddl_p_issuer_ref_t res = NULL;

	add_p(res, id, issuer_ref);
	return res;
}

DEFUN mddl_p_issue_date_t
mddl_issue_data_add_issue_date(mddl_p_issue_data_t id)
{
	mddl_p_issue_date_t res = NULL;

	add_p(res, id, issue_date);
	return res;
}

DEFUN mddl_p_issue_amount_t
mddl_issue_data_add_issue_amount(mddl_p_issue_data_t id)
{
	mddl_p_issue_amount_t res = NULL;

	add_p(res, id, issue_amount);
	return res;
}


static struct __g_code_name_s*
__issuer_ref_find_code_name(mddl_p_issuer_ref_t ir, enum code_name_e type)
{
	for (size_t i = 0; i < ir->ncode_name; i++) {
		struct __g_code_name_s *p = ir->code_name + i;
		if (p->code_name_gt == type) {
			return p;
		}
	}
	return NULL;
}

static struct __g_code_name_s*
__issuer_ref_add_code_name(mddl_p_issuer_ref_t ir, enum code_name_e type)
{
	struct __g_code_name_s *p;

	add_p(p, ir, code_name);
	p->code_name_gt = type;
	p->ncode_name = 0;
	return p;
}

DEFUN mddl_p_name_t
mddl_issuer_ref_add_name(mddl_p_issuer_ref_t ir)
{
	mddl_p_name_t res = NULL;
	struct __g_code_name_s *cn = NULL;
	size_t idx;

	if (!(cn = __issuer_ref_find_code_name(ir, MDDL_CODE_NAME_NAME)) &&
	    !(cn = __issuer_ref_add_code_name(ir, MDDL_CODE_NAME_NAME))) {
		return NULL;
	}

	idx = cn->ncode_name++;
	cn->name = realloc(cn->name, cn->ncode_name * sizeof(*cn->name));
	res = cn->name + idx;
	/* initialise res somehow */
	memset(res, 0, sizeof(*res));
	return res;
}

DEFUN mddl_p_code_t
mddl_issuer_ref_add_code(mddl_p_issuer_ref_t ir)
{
	mddl_p_code_t res = NULL;
	struct __g_code_name_s *cn = NULL;
	size_t idx;

	if (!(cn = __issuer_ref_find_code_name(ir, MDDL_CODE_NAME_CODE)) &&
	    !(cn = __issuer_ref_add_code_name(ir, MDDL_CODE_NAME_CODE))) {
		return NULL;
	}

	idx = cn->ncode_name++;
	cn->code = realloc(cn->code, cn->ncode_name * sizeof(*cn->code));
	res = cn->code + idx;
	/* initialise res somehow */
	memset(res, 0, sizeof(*res));
	return res;
}


DEFUN __a_instr_type_t
mddl_instr_data_add_instr_type(mddl_p_instr_data_t id, const char *type)
{
	__a_instr_type_t *res = NULL;
	size_t idx;

	idx = id->ninstr_type++;
	id->instr_type = realloc(
		id->instr_type, id->ninstr_type * sizeof(*id->instr_type));
	return id->instr_type[idx] = strdup(type);
}

DEFUN mddl_p_currency_t
mddl_instr_data_add_currency(mddl_p_instr_data_t id)
{
	mddl_p_currency_t res = NULL;

	add_p(res, id, currency);
	return res;
}


DEFUN __a_scheme_t
mddl_code_add_scheme(mddl_p_code_t code, const char *scheme)
{
	return code->scheme = strdup(scheme);
}

DEFUN __a_rank_t
mddl_code_add_rank(mddl_p_code_t code, int rank)
{
	size_t idx = code->nrank++;
	code->rank = realloc(code->rank, (code->nrank) * sizeof(*code->rank));
	return code->rank[idx] = rank;
}

DEFUN __a_role_t
mddl_name_add_role(mddl_p_name_t name, const char *role)
{
	size_t idx = name->nrole++;
	name->role = realloc(name->role, (name->nrole) * sizeof(*name->role));
	return name->role[idx] = strdup(role);
}

DEFUN __a_rank_t
mddl_name_add_rank(mddl_p_name_t name, int rank)
{
	size_t idx = name->nrank++;
	name->rank = realloc(name->rank, (name->nrank) * sizeof(*name->rank));
	return name->rank[idx] = rank;
}

/* mddl.c ends here */
