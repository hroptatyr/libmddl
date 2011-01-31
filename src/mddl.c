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


DEFUN mddl_snap_t
mddl_add_snap(mddl_doc_t doc)
{
	mddl_snap_t res = NULL;
	size_t idx;

	if ((idx = doc->choice->nchoice) == 0) {
		doc->choice->mddl_choi_gt = MDDL_CHOICE_SNAP;
		doc->choice->snap =
			malloc((++doc->choice->nchoice) *
			       sizeof(*doc->choice->snap));
		res = doc->choice->snap;
	} else if (doc->choice->mddl_choi_gt == MDDL_CHOICE_SNAP) {
		doc->choice->snap =
			realloc(doc->choice->snap,
				(++doc->choice->nchoice) *
				sizeof(*doc->choice->snap));
		res = doc->choice->snap + idx;
	}
	return res;
}

static mddl_dom_t
__snap_find_domain(mddl_snap_t snap, int dom_type)
{
	for (size_t i = 0; i < snap->choice->ndomains; i++) {
		mddl_dom_t p = snap->choice->domains + i;
		if (p->domains_gt == dom_type) {
			return p;
		}
	}
	return NULL;
}

static mddl_dom_t
__snap_add_domain(mddl_snap_t snap, int dom_type)
{
	mddl_dom_t p;
	size_t idx = snap->choice->ndomains;

	snap->choice->domains = realloc(
		snap->choice->domains,
		(++snap->choice->ndomains) * sizeof(*snap->choice->domains));
	p = snap->choice->domains + idx;
	/* initialise p somehow, we need a named enum here it seems */
	p->domains_gt = dom_type;
	p->ndomains = 0;
	return p;
}

DEFUN mddl_dom_instr_t
mddl_snap_add_dom_instr(mddl_snap_t snap)
{
	mddl_dom_t dom;
	mddl_dom_instr_t res = NULL;
	size_t idx;

	if ((idx = snap->choice->ndomains) == 0) {
		snap->choice->snap_choi_gt = MDDL_SNAP_CHOICE_DOMAINS;
		dom = snap->choice->domains =
			malloc((++snap->choice->ndomains) *
			       sizeof(*snap->choice->domains));
		dom->domains_gt = MDDL_DOM_INSTRUMENT;
		res = dom->instrument + idx;

	} else if (snap->choice->snap_choi_gt == MDDL_SNAP_CHOICE_DOMAINS) {
		/* trivially true coz there are no other choices */
		if (!(dom = __snap_find_domain(snap, MDDL_DOM_INSTRUMENT)) &&
		    !(dom = __snap_add_domain(snap, MDDL_DOM_INSTRUMENT))) {
			return NULL;
		}
		idx = dom->ndomains++;
		dom->instrument = realloc(
			dom->instrument,
			dom->ndomains * sizeof(*dom->instrument));
		res = dom->instrument + idx;
	}
	return res;
}

static struct __g_basic_idents_s*
__insdom_find_basic_ident(mddl_dom_instr_t insdom, int ident_type)
{
	for (size_t i = 0; i < insdom->nbasic_idents; i++) {
		struct __g_basic_idents_s *p = insdom->basic_idents + i;
		if (p->basic_ident_gt == ident_type) {
			return p;
		}
	}
	return NULL;
}

static struct __g_basic_idents_s*
__insdom_add_basic_ident(mddl_dom_instr_t insdom, int ident_type)
{
	struct __g_basic_idents_s *p;
	size_t idx = insdom->nbasic_idents;

	insdom->basic_idents = realloc(
		insdom->basic_idents,
		(++insdom->nbasic_idents) * sizeof(*insdom->basic_idents));
	p = insdom->basic_idents + idx;
	/* initialise p somehow, we need a named enum here it seems */
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

	if ((idx = insdom->nbasic_idents) == 0) {
		bi = insdom->basic_idents =
			malloc((++insdom->nbasic_idents) *
			       sizeof(*insdom->basic_idents));
		bi->basic_ident_gt = MDDL_BASIC_IDENT_INSTR_IDENT;
		bi->nidents = 0;

	} else if (!(bi = __insdom_find_basic_ident(
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
	return res;
}

/* mddl.c ends here */
