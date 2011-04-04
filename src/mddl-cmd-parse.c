#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>
#if defined __INTEL_COMPILER
# pragma warning (disable:1292)
#endif	/* __INTEL_COMPILER */
#include <libxml/parser.h>
#if defined __INTEL_COMPILER
# pragma warning (default:1292)
#endif	/* __INTEL_COMPILER */
#include "mddl.h"

/* gperf goodness */
#include "mddl-3.0-beta-tag.c"
#include "mddl-3.0-beta-attr.c"

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

/* contextual callbacks */
struct mddl_ctxcb_s {
	/* for a linked list */
	mddl_ctxcb_t next;

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
		struct mddl_doc_s *m = calloc(sizeof(*m), 1);
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

/* ugly trick */
static char*
__strip_const(const char *v)
{
	union {
		const char *c;
		char *p;
	} u = {
		.c = v,
	};
	return u.p;
}

static xsd_ID_t
attr_to_ID(const char *v)
{
	return __strip_const(v);
}

static xsd_string_t
attr_to_string(const char *v)
{
	return __strip_const(v);
}
#define attr_to_mdString	attr_to_string

static xsd_anyURI_t
attr_to_anyURI(const char *v)
{
	return __strip_const(v);
}

static xsd_integer_t
attr_to_integer(const char *v)
{
	return strtol(v, NULL, 10);
}

static mddl_QualityEnumeration_t
attr_to_QualityEnumeration(const char *UNUSED(v))
{
	return QE_UNK;
}

/* stuff buf handling */
static void
stuff_buf_reset(mddl_ctx_t ctx)
{
	ctx->sbix = 0;
	return;
}

static mddl_mdString_t
stuff_buf_to_mdString(mddl_ctx_t ctx)
{
	ctx->sbuf[ctx->sbix] = '\0';
	return ctx->sbuf;
}

static mddl_mdUri_t
stuff_buf_to_mdUri(mddl_ctx_t ctx)
{
	ctx->sbuf[ctx->sbix] = '\0';
	return ctx->sbuf;
}

static mddl_AnyDateTime_t
stuff_buf_to_mdDateTime(mddl_ctx_t ctx)
{
	ctx->sbuf[ctx->sbix] = '\0';
	return (mddl_AnyDateTime_t)get_zulu(ctx->sbuf);
}
#define stuff_buf_to_AnyDateTime	stuff_buf_to_mdDateTime

static xsd_duration_t
stuff_buf_to_mdDuration(mddl_ctx_t ctx)
{
	ctx->sbuf[ctx->sbix] = '\0';
	return (time_t)-1;
}

static xsd_integer_t
stuff_buf_to_mdInteger(mddl_ctx_t ctx)
{
	ctx->sbuf[ctx->sbix] = '\0';
	return strtol(ctx->sbuf, NULL, 10);
}

static xsd_decimal_t
stuff_buf_to_mdDecimal(mddl_ctx_t ctx)
{
	ctx->sbuf[ctx->sbix] = '\0';
	return strtod(ctx->sbuf, NULL);
}

static mddl_NonNegativeDecimal_t
stuff_buf_to_mdNonNegativeDecimal(mddl_ctx_t ctx)
{
	mddl_NonNegativeDecimal_t res;
	ctx->sbuf[ctx->sbix] = '\0';
	res.decimal = strtod(ctx->sbuf, NULL);
	if (res.decimal < 0.0) {
		res.decimal = 0.0;
	}
	return res;
}

static mddl_mdBoolean_t
stuff_buf_to_mdBoolean(mddl_ctx_t ctx)
{
	return false;
}

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


#include "mddl-3.0-beta-parse.c"

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

	/* call the autogen'd code */
	stuff_buf_reset(ctx);
	parse_bo(ctx, tid, attrs);
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

	/* call the autogen'd code */
	parse_eo(ctx, tid);
	stuff_buf_reset(ctx);

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

mddl_doc_t
mddl_cmd_parse(const char *file)
{
	mddl_doc_t res = NULL;
	struct mddl_ctx_s ctx[1];

	init(ctx);
	/* fill in the minimalistic sax handler to begin with */
	ctx->hdl->startElement = (startElementSAXFunc)sax_bo_elt;
	ctx->hdl->endElement = (endElementSAXFunc)sax_eo_elt;
	ctx->hdl->characters = (charactersSAXFunc)stuff_buf_push;
	ctx->hdl->getEntity = sax_get_ent;

	if (xmlSAXUserParseFile(ctx->hdl, ctx, file) == 0) {
		/* now make way for the name spaces we tracked */
		size_t nns = ctx->nns;
		res = realloc(ctx->doc, sizeof(*res) + nns * sizeof(*res->ns));
		res->nns = nns;
		for (size_t i = 0; i < nns; i++) {
			res->ns[i].pref = ctx->ns[i].pref
				? strdup(ctx->ns[i].pref) : NULL;
			res->ns[i].href = strdup(ctx->ns[i].href);
		}
	}
	deinit(ctx);
	return res;
}

/* mddl-code.c ends here */
