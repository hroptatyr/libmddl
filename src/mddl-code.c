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

struct mddl_ns_s {
	char *pref;
	char *href;
};

struct mddl_ctx_s {
	void *doc;
	struct mddl_ns_s ns[16];
	size_t nns;
	/* stuff buf */
#define INITIAL_STUFF_BUF_SIZE	(4096)
	char *sbuf;
	size_t sbsz;
	size_t sbix;
	/* the current sax handler */
	sax_hdl_s hdl[1];
};

#if 0
static const char w3_xsi_uri[] = "http://www.w3.org/2001/XMLSchema-instance";
static const char mddl_xsi_loc[] = "mddl-3.0-beta-full.xsd";
#endif	/* 0 */
static const char mddl_xsi_uri[] = "http://www.mddl.org/mddl/3.0-beta";


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


static const char tag_mddl[] = "mddl";
static const char tag_dt1[] = "dateTime";
static const char tag_dt2[] = "mdDateTime";
static const char tag_obj[] = "objective";
static const char tag_hdr[] = "header";

static void
sax_bo_elt(mddl_ctx_t ctx, const char *name, const char **attrs)
{
	/* where the real element name starts, sans ns prefix */
	const char *rname = tag_massage(name);

	/* check for mddl */
	if (UNLIKELY(strcmp(rname, tag_mddl) == 0)) {
		mddl_init(ctx, attrs);
		return;
	}

	if (!mddl_pref_p(ctx, name, rname - name)) {
		/* dont know what to do */
		return;
	}

	/* add up all the tags that need a stuff buf reset */
	if (strcmp(rname, tag_dt1) == 0 ||
	    strcmp(rname, tag_dt2) == 0 ||
	    strcmp(rname, tag_obj) == 0) {
		/* something fundamentally brilliant starts now */
		stuff_buf_reset(ctx);
	}

	/* all the stuff that needs a new sax handler */
	if (strcmp(rname, tag_hdr) == 0) {
		sax_hdl_t old_hdl = malloc(sizeof(*old_hdl));
		*old_hdl = *ctx->hdl;
		/* store a pointer to the old handler so we can restore it */
		ctx->hdl->_private = old_hdl;
	}
	return;
}

static void
sax_eo_elt(mddl_ctx_t ctx, const char *name)
{
	/* where the real element name starts, sans ns prefix */
	const char *rname = tag_massage(name);

	/* check if this is an mddl node */
	if (!mddl_pref_p(ctx, name, rname - name)) {
		/* dont know what to do */
		return;
	}

	/* check for mddl */
	if (strcmp(rname, tag_mddl) == 0) {
		fputs("doc has finished, YAY\n", stderr);

	} else if (strcmp(rname, tag_dt1) == 0 ||
		   strcmp(rname, tag_dt2) == 0) {
		time_t t = get_zulu(ctx->sbuf);
		fprintf(stderr, "DATETIME: %s gave us %ld\n", ctx->sbuf, t);
		stuff_buf_reset(ctx);

	} else if (strcmp(rname, tag_obj) == 0) {
		fputs("OBJECTIVE\n", stderr);
		fputs(ctx->sbuf, stderr);
		fputc('\n', stderr);
		fputs("/OBJECTIVE\n", stderr);
		stuff_buf_reset(ctx);

	} else if (strcmp(rname, tag_hdr) == 0) {
		/* restore the sax handler */
		sax_hdl_t old_hdl = ctx->hdl->_private;

		*ctx->hdl = *old_hdl;
		free(old_hdl);

	} else {
		/* stuff buf reset */
		stuff_buf_reset(ctx);
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
	ctx->hdl->startElement = (startElementSAXFunc)sax_bo_elt,
	ctx->hdl->endElement = (endElementSAXFunc)sax_eo_elt,
	ctx->hdl->characters = (charactersSAXFunc)stuff_buf_push,
	ctx->hdl->_private = NULL,

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