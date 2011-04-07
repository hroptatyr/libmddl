#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>
#include "mddl.h"
#include "mddl-core.h"

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


/* some more printers we need */
static void
print_attr_ID(
	FILE *out, const char *a,
	xsd_ID_t id, const char *UNUSED(_default), size_t UNUSED(indent))
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
	xsd_integer_t i, const char *UNUSED(_default), size_t UNUSED(i))
{
	fprintf(out, " %s=\"%li\"", a, i);
	return;
}

static void
print_attr_string(
	FILE *out, const char *a,
	xsd_string_t s, const char *UNUSED(_default), size_t UNUSED(i))
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
	xsd_anyURI_t u, const char *UNUSED(_default), size_t UNUSED(i))
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
print_decimal(FILE *out, xsd_decimal_t d, size_t indent)
{
	print_indent(out, indent);
	fputs("<mdDecimal>", out);
	fprintf(out, "%g", d);
	fputs("</mdDecimal>\n", out);
	return;
}
#define print_mdDecimal	print_decimal

static void
print_mdNonNegativeDecimal(
	FILE *out, mddl_NonNegativeDecimal_t d, size_t indent)
{
	if (d.decimal < 0.0) {
		return;
	}
	print_indent(out, indent);
	fputs("<mdNonNegativeDecimal>", out);
	fprintf(out, "%g", d.decimal);
	fputs("</mdNonNegativeDecimal>\n", out);
	return;
}

static void
print_integer(FILE *out, xsd_integer_t i, size_t indent)
{
	print_indent(out, indent);
	fputs("<mdInteger>", out);
	fprintf(out, "%li", i);
	fputs("</mdInteger>\n", out);
	return;
}
#define print_mdInteger		print_integer

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

/* special bollocks for extension elements */
static void
print_AnyDateTime(FILE *out, mddl_AnyDateTime_t dt, size_t UNUSED(indent))
{
	if (__time_null_p(dt.dateTime)) {
		return;
	}
	print_zulu(out, dt.dateTime);
	return;
}

static void
print_mdDateTime(FILE *out, mddl_AnyDateTime_t dt, size_t indent)
{
	if (__time_null_p(dt.dateTime)) {
		return;
	}
	print_indent(out, indent);
	fputs("<mdDateTime>", out);
	print_zulu(out, dt.dateTime);
	fputs("</mdDateTime>\n", out);
	return;
}

static void
print_mdDuration(FILE *out, xsd_duration_t du, size_t indent)
{
	print_indent(out, indent);
	fputs("<mdDuration>no_printer</mdDuration>\n", out);
	return;
}

/* get those auto-gen'd printers */
#include "mddl-3.0-beta-print.c"


void
mddl_cmd_print(mddl_clo_t clo, mddl_doc_t doc)
{
	const size_t indent = 0;
	mddl_mddl_t tree = doc->tree;
	FILE *out = clo->out;

	/* mimicking
	 * print_mddl(out, doc, indent);
	 * as that one cant deal with our name space decls */

	fputs("\
<?xml version=\"1.0\"?>\n\
<mddl", out);

	for (size_t i = 0; i < doc->nns; i++) {
		fputs(" xmlns", out);
		if (UNLIKELY(doc->ns[i].pref != NULL)) {
			fputc(':', out);
			fputs(doc->ns[i].pref, out);
		}
		fputc('=', out);
		fputc('"', out);
		fputs(doc->ns[i].href, out);
		fputc('"', out);
	}
	if (LIKELY(doc->tree->version != NULL)) {
		fputs(" version=\"", out);
		fputs(doc->tree->version, out);
		fputc('"', out);
	}
	/* finish the tag */
	fputs(">\n", out);

        if (!__header_null_p(tree->header)) {
                print_header(out, tree->header, indent + 2);
        }
        for (size_t i = 0; i < tree->nsnap; i++) {
                print_snap(out, tree->snap + i, indent + 2);
        }
        for (size_t i = 0; i < tree->ntimeseries; i++) {
                print_timeseries(out, tree->timeseries + i, indent + 2);
        }

	fputs("</mddl>\n", out);
	return;
}

/* mddl-print.c ends here */
