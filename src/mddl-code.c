#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>
#include <libxml/parser.h>
#include "mddl.h"

/* gperf goodness */
#include "mddl-tag.c"
#include "mddl-attr.c"

/* include the command line parser */
#if defined __INTEL_COMPILER
# pragma warning (disable:181)
# pragma warning (disable:593)
# pragma warning (disable:869)
#endif	/* __INTEL_COMPILER */
#include "mddl-code-opt.h"
#include "mddl-code-opt.c"
#if defined __INTEL_COMPILER
# pragma warning (default:181)
# pragma warning (default:593)
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

typedef struct mddl_ctx_s *mddl_ctx_t;
typedef xmlSAXHandler sax_hdl_s;
typedef sax_hdl_s *sax_hdl_t;
typedef struct mddl_ctxcb_s *mddl_ctxcb_t;

struct mddl_ns_s {
	char *pref;
	char *href;
};

/* contextual callbacks */
struct __ctxcb_s {
	/* principal types callbacks,
	 * sf is for strings (and mdStrings)
	 * dt is for dateTime (and mdDateTime) */
	void(*sf)(mddl_ctxcb_t ctx, const char *str, size_t len);
	void(*dtf)(mddl_ctxcb_t ctx, time_t date_time);
	void(*prif)(mddl_ctxcb_t ctx, double price);
	void(*amtf)(mddl_ctxcb_t ctx, double amount);
	void(*rtf)(mddl_ctxcb_t ctx, double rate);
};

struct mddl_ctxcb_s {
	/* for a linked list */
	mddl_ctxcb_t next;

	struct __ctxcb_s cb[1];
	/* navigation info, stores the context */
	mddl_tid_t otype;
	void *object;
	mddl_ctxcb_t old_state;
};

struct mddl_ctx_s {
	mddl_doc_t doc;
	struct mddl_ns_s ns[16];
	size_t nns;
	/* stuff buf */
#define INITIAL_STUFF_BUF_SIZE	(4096)
	char *sbuf;
	size_t sbsz;
	size_t sbix;
	/* the current sax handler */
	sax_hdl_s hdl[1];
	/* parser state, for contextual callbacks */
	mddl_ctxcb_t state;
	/* pool of context trackers, implies maximum parsing depth */
	struct mddl_ctxcb_s ctxcb_pool[16];
	mddl_ctxcb_t ctxcb_head;
};

#if 0
static const char w3_xsi_uri[] = "http://www.w3.org/2001/XMLSchema-instance";
static const char mddl_xsi_loc[] = "mddl-3.0-beta-full.xsd";
#endif	/* 0 */
static const char mddl_xsi_uri[] = "http://www.mddl.org/mddl/3.0-beta";


static void
init_ctxcb(mddl_ctx_t ctx)
{
	memset(ctx->ctxcb_pool, 0, sizeof(ctx->ctxcb_pool));
	for (int i = 0; i < countof(ctx->ctxcb_pool) - 1; i++) {
		ctx->ctxcb_pool[i].next = ctx->ctxcb_pool + i + 1;
	}
	ctx->ctxcb_head = ctx->ctxcb_pool;
	return;
}

static mddl_ctxcb_t
pop_ctxcb(mddl_ctx_t ctx)
{
	mddl_ctxcb_t res = ctx->ctxcb_head;

	if (LIKELY(res != NULL)) {
		ctx->ctxcb_head = res->next;
		memset(res, 0, sizeof(*res));
	}
	return res;
}

static void
push_ctxcb(mddl_ctx_t ctx, mddl_ctxcb_t ctxcb)
{
	ctxcb->next = ctx->ctxcb_head;
	ctx->ctxcb_head = ctxcb;
	return;
}

static void
pop_state(mddl_ctx_t ctx)
{
/* restore the previous current state */
	mddl_ctxcb_t curr = ctx->state;

	ctx->state = curr->old_state;
	/* queue him in our pool */
	push_ctxcb(ctx, curr);
	return;
}

static mddl_ctxcb_t
push_state(mddl_ctx_t ctx, mddl_tid_t otype, void *object)
{
	mddl_ctxcb_t res = pop_ctxcb(ctx);

	/* stuff it with the object we want to keep track of */
	res->object = object;
	res->otype = otype;
	/* fiddle with the states in our context */
	res->old_state = ctx->state;
	ctx->state = res;
	return res;
}

static mddl_tid_t
get_state_otype(mddl_ctx_t ctx)
{
	return ctx->state ? ctx->state->otype : MDDL_TAG_UNK;
}

static void*
get_state_object(mddl_ctx_t ctx)
{
	return ctx->state->object;
}

static void*
get_state_object_if(mddl_ctx_t ctx, mddl_tid_t otype)
{
/* like get_state_object() but return NULL if types do not match */
	if (LIKELY(get_state_otype(ctx) == otype)) {
		return get_state_object(ctx);
	}
	return NULL;
}


static void __attribute__((unused))
zulu_stamp(char *buf, size_t bsz, time_t stamp)
{
	struct tm tm = {0};
	gmtime_r(&stamp, &tm);
	strftime(buf, bsz, "%Y-%m-%dT%H:%M:%S%z", &tm);
	return;
}

static void __attribute__((unused))
zulu_stamp_now(char *buf, size_t bsz)
{
	zulu_stamp(buf, bsz, time(NULL));
	return;
}

static time_t
get_zulu(const char *buf)
{
	struct tm tm[1] = {{0}};
	time_t res = -1;

	/* skip over whitespace */
	for (; *buf && isspace(*buf); buf++);

	if (strptime(buf, "%Y-%m-%dT%H:%M:%S%Z", tm)) {
		res = timegm(tm);
	} else if (strptime(buf, "%Y-%m-%d", tm)) {
		res = timegm(tm);
	}
	return res;
}

static const char*
tag_massage(const char *tag)
{
/* return the real part of a (ns'd) tag or attribute,
 * i.e. foo:that_tag becomes that_tag */
	const char *p = strchr(tag, ':');

	if (p) {
		/* skip over ':' */
		return p + 1;
	}
	/* otherwise just return the tag as is */
	return tag;
}

static void
mddl_reg_ns(mddl_ctx_t ctx, const char *pref, const char *href)
{
	if (ctx->nns >= countof(ctx->ns)) {
		fputs("too many name spaces\n", stderr);
		return;
	}

	if (UNLIKELY(href == NULL)) {
		/* bollocks, user MUST be a twat */
		return;
	} else if (strcmp(href, mddl_xsi_uri) == 0) {
		/* oh, it's our fave, copy the  */
		ctx->ns[0].pref = pref ? strdup(pref) : NULL;
		ctx->ns[0].href = strdup(href);
	} else {
		size_t i = ctx->nns++;
		ctx->ns[i].pref = pref ? strdup(pref) : NULL;
		ctx->ns[i].href = strdup(href);
	}
	return;
}

static void
mddl_init(mddl_ctx_t ctx, const char **attrs)
{
	static const char att_xmlns[] = "xmlns";

	for (int i = 0; attrs[i] != NULL; i += 2) {
		if (strncmp(attrs[i], att_xmlns, sizeof(att_xmlns) - 1) == 0) {
			const char *pref = tag_massage(attrs[i]);
			const char *href = attrs[i + 1];
			mddl_reg_ns(ctx, pref == attrs[i] ? NULL : pref, href);
		}
	}
	/* initialise the ctxcb pool */
	init_ctxcb(ctx);
	/* alloc some space for our document */
	{
		struct __mddl_s *m = calloc(sizeof(*m), 1);
		mddl_ctxcb_t cc = pop_ctxcb(ctx);

		ctx->doc = m;
		ctx->state = cc;
		cc->old_state = NULL;
		cc->object = m;
		cc->otype = MDDL_TAG_MDDL;
	}
	return;
}

static bool
mddl_pref_p(mddl_ctx_t ctx, const char *pref, size_t pref_len)
{
	/* we sorted our namespaces so that mddl is always at index 0 */
	if (UNLIKELY(ctx->ns[0].href == NULL)) {
		return false;

	} else if (LIKELY(ctx->ns[0].pref == NULL)) {
		/* prefix must not be set here either */
		return pref == NULL || pref_len == 0;

	} else if (UNLIKELY(pref_len == 0)) {
		/* this node's prefix is "" but we expect a prefix of
		 * length > 0 */
		return false;

	} else {
		/* special service for us because we're lazy:
		 * you can pass pref = "foo:" and say pref_len is 4
		 * easier to deal with when strings are const etc. */
		if (pref[pref_len - 1] == ':') {
			pref_len--;
		}
		return memcmp(pref, ctx->ns[0].pref, pref_len) == 0;
	}
}

/* stuff buf handling */
static void
stuff_buf_reset(mddl_ctx_t ctx)
{
	ctx->sbix = 0;
	return;
}

static void
hdr_ass_dt(mddl_ctxcb_t ctx, time_t dt)
{
	mddl_header_t hdr = ctx->object;
	struct __dateTime_s tmp[1] = {{0}};

	mddl_dateTime_set_AnyDateTime(tmp, dt);
	mddl_header_set_dateTime(hdr, tmp);
	return;
}

static void
issdate_ass_dt(mddl_ctxcb_t ctx, time_t dt)
{
	mddl_issueDate_t id = ctx->object;

	mddl_issueDate_set_DateTime(id, dt);
	return;
}

static void
src_ass_s(mddl_ctxcb_t ctx, const char *str, size_t len)
{
	mddl_source_t src = ctx->object;
	mddl_source_set_Simple(src, str);
	return;
}

static void
name_ass_s(mddl_ctxcb_t ctx, const char *str, size_t len)
{
	mddl_name_t n = ctx->object;
	mddl_name_set_Simple(n, str);
	return;
}

static void
code_ass_s(mddl_ctxcb_t ctx, const char *str, size_t len)
{
	mddl_code_t c = ctx->object;
	mddl_code_set_Enumeration(c, str);
	return;
}

static void
ccy_ass_s(mddl_ctxcb_t ctx, const char *str, size_t len)
{
	mddl_currency_t c = ctx->object;
	mddl_currency_set_Enumeration(c, str);
	return;
}

static void
obj_ass_s(mddl_ctxcb_t ctx, const char *str, size_t len)
{
	mddl_objective_t o = ctx->object;
	mddl_objective_set_Simple(o, str);
	return;
}

static void
iamt_ass_pri(mddl_ctxcb_t ctx, double price)
{
	mddl_issueAmount_t amt = ctx->object;
	mddl_issueAmount_set_Price(amt, price);
	return;
}

static void
ifee_ass_pri(mddl_ctxcb_t ctx, double price)
{
	mddl_issueFees_t fee = ctx->object;
	mddl_issueFees_set_Price(fee, price);
	return;
}


static struct __ctxcb_s __hdr_cb = {
	.dtf = hdr_ass_dt,
};

static struct __ctxcb_s __src_cb = {
	.sf = src_ass_s,
};

static struct __ctxcb_s __name_cb = {
	.sf = name_ass_s,
};

static struct __ctxcb_s __code_cb = {
	.sf = code_ass_s,
};

static struct __ctxcb_s __ccy_cb = {
	.sf = ccy_ass_s,
};

static struct __ctxcb_s __obj_cb = {
	.sf = obj_ass_s,
};

static struct __ctxcb_s __issdate_cb = {
	.dtf = issdate_ass_dt,
};

static struct __ctxcb_s __issamt_cb = {
	.prif = iamt_ass_pri,
	/* for compatibility,
	 * normally the issue amount is issue price times # of issues */
	.amtf = iamt_ass_pri,
};

static struct __ctxcb_s __issfee_cb = {
	.prif = ifee_ass_pri,
	/* for compatibility,
	 * normally the issue fees is absolute */
	.rtf = ifee_ass_pri,
};

static mddl_tid_t
sax_tid_from_tag(const char *tag)
{
	size_t tlen = strlen(tag);
	const struct mddl_tag_s *t = __tiddify(tag, tlen);
	return t ? t->tid : MDDL_TAG_UNK;
}

static mddl_aid_t
sax_aid_from_attr(const char *attr)
{
	size_t alen = strlen(attr);
	const struct mddl_attr_s *a = __aiddify(attr, alen);
	return a ? a->aid : MDDL_ATTR_UNK;
}

static void
sax_bo_elt(mddl_ctx_t ctx, const char *name, const char **attrs)
{
	/* where the real element name starts, sans ns prefix */
	const char *rname = tag_massage(name);
	mddl_tid_t tid = sax_tid_from_tag(rname);

	/* check for mddl */
	if (UNLIKELY(tid == MDDL_TAG_MDDL)) {
		mddl_init(ctx, attrs);
		return;
	}

	if (!mddl_pref_p(ctx, name, rname - name)) {
		/* dont know what to do */
		return;
	}

	/* all the stuff that needs a new sax handler */
	switch (tid) {
	case MDDL_TAG_HEADER: {
		/* check that we're inside an mddl context */
		mddl_doc_t m =
			get_state_object_if(ctx, MDDL_TAG_MDDL);
		mddl_ctxcb_t cc;

		if (m && (cc = push_state(ctx, MDDL_TAG_HEADER, m->header))) {
			cc->cb[0] = __hdr_cb;
		}
		break;
	}
	case MDDL_TAG_SOURCE: {
		/* check that we're in a header context */
		mddl_header_t hdr =
			get_state_object_if(ctx, MDDL_TAG_HEADER);
		mddl_ctxcb_t cc;

		if (hdr &&
		    (cc = push_state(ctx, MDDL_TAG_SOURCE, hdr->source))) {
			cc->cb[0] = __src_cb;
		}
		break;
	}
	case MDDL_TAG_SNAP: {
		/* check that we're in an mddl context */
		mddl_doc_t m = get_state_object_if(ctx, MDDL_TAG_MDDL);
		mddl_snap_t s;

		if (m && (s = mddl_mddl_add_snap(m))) {
			push_state(ctx, MDDL_TAG_SNAP, s);
		}
		break;
	}
	case MDDL_TAG_INSTRUMENT_DOMAIN: {
		/* check that we're in a snap context */
		mddl_snap_t m = get_state_object_if(ctx, MDDL_TAG_SNAP);
		mddl_instrumentDomain_t insdom;

		if (m && (insdom = mddl_snap_add_instrumentDomain(m))) {
			push_state(ctx, MDDL_TAG_INSTRUMENT_DOMAIN, insdom);
		}
		break;
	}
	case MDDL_TAG_INSTRUMENT_IDENTIFIER: {
		/* check that we're in a insdom context */
		mddl_instrumentDomain_t insdom =
			get_state_object_if(ctx, MDDL_TAG_INSTRUMENT_DOMAIN);
		mddl_instrumentIdentifier_t iid;

		if (insdom &&
		    (iid = mddl_instrumentDomain_add\
_instrumentIdentifier(insdom))) {
			push_state(ctx, MDDL_TAG_INSTRUMENT_IDENTIFIER, iid);
		}
		break;
	}
	case MDDL_TAG_ISSUE_DATA: {
		/* check that we're in a insdom context */
		mddl_instrumentDomain_t insdom =
			get_state_object_if(ctx, MDDL_TAG_INSTRUMENT_DOMAIN);
		mddl_issueData_t id;

		if (insdom &&
		    (id = mddl_instrumentDomain_add_issueData(insdom))) {
			push_state(ctx, MDDL_TAG_ISSUE_DATA, id);
		}
		break;
	}
	case MDDL_TAG_OBJECTIVE: {
		/* check that we're in an insdom context */
		mddl_instrumentDomain_t insdom =
			get_state_object_if(ctx, MDDL_TAG_INSTRUMENT_DOMAIN);
		mddl_objective_t objctv;
		mddl_ctxcb_t cc;

		if (insdom &&
		    (objctv = mddl_instrumentDomain_add_objective(insdom)) &&
		    (cc = push_state(ctx, MDDL_TAG_OBJECTIVE, objctv))) {
			cc->cb[0] = __obj_cb;
		}
		stuff_buf_reset(ctx);
		break;
	}

/* depth 4 */
	case MDDL_TAG_INSTRUMENT_DATA: {
		mddl_instrumentIdentifier_t iid =
			get_state_object_if(
				ctx, MDDL_TAG_INSTRUMENT_IDENTIFIER);
		mddl_instrumentData_t id;

		if (iid &&
		    (id = mddl_instrumentIdentifier_add_instrumentData(iid))) {
			push_state(ctx, MDDL_TAG_INSTRUMENT_DATA, id);
		}
		break;
	}
	case MDDL_TAG_ISSUER_REF: {
		mddl_issueData_t id =
			get_state_object_if(ctx, MDDL_TAG_ISSUE_DATA);
		mddl_issuerRef_t iref;

		if (id && (iref = mddl_issueData_add_issuerRef(id))) {
			push_state(ctx, MDDL_TAG_ISSUER_REF, iref);
		}
		break;
	}
	case MDDL_TAG_ISSUE_DATE: {
		mddl_issueData_t id =
			get_state_object_if(ctx, MDDL_TAG_ISSUE_DATA);
		mddl_issueDate_t issd;
		mddl_ctxcb_t cc;

		if (id &&
		    (issd = mddl_issueData_add_issueDate(id)) &&
		    (cc = push_state(ctx, MDDL_TAG_ISSUE_DATE, issd))) {
			cc->cb[0] = __issdate_cb;
		}
		break;
	}
	case MDDL_TAG_ISSUE_AMOUNT: {
		mddl_issueData_t id =
			get_state_object_if(ctx, MDDL_TAG_ISSUE_DATA);
		mddl_issueAmount_t iamt;
		mddl_ctxcb_t cc;

		if (id &&
		    (iamt = mddl_issueData_add_issueAmount(id)) &&
		    (cc = push_state(ctx, MDDL_TAG_ISSUE_AMOUNT, iamt))) {
			cc->cb[0] = __issamt_cb;
		}
		break;
	}
	case MDDL_TAG_ISSUE_FEES: {
		mddl_issueData_t id =
			get_state_object_if(ctx, MDDL_TAG_ISSUE_DATA);
		mddl_issueFees_t ifee;
		mddl_ctxcb_t cc;

		if (id &&
		    (ifee = mddl_issueData_add_issueFees(id)) &&
		    (cc = push_state(ctx, MDDL_TAG_ISSUE_FEES, ifee))) {
			cc->cb[0] = __issfee_cb;
		}
		break;
	}
	case MDDL_TAG_CLEARING_SETTLEMENT: {
		mddl_issueData_t id =
			get_state_object_if(ctx, MDDL_TAG_ISSUE_DATA);
		mddl_clearingSettlement_t cs;

		if (id && (cs = mddl_issueData_add_clearingSettlement(id))) {
			push_state(ctx, MDDL_TAG_CLEARING_SETTLEMENT, cs);
		}
		break;
	}
	case MDDL_TAG_SETTLEMENT_TYPE: {
		mddl_clearingSettlement_t cs =
			get_state_object_if(ctx, MDDL_TAG_CLEARING_SETTLEMENT);

		/* todo */
		cs->settlementType = NULL;
		break;
	}

	case MDDL_TAG_NAME: {
		/* allow names nearly everywhere */
		mddl_name_t n = NULL;

		switch (get_state_otype(ctx)) {
		case MDDL_TAG_INSTRUMENT_IDENTIFIER: {
			mddl_instrumentIdentifier_t iid = get_state_object(ctx);

			n = mddl_instrumentIdentifier_add_name(iid);
			break;
		}
		case MDDL_TAG_ISSUER_REF: {
			mddl_issuerRef_t iref = get_state_object(ctx);

			n = mddl_issuerRef_add_name(iref);
			break;
		}
		default:
			break;
		}

		if (LIKELY(n != NULL)) {
			mddl_ctxcb_t cc = push_state(ctx, MDDL_TAG_NAME, n);
			cc->cb[0] = __name_cb;
		}
		break;
	}
	case MDDL_TAG_CODE: {
		/* allow codes nearly everywhere */
		mddl_code_t c = NULL;

		switch (get_state_otype(ctx)) {
		case MDDL_TAG_INSTRUMENT_IDENTIFIER: {
			mddl_instrumentIdentifier_t iid = get_state_object(ctx);

			c = mddl_instrumentIdentifier_add_code(iid);
			break;
		}
		case MDDL_TAG_ISSUER_REF: {
			mddl_issuerRef_t iref = get_state_object(ctx);

			c = mddl_issuerRef_add_code(iref);
			break;
		}
		default:
			break;
		}

		if (LIKELY(c != NULL)) {
			mddl_ctxcb_t cc = push_state(ctx, MDDL_TAG_CODE, c);
			cc->cb[0] = __code_cb;
			/* try and find the scheme attr */
			for (int i = 0; attrs[i] != NULL; i += 2) {
				switch (sax_aid_from_attr(attrs[i])) {
				case MDDL_ATTR_SCHEME:
					mddl_code_set_scheme(c, attrs[i + 1]);
					break;
				default:
					break;
				}
			}
		}
		break;
	}
	case MDDL_TAG_CURRENCY: {
		mddl_currency_t ccy;
		mddl_ctxcb_t cc;

		switch (get_state_otype(ctx)) {
		case MDDL_TAG_INSTRUMENT_DATA: {
			mddl_instrumentData_t id = get_state_object(ctx);

			ccy = mddl_instrumentData_add_currency(id);
			break;
		}
		case MDDL_TAG_ISSUE_AMOUNT: {
			mddl_issueAmount_t iamt = get_state_object(ctx);

			ccy = mddl_issueAmount_add_currency(iamt);
			break;
		}
		case MDDL_TAG_ISSUE_FEES: {
			mddl_issueFees_t ifee = get_state_object(ctx);

			ccy = mddl_issueFees_add_currency(ifee);
			break;
		}
		default:
			break;
		}

		if (LIKELY(ccy != NULL &&
			   (cc = push_state(ctx, MDDL_TAG_CURRENCY, ccy)))) {
			cc->cb[0] = __ccy_cb;
			stuff_buf_reset(ctx);
		}
		break;
	}
	/* add up all the tags that need a stuff buf reset */
	case MDDL_TAG_STRING:
	case MDDL_TAG_DATETIME:
	case MDDL_TAG_AMOUNT:
	case MDDL_TAG_PRICE:
	case MDDL_TAG_RATE:
	case MDDL_TAG_ROLE:
	case MDDL_TAG_RANK:
	case MDDL_TAG_INSTRUMENT_TYPE:
	case MDDL_TAG_FUND_STRATEGY_TYPE:
	case MDDL_TAG_DISTRIBUTION_TYPE:
		/* something fundamentally brilliant starts now */
		stuff_buf_reset(ctx);
		break;
	default:
		fprintf(stderr, "WARN: unknown tag %s\n", name);
		break;
	}
	return;
}

static void
sax_eo_elt(mddl_ctx_t ctx, const char *name)
{
	/* where the real element name starts, sans ns prefix */
	const char *rname = tag_massage(name);
	mddl_tid_t tid = sax_tid_from_tag(rname);

	/* check if this is an mddl node */
	if (!mddl_pref_p(ctx, name, rname - name)) {
		/* dont know what to do */
		return;
	}

	/* check for mddl */
	switch (tid) {
	case MDDL_TAG_STRING: {
		size_t len = strlen(ctx->sbuf);
		if (ctx->state->cb->sf) {
			ctx->state->cb->sf(ctx->state, ctx->sbuf, len);
		}
		stuff_buf_reset(ctx);
		break;
	}
	case MDDL_TAG_DATETIME: {
		time_t t = get_zulu(ctx->sbuf);
		if (ctx->state->cb->dtf) {
			ctx->state->cb->dtf(ctx->state, t);
		}
		stuff_buf_reset(ctx);
		break;
	}
	case MDDL_TAG_AMOUNT: {
		double amt = strtod(ctx->sbuf, NULL);
		if (ctx->state->cb->amtf) {
			ctx->state->cb->amtf(ctx->state, amt);
		}
		stuff_buf_reset(ctx);
		break;
	}
	case MDDL_TAG_PRICE: {
		double pri = strtod(ctx->sbuf, NULL);
		if (ctx->state->cb->prif) {
			ctx->state->cb->prif(ctx->state, pri);
		}
		stuff_buf_reset(ctx);
		break;
	}
	case MDDL_TAG_RATE: {
		double rt = strtod(ctx->sbuf, NULL);
		if (ctx->state->cb->rtf) {
			ctx->state->cb->rtf(ctx->state, rt);
		}
		stuff_buf_reset(ctx);
		break;
	}
	case MDDL_TAG_ROLE: {
		switch (get_state_otype(ctx)) {
		case MDDL_TAG_NAME: {
			mddl_name_t n = get_state_object(ctx);
			mddl_role_t r = mddl_name_add_role(n);
			mddl_role_set_Enumeration(r, ctx->sbuf);
		}
		default:
			break;
		}
		stuff_buf_reset(ctx);
		break;
	}
	case MDDL_TAG_RANK: {
		switch (get_state_otype(ctx)) {
		case MDDL_TAG_NAME: {
			mddl_name_t n = get_state_object(ctx);
			mddl_rank_t rk = mddl_name_add_rank(n);
			long int v = strtol(ctx->sbuf, NULL, 10);
			mddl_rank_set_Simple(rk, (mddl_mdDecimal_t)v);
			break;
		}
		case MDDL_TAG_CODE: {
			mddl_code_t c = get_state_object(ctx);
			mddl_rank_t rk = mddl_code_add_rank(c);
			long int v = strtol(ctx->sbuf, NULL, 10);
			mddl_rank_set_Simple(rk, (mddl_mdDecimal_t)v);
			break;
		}
		default:
			break;
		}
		stuff_buf_reset(ctx);
		break;
	}
	case MDDL_TAG_INSTRUMENT_TYPE: {
		mddl_instrumentData_t id =
			get_state_object_if(ctx, MDDL_TAG_INSTRUMENT_DATA);

		if (LIKELY(id != NULL)) {
			mddl_instrumentType_t it =
				mddl_instrumentData_add_instrumentType(id);
			mddl_instrumentType_set_Enumeration(it, ctx->sbuf);
		}
		break;
	}
	case MDDL_TAG_CURRENCY: {
		mddl_currency_t ccy =
			get_state_object_if(ctx, MDDL_TAG_CURRENCY);

		if (LIKELY(ccy != NULL && ccy->Enumeration == NULL)) {
			ccy_ass_s(ctx->state, ctx->sbuf, ctx->sbsz);
		}
		break;
	}
	case MDDL_TAG_FUND_STRATEGY_TYPE: {
		mddl_instrumentDomain_t id =
			get_state_object_if(ctx, MDDL_TAG_INSTRUMENT_DOMAIN);

		if (LIKELY(id != NULL)) {
			mddl_fundStrategyType_t fst =
				mddl_instrumentDomain_add_fundStrategyType(id);
			mddl_fundStrategyType_set_Enumeration(fst, ctx->sbuf);
		}
		break;
	}
	case MDDL_TAG_DISTRIBUTION_TYPE: {
		mddl_instrumentDomain_t id =
			get_state_object_if(ctx, MDDL_TAG_INSTRUMENT_DOMAIN);

		if (LIKELY(id != NULL)) {
			mddl_distributionType_t dt =
				mddl_instrumentDomain_add_distributionType(id);
			mddl_distributionType_set_Enumeration(dt, ctx->sbuf);
		}
		break;
	}
	case MDDL_TAG_OBJECTIVE: {
		mddl_objective_t obj =
			get_state_object_if(ctx, MDDL_TAG_OBJECTIVE);

		if (LIKELY(obj != NULL && obj->Simple == NULL)) {
			obj_ass_s(ctx->state, ctx->sbuf, ctx->sbsz);
		}
		break;
	}
	case MDDL_TAG_SNAP: {
		break;
	}
	default:
		/* stuff buf reset */
		stuff_buf_reset(ctx);
		break;
	}

	/* restore old handler */
	if (LIKELY(tid == get_state_otype(ctx))) {
		pop_state(ctx);
	}
	return;
}

static void
stuff_buf_push(mddl_ctx_t ctx, const char *ch, int len)
{
	if (UNLIKELY(ctx->sbix + len >= ctx->sbsz)) {
		size_t new_sz = ctx->sbix + len;

		/* round to multiple of 4096 */
		new_sz = (new_sz & ~0xfff) + 4096L;
		/* realloc now */
		ctx->sbuf = realloc(ctx->sbuf, ctx->sbsz = new_sz);
	}
	/* now copy */
	memcpy(ctx->sbuf + ctx->sbix, ch, len);
	ctx->sbuf[ctx->sbix += len] = '\0';
	return;
}

static xmlEntityPtr
sax_get_ent(void *UNUSED(user_data), const xmlChar *name)
{
	return xmlGetPredefinedEntity(name);
}

static int
parse_doc(mddl_ctx_t ctx, const char *file)
{
	int res;

	/* fill in the minimalistic sax handler to begin with */
	ctx->hdl->startElement = (startElementSAXFunc)sax_bo_elt;
	ctx->hdl->endElement = (endElementSAXFunc)sax_eo_elt;
	ctx->hdl->characters = (charactersSAXFunc)stuff_buf_push;
	ctx->hdl->getEntity = sax_get_ent;

	res = xmlSAXUserParseFile(ctx->hdl, ctx, file);
	return res;
}


static void
init(mddl_ctx_t ctx)
{
	/* total wipeout */
	memset(ctx, 0, sizeof(*ctx));
	/* initialise the stuff buffer */
	ctx->sbuf = malloc(ctx->sbsz = INITIAL_STUFF_BUF_SIZE);
	/* we always reserve one name space slot for mddl */
	ctx->nns = 1;
	return;
}

static void
deinit(mddl_ctx_t ctx)
{
	for (size_t i = 0; i < ctx->nns; i++) {
		if (ctx->ns[i].pref) {
			free(ctx->ns[i].pref);
		}
		ctx->ns[i].pref = NULL;
		if (ctx->ns[i].href) {
			free(ctx->ns[i].href);
		}
		ctx->ns[i].href = NULL;
	}
	ctx->nns = 1;
	return;
}

static char *package_name __attribute__((unused));


int
main(int argc, char *argv[])
{
	struct gengetopt_args_info argi[1] = {{0}};
	struct mddl_ctx_s ctx[1];

	/* parse our command line, using gengetopt */
	if (cmdline_parser(argc, argv, argi) != 0) {
		return 1;
	}

	init(ctx);
	for (unsigned int i = 0; i < argi->inputs_num; i++) {
		parse_doc(ctx, argi->inputs[i]);
		mddl_print(stdout, ctx->doc);
	}
	deinit(ctx);
	return 0;
}

/* mddl-code.c ends here */
