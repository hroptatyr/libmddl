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

	idx = choi->nchoice++;
	choi->snap = realloc(
		choi->snap, choi->nchoice * sizeof(*choi->snap));
	snap = choi->snap + idx;
	/* intialise snap */
	memset(snap, 0, sizeof(*snap));
	return snap;
}

static mddl_dom_t
__snap_choi_find_domain(mddl_g_snap_choi_t snch, enum domains_e dom_type)
{
	for (size_t i = 0; i < snch->ndomains; i++) {
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
	size_t idx = snch->ndomains;

	snch->domains = realloc(
		snch->domains, (++snch->ndomains) * sizeof(*snch->domains));
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
		if (p->basic_ident_gt == ident_type) {
			return p;
		}
	}
	return NULL;
}

static struct __g_basic_idents_s*
__insdom_add_basic_ident(mddl_dom_instr_t idom, enum basic_idents_e ident_type)
{
	struct __g_basic_idents_s *p;
	size_t idx = idom->nbasic_idents;

	idom->basic_idents = realloc(
		idom->basic_idents,
		(++idom->nbasic_idents) * sizeof(*idom->basic_idents));
	p = idom->basic_idents + idx;
	/* initialise p somehow, we need a named enum here it seems */
	memset(p, 0, sizeof(*p));
	p->basic_ident_gt = ident_type;
	p->nidents = 0;
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

	idx = bi->nidents++;
	bi->instr_ident = realloc(
		bi->instr_ident,
		bi->nidents * sizeof(*bi->instr_ident));
	res = bi->instr_ident + idx;
	/* initialise res somehow */
	memset(res, 0, sizeof(*res));
	return res;
}

/* mddl.c ends here */
