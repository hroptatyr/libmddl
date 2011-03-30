#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>
#include "mddl.h"

#if defined __INTEL_COMPILER
# pragma warning (disable:869)
# pragma warning (disable:177)
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


static void
print_indent(FILE *out, size_t indent)
{
	for (size_t i = 0; i < indent; i++) {
		fputc(' ', out);
	}
	return;
}

static void
print_zulu(FILE *out, time_t stamp)
{
	struct tm tm[1] = {{0}};
	char buf[32];
	gmtime_r(&stamp, tm);
	strftime(buf, sizeof(buf), "%FT%T%z", tm);
	fputs(buf, out);
	return;
}

static void
print_date(FILE *out, time_t stamp)
{
	struct tm tm[1] = {{0}};
	char buf[32];
	gmtime_r(&stamp, tm);
	strftime(buf, sizeof(buf), "%F", tm);
	fputs(buf, out);
	return;
}

static void
fputs_enc(const char *s, FILE *out)
{
/* like fputs() but encode special chars */
	static const char stpset[] = "<>&";

	for (size_t idx; idx = strcspn(s, stpset); s += idx + sizeof(*s)) {
		/* write what we've got */
		fwrite(s, sizeof(*s), idx, out);
		/* inspect the character */
		switch (s[idx]) {
		default:
		case '\0':
			return;
		case '<':
			fputs("&lt;", out);
			break;
		case '>':
			fputs("&gt;", out);
			break;
		case '&':
			fputs("&amp;", out);
			break;
		}
	}
	return;
}

static void
fputs_encq(const char *s, FILE *out)
{
/* like fputs() but encode special chars */
	static const char stpset[] = "<>&'\"";

	for (size_t idx; idx = strcspn(s, stpset); s += idx + sizeof(*s)) {
		/* write what we've got */
		fwrite(s, sizeof(*s), idx, out);
		/* inspect the character */
		switch (s[idx]) {
		default:
		case '\0':
			return;
		case '<':
			fputs("&lt;", out);
			break;
		case '>':
			fputs("&gt;", out);
			break;
		case '&':
			fputs("&amp;", out);
			break;
		case '\'':
			fputs("&apos;", out);
			break;
		case '"':
			fputs("&quot;", out);
			break;
		}
	}
	return;
}


/* some predicates */
static bool
__time_null_p(time_t stamp)
{
	return stamp == 0 || stamp == -1;
}

static bool
__dateTime_null_p(mddl_dateTime_t dt)
{
	return __time_null_p(dt->AnyDateTime);
}

static bool
__snap_null_p(mddl_snap_t snap)
{
	return snap->ndateTime == 0 &&
		snap->ncaeDomain == 0 &&
		snap->ncashDomain == 0 &&
		snap->ncommodityDomain == 0 &&
		snap->nentityDomain == 0 &&
		snap->nforeignExchangeDomain == 0 &&
		snap->nindexDomain == 0 &&
		snap->nindicatorDomain == 0 &&
		snap->ninstrumentDomain == 0 &&
		snap->nportfolioDomain == 0 &&
		snap->nsnapType == 0;
}

static bool
__query_null_p(mddl_query_t q)
{
	return true;
}

static bool
__header_null_p(mddl_header_t h)
{
	return __dateTime_null_p(h->dateTime) &&
		__query_null_p(h->query) &&
		h->nschemeInfo == 0;
}

static bool
__mddlQuerySource_null_p(mddl_mddlQuerySource_t qs)
{
	return true;
}

static bool
__queryReference_null_p(mddl_queryReference_t qr)
{
	return true;
}

static bool
__queryStatusType_null_p(mddl_queryStatusType_t qst)
{
	return true;
}

static bool
__source_null_p(mddl_source_t src)
{
	return src->Simple == NULL;
}

static bool
__sequence_null_p(mddl_sequence_t seq)
{
	return seq->Simple == NULL;
}

/* some more printers we need */
static void
print_attr_ID(
	FILE *out, const char *a,
	mddl_ID_t id, const char *UNUSED(_default), size_t UNUSED(indent))
{
	if (id == NULL) {
		return;
	}
	fputc(' ', out);
	fputs(a, out);
	fputc('=', out);
	fputc('"', out);
	fputs_encq(id, out);
	fputc('"', out);
	return;
}

static void
print_attr_integer(
	FILE *out, const char *a,
	mddl_integer_t i, const char *UNUSED(_default), size_t UNUSED(i))
{
	fprintf(out, " %s=\"%li\"", a, i);
	return;
}

static void
print_attr_string(
	FILE *out, const char *a,
	mddl_string_t s, const char *UNUSED(_default), size_t UNUSED(i))
{
	if (s == NULL) {
		return;
	}
	fputc(' ', out);
	fputs(a, out);
	fputc('=', out);
	fputc('"', out);
	fputs_encq(s, out);
	fputc('"', out);
	return;
}

static void
print_attr_QualityEnumeration(
	FILE *out, const char *a,
	mddl_QualityEnumeration_t qe, const char *UNUSED(_default),
	size_t UNUSED(indent))
{
	switch (qe) {
	default:
	case QE_UNK:
		break;
	case QE_VALID:
		fputs(" validity=\"valid\"", out);
		break;
	case QE_UNAVAILABLE:
		fputs(" validity=\"unavailable\"", out);
		break;
	}
	return;
}

static void
print_attr_mdString(
	FILE *out, const char *a,
	mddl_mdString_t s, const char *UNUSED(_default), size_t UNUSED(indent))
{
	if (s == NULL) {
		return;
	}
	fputc(' ', out);
	fputs(a, out);
	fputc('=', out);
	fputc('"', out);
	fputs_encq(s, out);
	fputc('"', out);
	return;
}

static void
print_attr_anyURI(
	FILE *out, const char *a,
	mddl_anyURI_t u, const char *UNUSED(_default), size_t UNUSED(i))
{
	if (u == NULL) {
		return;
	}
	fputc(' ', out);
	fputs(a, out);
	fputc('=', out);
	fputc('"', out);
	fputs_encq(u, out);
	fputc('"', out);
	return;
}

static void
print_mdString(FILE *out, mddl_mdString_t s, size_t indent)
{
	if (s == NULL) {
		return;
	}
	print_indent(out, indent);
	fputs("<mdString>", out);
	fputs_enc(s, out);
	fputs("</mdString>\n", out);
	return;
}

static void
print_mdUri(FILE *out, mddl_mdUri_t u, size_t indent)
{
	if (u == NULL) {
		return;
	}
	print_indent(out, indent);
	fputs("<mdUri>", out);
	fputs_enc(u, out);
	fputs("</mdUri>\n", out);
	return;
}

static void
print_mdDecimal(FILE *out, mddl_mdDecimal_t d, size_t indent)
{
	print_indent(out, indent);
	fputs("<mdDecimal>", out);
	fprintf(out, "%g", d);
	fputs("</mdDecimal>\n", out);
	return;
}

static void
print_mdNonNegativeDecimal(
	FILE *out, mddl_mdNonNegativeDecimal_t d, size_t indent)
{
	if (d < 0.0) {
		return;
	}
	print_indent(out, indent);
	fputs("<mdNonNegativeDecimal>", out);
	fprintf(out, "%g", d);
	fputs("</mdNonNegativeDecimal>\n", out);
	return;
}

static void
print_mdInteger(FILE *out, mddl_mdInteger_t i, size_t indent)
{
	print_indent(out, indent);
	fputs("<mdInteger>", out);
	fprintf(out, "%li", i);
	fputs("</mdInteger>\n", out);
	return;
}

static void
print_mdBoolean(FILE *out, mddl_mdBoolean_t b, size_t indent)
{
	print_indent(out, indent);
	if (b) {
		fputs("<mdBoolean>true</mdBoolean>\n", out);
	} else {
		fputs("<mdBoolean>false</mdBoolean>\n", out);
	}
	return;
}

static void
print_mdDateTime(FILE *out, mddl_mdDateTime_t dt, size_t indent)
{
	if (__time_null_p(dt)) {
		return;
	}
	print_indent(out, indent);
	fputs("<mdDateTime>", out);
	print_zulu(out, dt);
	fputs("</mdDateTime>\n", out);
	return;
}

static void
print_AnyDateTime(FILE *out, mddl_AnyDateTime_t dt, size_t indent)
{
	print_indent(out, indent);
	print_zulu(out, dt);
	fputs("\n", out);
	return;
}

static void
print_mdDuration(FILE *out, mddl_mdDuration_t du, size_t indent)
{
	print_indent(out, indent);
	fputs("<mdDuration>no_printer</mdDuration>\n", out);
	return;
}

/* get those auto-gen'd printers */
#include "mddl-3.0-beta-print.c"


void
mddl_cmd_print(void *out, mddl_doc_t doc)
{
	const size_t indent = 0;

	/* mimicking
	 * print_mddl(out, doc, indent);
	 * as that one cant deal with our name space decls */

	fputs("\
<?xml version=\"1.0\"?>\n\
<mddl xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n\
  xmlns=\"http://www.mddl.org/mddl/3.0-beta\"\n\
  version=\"3.0-beta\"\n\
  xsi:schemaLocation=\
\"http://www.mddl.org/mddl/3.0-beta mddl-3.0-beta-full.xsd\">\n", out);

        if (!__header_null_p(doc->header)) {
                print_header(out, doc->header, indent + 2);
        }
        for (size_t i = 0; i < doc->nsnap; i++) {
                print_snap(out, doc->snap + i, indent + 2);
        }
        for (size_t i = 0; i < doc->ntimeseries; i++) {
                print_timeseries(out, doc->timeseries + i, indent + 2);
        }

	fputs("</mddl>\n", out);
	return;
}

/* mddl-print.c ends here */
