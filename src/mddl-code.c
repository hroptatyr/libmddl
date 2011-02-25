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
#include "mddl-3.0-beta-tag.c"
#include "mddl-3.0-beta-attr.c"

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
	void(*df)(mddl_ctxcb_t ctx, double decimal);
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
		cc->otype = MDDL_TAG_mddl;
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
bs_ass_s(mddl_ctxcb_t ctx, const char *str, size_t len)
{
	mddl_benchmarkStrategy_t bs = ctx->object;
	mddl_benchmarkStrategy_set_Simple(bs, str);
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

static struct __ctxcb_s __bs_cb = {
	.sf = bs_ass_s,
};

static struct __ctxcb_s __issdate_cb = {
	.dtf = issdate_ass_dt,
};

static struct __ctxcb_s __issamt_cb = {
	.df = iamt_ass_pri,
};

static struct __ctxcb_s __issfee_cb = {
	.df = ifee_ass_pri,
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
	if (UNLIKELY(tid == MDDL_TAG_mddl)) {
		mddl_init(ctx, attrs);
		return;
	}

	if (!mddl_pref_p(ctx, name, rname - name)) {
		/* dont know what to do */
		return;
	}

	/* all the stuff that needs a new sax handler */
	switch (tid) {
	case MDDL_TAG_header: {
		/* check that we're inside an mddl context */
		mddl_doc_t m =
			get_state_object_if(ctx, MDDL_TAG_mddl);
		mddl_ctxcb_t cc;

		if (m && (cc = push_state(ctx, MDDL_TAG_header, m->header))) {
			cc->cb[0] = __hdr_cb;
		}
		break;
	}
	case MDDL_TAG_source: {
		/* check that we're in a header context */
		mddl_header_t hdr =
			get_state_object_if(ctx, MDDL_TAG_header);
		mddl_ctxcb_t cc;

		if (hdr &&
		    (cc = push_state(ctx, MDDL_TAG_source, hdr->source))) {
			cc->cb[0] = __src_cb;
		}
		break;
	}
	case MDDL_TAG_snap: {
		/* check that we're in an mddl context */
		mddl_doc_t m = get_state_object_if(ctx, MDDL_TAG_mddl);
		mddl_snap_t s;

		if (m && (s = mddl_mddl_add_snap(m))) {
			push_state(ctx, MDDL_TAG_snap, s);
		}
		break;
	}
	case MDDL_TAG_instrumentDomain: {
		/* check that we're in a snap context */
		mddl_snap_t m = get_state_object_if(ctx, MDDL_TAG_snap);
		mddl_instrumentDomain_t insdom;

		if (m && (insdom = mddl_snap_add_instrumentDomain(m))) {
			push_state(ctx, MDDL_TAG_instrumentDomain, insdom);
		}
		break;
	}
	case MDDL_TAG_instrumentIdentifier: {
		/* check that we're in a insdom context */
		mddl_instrumentDomain_t insdom =
			get_state_object_if(ctx, MDDL_TAG_instrumentDomain);
		mddl_instrumentIdentifier_t iid;

		if (insdom &&
		    (iid = mddl_instrumentDomain_add\
_instrumentIdentifier(insdom))) {
			push_state(ctx, MDDL_TAG_instrumentIdentifier, iid);
		}
		break;
	}
	case MDDL_TAG_issueData: {
		/* check that we're in a insdom context */
		mddl_instrumentDomain_t insdom =
			get_state_object_if(ctx, MDDL_TAG_instrumentDomain);
		mddl_issueData_t id;

		if (insdom &&
		    (id = mddl_instrumentDomain_add_issueData(insdom))) {
			push_state(ctx, MDDL_TAG_issueData, id);
		}
		break;
	}
	case MDDL_TAG_objective: {
		/* check that we're in an insdom context */
		mddl_instrumentDomain_t insdom =
			get_state_object_if(ctx, MDDL_TAG_instrumentDomain);
		mddl_objective_t objctv;
		mddl_ctxcb_t cc;

		if (insdom &&
		    (objctv = mddl_instrumentDomain_add_objective(insdom)) &&
		    (cc = push_state(ctx, MDDL_TAG_objective, objctv))) {
			cc->cb[0] = __obj_cb;
		}
		stuff_buf_reset(ctx);
		break;
	}
	case MDDL_TAG_benchmarkStrategy: {
		/* check that we're in an insdom context */
		mddl_instrumentDomain_t id =
			get_state_object_if(ctx, MDDL_TAG_instrumentDomain);
		mddl_benchmarkStrategy_t bs;
		mddl_ctxcb_t cc;

		if (id &&
		    (bs = mddl_instrumentDomain_add_benchmarkStrategy(id)) &&
		    (cc = push_state(ctx, MDDL_TAG_benchmarkStrategy, bs))) {
			cc->cb[0] = __bs_cb;
		}
		stuff_buf_reset(ctx);
		break;
	}

/* depth 4 */
	case MDDL_TAG_instrumentData: {
		mddl_instrumentIdentifier_t iid =
			get_state_object_if(
				ctx, MDDL_TAG_instrumentIdentifier);
		mddl_instrumentData_t id;

		if (iid &&
		    (id = mddl_instrumentIdentifier_add_instrumentData(iid))) {
			push_state(ctx, MDDL_TAG_instrumentData, id);
		}
		break;
	}
	case MDDL_TAG_issuerRef: {
		mddl_issueData_t id =
			get_state_object_if(ctx, MDDL_TAG_issueData);
		mddl_issuerRef_t iref;

		if (id && (iref = mddl_issueData_add_issuerRef(id))) {
			push_state(ctx, MDDL_TAG_issuerRef, iref);
		}
		break;
	}
	case MDDL_TAG_issueDate: {
		mddl_issueData_t id =
			get_state_object_if(ctx, MDDL_TAG_issueData);
		mddl_issueDate_t issd;
		mddl_ctxcb_t cc;

		if (id &&
		    (issd = mddl_issueData_add_issueDate(id)) &&
		    (cc = push_state(ctx, MDDL_TAG_issueDate, issd))) {
			cc->cb[0] = __issdate_cb;
		}
		break;
	}
	case MDDL_TAG_issueAmount: {
		mddl_issueData_t id =
			get_state_object_if(ctx, MDDL_TAG_issueData);
		mddl_issueAmount_t iamt;
		mddl_ctxcb_t cc;

		if (id &&
		    (iamt = mddl_issueData_add_issueAmount(id)) &&
		    (cc = push_state(ctx, MDDL_TAG_issueAmount, iamt))) {
			cc->cb[0] = __issamt_cb;
		}
		break;
	}
	case MDDL_TAG_issueFees: {
		mddl_issueData_t id =
			get_state_object_if(ctx, MDDL_TAG_issueData);
		mddl_issueFees_t ifee;
		mddl_ctxcb_t cc;

		if (id &&
		    (ifee = mddl_issueData_add_issueFees(id)) &&
		    (cc = push_state(ctx, MDDL_TAG_issueFees, ifee))) {
			cc->cb[0] = __issfee_cb;
		}
		break;
	}
	case MDDL_TAG_clearingSettlement: {
		mddl_issueData_t id =
			get_state_object_if(ctx, MDDL_TAG_issueData);
		mddl_clearingSettlement_t cs;

		if (id && (cs = mddl_issueData_add_clearingSettlement(id))) {
			push_state(ctx, MDDL_TAG_clearingSettlement, cs);
		}
		break;
	}
	case MDDL_TAG_settlementType: {
		mddl_clearingSettlement_t cs =
			get_state_object_if(ctx, MDDL_TAG_clearingSettlement);

		/* todo */
		cs->settlementType = NULL;
		break;
	}

	case MDDL_TAG_name: {
		/* allow names nearly everywhere */
		mddl_name_t n = NULL;

		switch (get_state_otype(ctx)) {
		case MDDL_TAG_instrumentIdentifier: {
			mddl_instrumentIdentifier_t iid = get_state_object(ctx);

			n = mddl_instrumentIdentifier_add_name(iid);
			break;
		}
		case MDDL_TAG_issuerRef: {
			mddl_issuerRef_t iref = get_state_object(ctx);

			n = mddl_issuerRef_add_name(iref);
			break;
		}
		default:
			break;
		}

		if (LIKELY(n != NULL)) {
			mddl_ctxcb_t cc = push_state(ctx, MDDL_TAG_name, n);
			cc->cb[0] = __name_cb;
		}
		break;
	}
	case MDDL_TAG_code: {
		/* allow codes nearly everywhere */
		mddl_code_t c = NULL;

		switch (get_state_otype(ctx)) {
		case MDDL_TAG_instrumentIdentifier: {
			mddl_instrumentIdentifier_t iid = get_state_object(ctx);

			c = mddl_instrumentIdentifier_add_code(iid);
			break;
		}
		case MDDL_TAG_issuerRef: {
			mddl_issuerRef_t iref = get_state_object(ctx);

			c = mddl_issuerRef_add_code(iref);
			break;
		}
		default:
			break;
		}

		if (LIKELY(c != NULL)) {
			mddl_ctxcb_t cc = push_state(ctx, MDDL_TAG_code, c);
			cc->cb[0] = __code_cb;
			/* try and find the scheme attr */
			for (int i = 0; attrs[i] != NULL; i += 2) {
				switch (sax_aid_from_attr(attrs[i])) {
				case MDDL_ATTR_scheme:
					mddl_code_set_scheme(c, attrs[i + 1]);
					break;
				default:
					break;
				}
			}
		}
		break;
	}
	case MDDL_TAG_currency: {
		mddl_currency_t ccy;
		mddl_ctxcb_t cc;

		switch (get_state_otype(ctx)) {
		case MDDL_TAG_instrumentData: {
			mddl_instrumentData_t id = get_state_object(ctx);

			ccy = mddl_instrumentData_add_currency(id);
			break;
		}
		case MDDL_TAG_issueAmount: {
			mddl_issueAmount_t iamt = get_state_object(ctx);

			ccy = mddl_issueAmount_add_currency(iamt);
			break;
		}
		case MDDL_TAG_issueFees: {
			mddl_issueFees_t ifee = get_state_object(ctx);

			ccy = mddl_issueFees_add_currency(ifee);
			break;
		}
		default:
			break;
		}

		if (LIKELY(ccy != NULL &&
			   (cc = push_state(ctx, MDDL_TAG_currency, ccy)))) {
			cc->cb[0] = __ccy_cb;
			stuff_buf_reset(ctx);
		}
		break;
	}
	/* add up all the tags that need a stuff buf reset */
	case MDDL_TAG_mdString:
	case MDDL_TAG_mdDecimal:
	case MDDL_TAG_mdDateTime:
	case MDDL_TAG_rate:
	case MDDL_TAG_role:
	case MDDL_TAG_rank:
	case MDDL_TAG_instrumentType:
	case MDDL_TAG_fundStrategyType:
	case MDDL_TAG_distributionType:
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
	case MDDL_TAG_mdString: {
		size_t len = strlen(ctx->sbuf);
		if (ctx->state->cb->sf) {
			ctx->state->cb->sf(ctx->state, ctx->sbuf, len);
		}
		stuff_buf_reset(ctx);
		break;
	}
	case MDDL_TAG_mdDateTime: {
		time_t t = get_zulu(ctx->sbuf);
		if (ctx->state->cb->dtf) {
			ctx->state->cb->dtf(ctx->state, t);
		}
		stuff_buf_reset(ctx);
		break;
	}
	case MDDL_TAG_mdDecimal: {
		double amt = strtod(ctx->sbuf, NULL);
		if (ctx->state->cb->df) {
			ctx->state->cb->df(ctx->state, amt);
		}
		stuff_buf_reset(ctx);
		break;
	}
	case MDDL_TAG_role: {
		switch (get_state_otype(ctx)) {
		case MDDL_TAG_name: {
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
	case MDDL_TAG_rank: {
		switch (get_state_otype(ctx)) {
		case MDDL_TAG_name: {
			mddl_name_t n = get_state_object(ctx);
			mddl_rank_t rk = mddl_name_add_rank(n);
			long int v = strtol(ctx->sbuf, NULL, 10);
			mddl_rank_set_Simple(rk, (mddl_mdDecimal_t)v);
			break;
		}
		case MDDL_TAG_code: {
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
	case MDDL_TAG_instrumentType: {
		mddl_instrumentData_t id =
			get_state_object_if(ctx, MDDL_TAG_instrumentData);

		if (LIKELY(id != NULL)) {
			mddl_instrumentType_t it =
				mddl_instrumentData_add_instrumentType(id);
			mddl_instrumentType_set_Enumeration(it, ctx->sbuf);
		}
		break;
	}
	case MDDL_TAG_currency: {
		mddl_currency_t ccy =
			get_state_object_if(ctx, MDDL_TAG_currency);

		if (LIKELY(ccy != NULL && ccy->Enumeration == NULL)) {
			ccy_ass_s(ctx->state, ctx->sbuf, ctx->sbsz);
		}
		break;
	}
	case MDDL_TAG_fundStrategyType: {
		mddl_instrumentDomain_t id =
			get_state_object_if(ctx, MDDL_TAG_instrumentDomain);

		if (LIKELY(id != NULL)) {
			mddl_fundStrategyType_t fst =
				mddl_instrumentDomain_add_fundStrategyType(id);
			mddl_fundStrategyType_set_Enumeration(fst, ctx->sbuf);
		}
		break;
	}
	case MDDL_TAG_distributionType: {
		mddl_instrumentDomain_t id =
			get_state_object_if(ctx, MDDL_TAG_instrumentDomain);

		if (LIKELY(id != NULL)) {
			mddl_distributionType_t dt =
				mddl_instrumentDomain_add_distributionType(id);
			mddl_distributionType_set_Enumeration(dt, ctx->sbuf);
		}
		break;
	}
	case MDDL_TAG_objective: {
		mddl_objective_t obj =
			get_state_object_if(ctx, MDDL_TAG_objective);

		if (LIKELY(obj != NULL && obj->Simple == NULL)) {
			obj_ass_s(ctx->state, ctx->sbuf, ctx->sbsz);
		}
		break;
	}
	case MDDL_TAG_benchmarkStrategy: {
		mddl_benchmarkStrategy_t bs =
			get_state_object_if(ctx, MDDL_TAG_benchmarkStrategy);

		if (LIKELY(bs != NULL && bs->Simple == NULL)) {
			bs_ass_s(ctx->state, ctx->sbuf, ctx->sbsz);
		}
		break;
	}
	case MDDL_TAG_snap: {
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
