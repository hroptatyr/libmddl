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
		struct __e_mddl_s *m = calloc(sizeof(*m), 1);
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

static void
print_indent(size_t indent)
{
	for (size_t i = 0; i < indent; i++) {
		fputc(' ', stderr);
	}
	return;
}

static void
print_name(mddl_p_name_t name, size_t indent)
{
	print_indent(indent);
	fprintf(stderr, "name \"%s\"\n", name->value);
	for (size_t i = 0; i < name->nrole; i++) {
		print_indent(indent);
		fprintf(stderr, "  role %zu %s\n", i, name->role[i]);
	}
	for (size_t i = 0; i < name->nrank; i++) {
		print_indent(indent);
		fprintf(stderr, "  rank %zu %i\n", i, name->rank[i]);
	}
	return;
}

static void
print_code(mddl_p_code_t code, size_t indent)
{
	print_indent(indent);
	fprintf(stderr, "code %s \"%s\"\n", code->scheme, code->value);
	for (size_t i = 0; i < code->nrank; i++) {
		print_indent(indent);
		fprintf(stderr, "  rank %zu %i\n", i, code->rank[i]);
	}
	return;
}

static void
print_instr_data(mddl_p_instr_data_t id, size_t indent)
{
	print_indent(indent);
	fprintf(stderr, "instr data\n");
	for (size_t i = 0; i < id->ninstr_type; i++) {
		__a_instr_type_t t = id->instr_type[i];
		print_indent(indent);
		fprintf(stderr, "  type %s\n", t);
	}
	return;
}

static void
print_instr_ident(mddl_p_instr_ident_t iid, size_t indent)
{
	fprintf(stderr, "  %zu code/names\n", iid->ncode_name);
	for (size_t i = 0; i < iid->ncode_name; i++) {
		struct __g_code_name_s *cn = iid->code_name + i;
		fprintf(stderr, "  type %u %zu %p\n",
			cn->code_name_gt, cn->ncode_name, cn->ptr);
		switch (cn->code_name_gt) {
		case MDDL_CODE_NAME_NAME:
			for (size_t j = 0; j < cn->ncode_name; j++) {
				mddl_p_name_t n = cn->name + j;
				print_name(n, indent + 4);
			}
			break;
		case MDDL_CODE_NAME_CODE:
			for (size_t j = 0; j < cn->ncode_name; j++) {
				mddl_p_code_t c = cn->code + j;
				print_code(c, indent + 4);
			}
			break;
		default:
			break;
		}
	}
	for (size_t j = 0; j < iid->ninstr_data; j++) {
		print_instr_data(iid->instr_data + j, indent + 2);
	}
	return;
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
	struct __e_hdr_s *hdr = ctx->object;
	hdr->stamp = dt;
	return;
}

static void
src_ass_s(mddl_ctxcb_t ctx, const char *str, size_t len)
{
	struct __p_src_s *src = ctx->object;
	src->value = strndup(str, len);
	return;
}

static void
name_ass_s(mddl_ctxcb_t ctx, const char *str, size_t len)
{
	mddl_p_name_t n = ctx->object;
	n->value = strndup(str, len);
	return;
}

static void
code_ass_s(mddl_ctxcb_t ctx, const char *str, size_t len)
{
	mddl_p_code_t c = ctx->object;
	c->value = strndup(str, len);
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
		struct __e_mddl_s *m =
			get_state_object_if(ctx, MDDL_TAG_MDDL);
		mddl_ctxcb_t cc;

		if (m && (cc = push_state(ctx, MDDL_TAG_HEADER, m->hdr))) {
			cc->cb[0] = __hdr_cb;
		}
		break;
	}
	case MDDL_TAG_SOURCE: {
		/* check that we're in a header context */
		struct __e_hdr_s *hdr =
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

		if (m && (s = mddl_add_snap(m))) {
			push_state(ctx, MDDL_TAG_SNAP, s);
		}
		break;
	}
	case MDDL_TAG_INSTRUMENT_DOMAIN: {
		/* check that we're in a snap context */
		mddl_snap_t m = get_state_object_if(ctx, MDDL_TAG_SNAP);
		struct __dom_instr_s *insdom;

		if (m && (insdom = mddl_snap_add_dom_instr(m))) {
			push_state(ctx, MDDL_TAG_INSTRUMENT_DOMAIN, insdom);
		}
		break;
	}
	case MDDL_TAG_INSTRUMENT_IDENTIFIER: {
		/* check that we're in a insdom context */
		mddl_dom_instr_t insdom =
			get_state_object_if(ctx, MDDL_TAG_INSTRUMENT_DOMAIN);
		mddl_p_instr_ident_t iid;

		if (insdom && (iid = mddl_dom_instr_add_instr_ident(insdom))) {
			push_state(ctx, MDDL_TAG_INSTRUMENT_IDENTIFIER, iid);
		}
		break;
	}
	case MDDL_TAG_INSTRUMENT_DATA: {
		mddl_p_instr_ident_t iid =
			get_state_object_if(
				ctx, MDDL_TAG_INSTRUMENT_IDENTIFIER);
		mddl_p_instr_data_t id;

		if (iid && (id = mddl_instr_ident_add_instr_data(iid))) {
			push_state(ctx, MDDL_TAG_INSTRUMENT_DATA, id);
		}
		break;
	}
	case MDDL_TAG_NAME: {
		/* allow names nearly everywhere */
		mddl_p_name_t n = NULL;

		switch (get_state_otype(ctx)) {
		case MDDL_TAG_INSTRUMENT_IDENTIFIER: {
			struct __p_instr_ident_s *iid = get_state_object(ctx);

			n = mddl_instr_ident_add_name(iid);
			break;
		}
		case MDDL_TAG_ISSUER_REF: {
			struct __p_issuer_ref_s *iref = get_state_object(ctx);
			iref->code_name = NULL;
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
		mddl_p_code_t c = NULL;

		switch (get_state_otype(ctx)) {
		case MDDL_TAG_INSTRUMENT_IDENTIFIER: {
			struct __p_instr_ident_s *iid = get_state_object(ctx);

			c = mddl_instr_ident_add_code(iid);
			break;
		}
		case MDDL_TAG_ISSUER_REF:
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
					mddl_code_add_scheme(c, attrs[i + 1]);
					break;
				default:
					break;
				}
			}
		}
		break;
	}
	/* add up all the tags that need a stuff buf reset */
	case MDDL_TAG_STRING:
	case MDDL_TAG_DATETIME:
	case MDDL_TAG_ROLE:
	case MDDL_TAG_RANK:
	case MDDL_TAG_INSTRUMENT_TYPE:
	default:
		/* something fundamentally brilliant starts now */
		stuff_buf_reset(ctx);
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
	case MDDL_TAG_MDDL: {
		struct __e_mddl_s *mddl = get_state_object(ctx);
		fputs("mddl popped\n", stderr);
		for (size_t i = 0; i < 1; i++) {
			struct __g_mddl_choi_s *choi = mddl->choice + i;
			fprintf(stderr, "  type %u %zu %p\n",
				choi->mddl_choi_gt,
				choi->nmddl_choi,
				choi->ptr);
		}
		break;
	}
	case MDDL_TAG_STRING: {
		size_t len = strlen(ctx->sbuf);
		fprintf(stderr, "STRING: \"%s\"\n", ctx->sbuf);
		if (ctx->state->cb->sf) {
			ctx->state->cb->sf(ctx->state, ctx->sbuf, len);
		}
		stuff_buf_reset(ctx);
		break;
	}
	case MDDL_TAG_DATETIME: {
		time_t t = get_zulu(ctx->sbuf);
		fprintf(stderr, "DATETIME: %s gave us %ld\n", ctx->sbuf, t);
		if (ctx->state->cb->dtf) {
			ctx->state->cb->dtf(ctx->state, t);
		}
		stuff_buf_reset(ctx);
		break;
	}
	case MDDL_TAG_ROLE: {
		switch (get_state_otype(ctx)) {
		case MDDL_TAG_NAME: {
			mddl_p_name_t n = get_state_object(ctx);
			mddl_name_add_role(n, ctx->sbuf);
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
			mddl_p_name_t n = get_state_object(ctx);
			long int rk = strtol(ctx->sbuf, NULL, 10);
			mddl_name_add_rank(n, rk);
			break;
		}
		case MDDL_TAG_CODE: {
			mddl_p_code_t c = get_state_object(ctx);
			long int rk = strtol(ctx->sbuf, NULL, 10);
			mddl_code_add_rank(c, rk);
			break;
		}
		default:
			break;
		}
		stuff_buf_reset(ctx);
		break;
	}
	case MDDL_TAG_INSTRUMENT_TYPE: {
		mddl_p_instr_data_t id =
			get_state_object_if(ctx, MDDL_TAG_INSTRUMENT_DATA);

		if (LIKELY(id != NULL)) {
			mddl_instr_data_add_instr_type(id, ctx->sbuf);
		}
		break;
	}
	case MDDL_TAG_OBJECTIVE: {
		fputs("OBJECTIVE\n", stderr);
		fputs(ctx->sbuf, stderr);
		fputc('\n', stderr);
		fputs("/OBJECTIVE\n", stderr);
		stuff_buf_reset(ctx);
		break;
	}
	case MDDL_TAG_HEADER: {
		struct __e_hdr_s *hdr = get_state_object(ctx);
		fputs("HEADER", stderr);
		fprintf(stderr, " %p", hdr);
		if (hdr->stamp > 0) {
			fprintf(stderr, " .stamp = %ld", hdr->stamp);
		}
		if (hdr->source->value) {
			fprintf(stderr, " .source = %s", hdr->source->value);
		}
		fputc('\n', stderr);
		break;
	}
	case MDDL_TAG_SOURCE: {
		break;
	}
	case MDDL_TAG_SNAP: {
		struct __e_snap_s *snap = get_state_object(ctx);
		fputs("snap popped\n", stderr);
		fprintf(stderr, "%zu domains\n", snap->choice->nsnap_choi);
		for (size_t i = 0; i < snap->choice->nsnap_choi; i++) {
			struct __g_domains_s *dom = snap->choice->domains + i;
			fprintf(stderr, "  type %u %zu %p\n",
				dom->domains_gt, dom->ndomains, dom->ptr);
		}
		break;
	}
	case MDDL_TAG_INSTRUMENT_DOMAIN: {
		fputs("instrumendDomain popped\n", stderr);
		break;
	}
	case MDDL_TAG_INSTRUMENT_IDENTIFIER: {
		struct __p_instr_ident_s *iid = get_state_object(ctx);
		fputs("instrumentIdentifier popped\n", stderr);
		print_instr_ident(iid, 0);
		break;
	}
	case MDDL_TAG_INSTRUMENT_DATA: {
		fputs("instrumentData popped\n", stderr);
		break;
	}
	case MDDL_TAG_NAME: {
		if (get_state_otype(ctx) == MDDL_TAG_NAME) {
			fputs("name popped\n", stderr);
		}
		break;
	}
	case MDDL_TAG_CODE: {
		if (get_state_otype(ctx) == MDDL_TAG_CODE) {
			fputs("code popped\n", stderr);
		}
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
	ctx->sbuf[ctx->sbix = len] = '\0';
	return;
}

static int
parse_doc(mddl_ctx_t ctx, const char *file)
{
	int res;

	/* fill in the minimalistic sax handler to begin with */
	ctx->hdl->startElement = (startElementSAXFunc)sax_bo_elt;
	ctx->hdl->endElement = (endElementSAXFunc)sax_eo_elt;
	ctx->hdl->characters = (charactersSAXFunc)stuff_buf_push;

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
	}
	deinit(ctx);
	return 0;
}

/* mddl-code.c ends here */
