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

#define DEF_ADD_F(name, nty, slot, slotty, slotname, body...)		\
	DEFUN slotty(slot)*						\
	mddl_##name##_add_##slot(nty(name) *name)			\
	{								\
		slotty(slot) *res = NULL;				\
		add_p(res, name, slotname);					\
		body							\
		return res;						\
	}								\
	/* just to get rid of stray semicolon warnings */		\
	typedef void mddl_##name##_add_##slot##_f

#define DEFP_ADD_PROPF(name, prop, body...)				\
	DEF_ADD_F(name, MDDL_PROP, prop, MDDL_PROP, prop, body)
#define DEFP_ADD_GROUPF(name, grp, body...)				\
	DEF_ADD_F(name, MDDL_PROP, grp, MDDL_GROUP, grp, body)
#define DEFG_ADD_PROPF(name, prop, body...)				\
	DEF_ADD_F(name, MDDL_GROUP, prop, MDDL_PROP, prop, body)
#define DEFG_ADD_GROUPF(name, grp, body...)				\
	DEF_ADD_F(name, MDDL_GROUP, grp, MDDL_GROUP, grp, body)

/* for when slot type and slot name are not the same */
#define DEFP_ADD_PROPF_SN(name, prop, sn, body...)		\
	DEF_ADD_F(name, MDDL_PROP, prop, MDDL_PROP, sn, body)
#define DEFP_ADD_GROUPF_SN(name, grp, sn, body...)		\
	DEF_ADD_F(name, MDDL_PROP, grp, MDDL_GROUP, sn, body)
#define DEFG_ADD_PROPF_SN(name, prop, sn, body...)			\
	DEF_ADD_F(name, MDDL_GROUP, prop, MDDL_PROP, sn, body)
#define DEFG_ADD_GROUPF_SN(name, grp, sn, body...)		\
	DEF_ADD_F(name, MDDL_GROUP, grp, MDDL_GROUP, sn, body)


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

DEFUN __a_fund_strat_type_t
mddl_dom_instr_add_fund_strat_type(mddl_dom_instr_t idom, const char *fst)
{
	size_t idx = idom->nfund_strat_type++;
	idom->fund_strat_type = realloc(
		idom->fund_strat_type,
		(idom->nfund_strat_type) * sizeof(*idom->fund_strat_type));
	return idom->fund_strat_type[idx] = strdup(fst);
}

DEFUN __a_distri_type_t
mddl_dom_instr_add_distri_type(mddl_dom_instr_t idom, const char *dt)
{
	size_t idx = idom->ndistri_type++;
	idom->distri_type = realloc(
		idom->distri_type,
		(idom->ndistri_type) * sizeof(*idom->distri_type));
	return idom->distri_type[idx] = strdup(dt);
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

DEFP_ADD_PROPF(instr_ident, mkt_ident);
DEFP_ADD_PROPF(instr_ident, instr_data);
DEFP_ADD_PROPF(instr_ident, seg_ident);
DEFP_ADD_PROPF(instr_ident, tranche);

DEFP_ADD_PROPF(issue_data, issuer_ref);
DEFP_ADD_PROPF(issue_data, issue_date);
DEFP_ADD_PROPF(issue_data, issue_amount);
DEFP_ADD_PROPF(issue_data, issue_fees);
DEFP_ADD_PROPF(issue_data, clearing_stlmnt);

DEFP_ADD_PROPF(clearing_stlmnt, mkt_ident);
DEFP_ADD_PROPF(clearing_stlmnt, currency);
DEFP_ADD_PROPF(clearing_stlmnt, clearing_house);
DEFP_ADD_PROPF_SN(clearing_stlmnt, clearing_proc, clearing_process);
DEFP_ADD_PROPF_SN(clearing_stlmnt, clearing_sys, clearing_system);
DEFP_ADD_PROPF_SN(clearing_stlmnt, depo_name, depository_name);
DEFP_ADD_PROPF_SN(clearing_stlmnt, depo_sys, depository_system);
DEFP_ADD_PROPF_SN(clearing_stlmnt, parties_invlv, parties_involved);

DEFP_ADD_PROPF(crossrate, multiplier);

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

static struct __g_clsf_price_s*
__issue_amount_find_clsf_price(mddl_p_issue_amount_t ia, enum clsf_price_e type)
{
	for (size_t i = 0; i < ia->nclsf_price; i++) {
		struct __g_clsf_price_s *p = ia->clsf_price + i;
		if (p->clsf_price_gt == type) {
			return p;
		}
	}
	return NULL;
}

static struct __g_clsf_price_s*
__issue_amount_add_clsf_price(mddl_p_issue_amount_t ia, enum clsf_price_e type)
{
	struct __g_clsf_price_s *p;

	add_p(p, ia, clsf_price);
	p->clsf_price_gt = type;
	p->nclsf_price = 0;
	return p;
}

DEFUN mddl_p_currency_t
mddl_issue_amount_add_currency(mddl_p_issue_amount_t iamt)
{
	mddl_p_currency_t res = NULL;
	struct __g_clsf_price_s *cp = NULL;
	size_t idx;

	if (!(cp = __issue_amount_find_clsf_price(
		      iamt, MDDL_CLSF_PRICE_CURRENCY)) &&
	    !(cp = __issue_amount_add_clsf_price(
		      iamt, MDDL_CLSF_PRICE_CURRENCY))) {
		return NULL;
	}

	idx = cp->nclsf_price++;
	cp->currency = realloc(
		cp->currency, cp->nclsf_price * sizeof(*cp->currency));
	res = cp->currency + idx;
	/* initialise res somehow */
	memset(res, 0, sizeof(*res));
	return res;
}

DEFUN mddl_p_crossrate_t
mddl_issue_amount_add_crossrate(mddl_p_issue_amount_t iamt)
{
	mddl_p_crossrate_t res = NULL;
	struct __g_clsf_price_s *cp = NULL;
	size_t idx;

	if (!(cp = __issue_amount_find_clsf_price(
		      iamt, MDDL_CLSF_PRICE_CROSSRATE)) &&
	    !(cp = __issue_amount_add_clsf_price(
		      iamt, MDDL_CLSF_PRICE_CROSSRATE))) {
		return NULL;
	}

	idx = cp->nclsf_price++;
	cp->crossrate = realloc(
		cp->crossrate, cp->nclsf_price * sizeof(*cp->crossrate));
	res = cp->crossrate + idx;
	/* initialise res somehow */
	memset(res, 0, sizeof(*res));
	return res;
}

DEFUN mddl_p_size_t
mddl_issue_amount_add_size(mddl_p_issue_amount_t iamt)
{
	mddl_p_size_t res = NULL;
	struct __g_clsf_price_s *cp = NULL;
	size_t idx;

	if (!(cp = __issue_amount_find_clsf_price(
		      iamt, MDDL_CLSF_PRICE_SIZE)) &&
	    !(cp = __issue_amount_add_clsf_price(
		      iamt, MDDL_CLSF_PRICE_SIZE))) {
		return NULL;
	}

	idx = cp->nclsf_price++;
	cp->size = realloc(
		cp->size, cp->nclsf_price * sizeof(*cp->size));
	res = cp->size + idx;
	/* initialise res somehow */
	memset(res, 0, sizeof(*res));
	return res;
}

static struct __g_clsf_price_s*
__issue_fees_find_clsf_price(mddl_p_issue_fees_t ia, enum clsf_price_e type)
{
	for (size_t i = 0; i < ia->nclsf_price; i++) {
		struct __g_clsf_price_s *p = ia->clsf_price + i;
		if (p->clsf_price_gt == type) {
			return p;
		}
	}
	return NULL;
}

static struct __g_clsf_price_s*
__issue_fees_add_clsf_price(mddl_p_issue_fees_t ia, enum clsf_price_e type)
{
	struct __g_clsf_price_s *p;

	add_p(p, ia, clsf_price);
	p->clsf_price_gt = type;
	p->nclsf_price = 0;
	return p;
}

DEFUN mddl_p_currency_t
mddl_issue_fees_add_currency(mddl_p_issue_fees_t ifee)
{
	mddl_p_currency_t res = NULL;
	struct __g_clsf_price_s *cp = NULL;
	size_t idx;

	if (!(cp = __issue_fees_find_clsf_price(
		      ifee, MDDL_CLSF_PRICE_CURRENCY)) &&
	    !(cp = __issue_fees_add_clsf_price(
		      ifee, MDDL_CLSF_PRICE_CURRENCY))) {
		return NULL;
	}

	idx = cp->nclsf_price++;
	cp->currency = realloc(
		cp->currency, cp->nclsf_price * sizeof(*cp->currency));
	res = cp->currency + idx;
	/* initialise res somehow */
	memset(res, 0, sizeof(*res));
	return res;
}

DEFUN mddl_p_crossrate_t
mddl_issue_fees_add_crossrate(mddl_p_issue_fees_t ifee)
{
	mddl_p_crossrate_t res = NULL;
	struct __g_clsf_price_s *cp = NULL;
	size_t idx;

	if (!(cp = __issue_fees_find_clsf_price(
		      ifee, MDDL_CLSF_PRICE_CROSSRATE)) &&
	    !(cp = __issue_fees_add_clsf_price(
		      ifee, MDDL_CLSF_PRICE_CROSSRATE))) {
		return NULL;
	}

	idx = cp->nclsf_price++;
	cp->crossrate = realloc(
		cp->crossrate, cp->nclsf_price * sizeof(*cp->crossrate));
	res = cp->crossrate + idx;
	/* initialise res somehow */
	memset(res, 0, sizeof(*res));
	return res;
}

DEFUN mddl_p_size_t
mddl_issue_fees_add_size(mddl_p_issue_fees_t ifee)
{
	mddl_p_size_t res = NULL;
	struct __g_clsf_price_s *cp = NULL;
	size_t idx;

	if (!(cp = __issue_fees_find_clsf_price(
		      ifee, MDDL_CLSF_PRICE_SIZE)) &&
	    !(cp = __issue_fees_add_clsf_price(
		      ifee, MDDL_CLSF_PRICE_SIZE))) {
		return NULL;
	}

	idx = cp->nclsf_price++;
	cp->size = realloc(
		cp->size, cp->nclsf_price * sizeof(*cp->size));
	res = cp->size + idx;
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

DEFP_ADD_PROPF(instr_data, currency);
DEFP_ADD_PROPF(instr_data, tra_restr_type);
DEFP_ADD_PROPF(instr_data, last_cae);


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

DEFP_ADD_PROPF(issuer, instr_ident);
DEFP_ADD_PROPF(issuer, mkt_cap);

DEFP_ADD_PROPF(day_of_week, hours);
DEFP_ADD_PROPF(ordinal_day, day_of_week);
DEFP_ADD_PROPF(start, ordinal_day);
DEFP_ADD_PROPF(end, ordinal_day);

DEFP_ADD_PROPF(period, start);
DEFP_ADD_PROPF(period, end);

DEFP_ADD_PROPF(name, period);
DEFP_ADD_PROPF(code, period);

DEFP_ADD_PROPF(days, day_of_week);

DEFP_ADD_PROPF(mkt_cond, period);
DEFP_ADD_PROPF(mkt_cond, days);

DEFP_ADD_PROPF(valua_ref, valua_base);

DEFP_ADD_PROPF(currency, multiplier);

DEFP_ADD_PROPF(clearing_house, period);
DEFP_ADD_PROPF(clearing_proc, period);
DEFP_ADD_PROPF(clearing_sys, period);
DEFP_ADD_PROPF(depo_name, period);
DEFP_ADD_PROPF(depo_sys, period);
DEFP_ADD_PROPF(parties_invlv, agent);

DEFP_ADD_PROPF(post_code, period);

DEFP_ADD_PROPF(orderbook, bid);
DEFP_ADD_PROPF(orderbook, ask);

/* mddl.c ends here */
