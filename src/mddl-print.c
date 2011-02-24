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
print_zulu(FILE *out, mddl_mdDateTime_t stamp)
{
	struct tm tm[1] = {{0}};
	char buf[32];
	gmtime_r(&stamp, tm);
	strftime(buf, sizeof(buf), "%FT%T%z", tm);
	fputs(buf, out);
	return;
}

static void
print_date(FILE *out, mddl_mdDateTime_t stamp)
{
	struct tm tm[1] = {{0}};
	char buf[32];
	gmtime_r(&stamp, tm);
	strftime(buf, sizeof(buf), "%F", tm);
	fputs(buf, out);
	return;
}

static void
print_name(FILE *out, mddl_name_t name, size_t indent)
{
	print_indent(out, indent);
	fputs("<name>\n", out);

	print_indent(out, indent + 2);
	fputs("<mdString>", out);
	fputs(name->Simple, out);
	fputs("</mdString>\n", out);

	for (size_t i = 0; i < name->nrole; i++) {
		print_indent(out, indent + 2);
		fputs("<role>", out);
		fputs(name->role[i].Enumeration, out);
		fputs("</role>\n", out);
	}
	for (size_t i = 0; i < name->nrank; i++) {
		print_indent(out, indent + 2);
		fprintf(out, "<rank>%2.0f</rank>\n", name->rank[i].Simple);
	}

	print_indent(out, indent);
	fputs("</name>\n", out);
	return;
}

static void
print_code(FILE *out, mddl_code_t code, size_t indent)
{
	print_indent(out, indent);
	fprintf(out, "<code scheme=\"%s\">\n", code->scheme);

	print_indent(out, indent + 2);
	fputs("<mdString>", out);
	fputs(code->Enumeration, out);
	fputs("</mdString>\n", out);
	for (size_t i = 0; i < code->nrank; i++) {
		print_indent(out, indent + 2);
		fprintf(out, "<rank>%2.0f</rank>\n", code->rank[i].Simple);
	}

	print_indent(out, indent);
	fputs("</code>\n", out);
	return;
}

static void
print_currency(FILE *out, mddl_currency_t ccy, size_t indent)
{
	print_indent(out, indent);
	fputs("<currency>", out);
	fputs(ccy->Enumeration, out);
	fputs("</currency>\n", out);
	return;
}

static void
print_instr_type(FILE *out, mddl_instrumentType_t type, size_t indent)
{
	print_indent(out, indent);
	fputs("<instrumentType>", out);
	fputs(type->Enumeration, out);
	fputs("</instrumentType>\n", out);
	return;
}

static void
print_instr_data(FILE *out, mddl_instrumentData_t id, size_t indent)
{
	print_indent(out, indent);
	fputs("<instrumentData>\n", out);

	for (size_t i = 0; i < id->ninstrumentType; i++) {
		mddl_instrumentType_t t = id->instrumentType + i;
		print_instr_type(out, t, indent + 2);
	}
	for (size_t i = 0; i < id->ncurrency; i++) {
		mddl_currency_t ccy = id->currency + i;
		print_currency(out, ccy, indent + 2);
	}

	print_indent(out, indent);
	fputs("</instrumentData>\n", out);
	return;
}

static void
print_indus_ident(FILE *out, mddl_industryIdentifier_t iid, size_t indent)
{
	return;
}

static void
print_instr_ident(FILE *out, mddl_instrumentIdentifier_t iid, size_t indent)
{
	print_indent(out, indent);
	fputs("<instrumentIdentifier>\n", out);

	for (size_t j = 0; j < iid->nname; j++) {
		mddl_name_t n = iid->name + j;
		print_name(out, n, indent + 2);
	}
	for (size_t j = 0; j < iid->ncode; j++) {
		mddl_code_t c = iid->code + j;
		print_code(out, c, indent + 2);
	}
	for (size_t j = 0; j < iid->ninstrumentData; j++) {
		print_instr_data(out, iid->instrumentData + j, indent + 2);
	}

	print_indent(out, indent);
	fputs("</instrumentIdentifier>\n", out);
	return;
}

static void
print_issue_amount(FILE *out, mddl_issueAmount_t iamt, size_t indent)
{
	print_indent(out, indent);
	fputs("<issueAmount>\n", out);

	print_indent(out, indent + 2);
	fprintf(out, "<mdPrice>%2.4f</mdPrice>\n", iamt->Price);

	for (size_t i = 0; i < iamt->ncurrency; i++) {
		mddl_currency_t ccy = iamt->currency + i;
		print_currency(out, ccy, indent);
	}

	print_indent(out, indent);
	fputs("</issueAmount>\n", out);
	return;
}

static void
print_issue_fees(FILE *out, mddl_issueFees_t ifee, size_t indent)
{
	print_indent(out, indent);
	fputs("<issueFees>\n", out);

	print_indent(out, indent + 2);
	fprintf(out, "<mdPrice>%2.4f</mdPrice>\n", ifee->Price);

	for (size_t i = 0; i < ifee->ncurrency; i++) {
		mddl_currency_t ccy = ifee->currency + i;
		print_currency(out, ccy, indent);
	}

	print_indent(out, indent);
	fputs("</issueFees>\n", out);
	return;
}

static void
print_issuer_ref(FILE *out, mddl_issuerRef_t iref, size_t indent)
{
	print_indent(out, indent);
	fputs("<issuerRef>\n", out);

	for (size_t i = 0; i < iref->nname; i++) {
		mddl_name_t n = iref->name + i;
		print_name(out, n, indent + 2);
	}
	for (size_t i = 0; i < iref->ncode; i++) {
		mddl_code_t c = iref->code + i;
		print_code(out, c, indent + 2);
	}

	print_indent(out, indent);
	fputs("</issuerRef>\n", out);
	return;
}

static void
print_issue_date(FILE *out, mddl_issueDate_t idate, size_t indent)
{
	print_indent(out, indent);
	fputs("<issueDate>\n", out);

	print_indent(out, indent + 2);
	fputs("<mdDateTime>", out);
	print_date(out, idate->DateTime);
	fputs("</mdDateTime>\n", out);

	print_indent(out, indent);
	fputs("</issueDate>\n", out);
	return;
}

static void
print_issue_data(FILE *out, mddl_issueData_t id, size_t indent)
{
	print_indent(out, indent);
	fputs("<issueData>\n", out);

	for (size_t i = 0; i < id->nissuerRef; i++) {
		mddl_issuerRef_t iref = id->issuerRef + i;
		print_issuer_ref(out, iref, indent + 2);
	}
	for (size_t i = 0; i < id->nissueDate; i++) {
		mddl_issueDate_t idate = id->issueDate + i;
		print_issue_date(out, idate, indent + 2);
	}
	for (size_t i = 0; i < id->nissueAmount; i++) {
		mddl_issueAmount_t iamt = id->issueAmount + i;
		print_issue_amount(out, iamt, indent + 2);
	}
	for (size_t i = 0; i < id->nissueFees; i++) {
		mddl_issueFees_t ifee = id->issueFees + i;
		print_issue_fees(out, ifee, indent + 2);
	}

	print_indent(out, indent);
	fputs("</issueData>\n", out);
	return;
}

static void
print_objective(FILE *out, mddl_objective_t obj, size_t indent)
{
	print_indent(out, indent);
	fputs("<objective>\n", out);
	print_indent(out, indent + 2);
	fputs(obj->Simple, out);
	fputc('\n', out);
	print_indent(out, indent);
	fputs("</objective>\n", out);
	return;
}

static void
print_fund_strat_type(FILE *out, mddl_fundStrategyType_t fst, size_t indent)
{
	print_indent(out, indent);
	fputs("<fundStrategyType>", out);
	fputs(fst->Enumeration, out);
	fputs("</fundStrategyType>\n", out);
	return;
}

static void
print_distri_type(FILE *out, mddl_distributionType_t dt, size_t indent)
{
	print_indent(out, indent);
	fputs("<distributionType>", out);
	fputs(dt->Enumeration, out);
	fputs("</distributionType>\n", out);
	return;
}

static void
print_insdom(FILE *out, mddl_instrumentDomain_t id, size_t indent)
{
	print_indent(out, indent);
	fputs("<instrumentDomain>\n", out);

	for (size_t i = 0; i < id->ninstrumentIdentifier; i++) {
		mddl_instrumentIdentifier_t insidnt =
			id->instrumentIdentifier + i;
		print_instr_ident(out, insidnt, indent + 2);
	}
	for (size_t i = 0; i < id->nindustryIdentifier; i++) {
		mddl_industryIdentifier_t indidnt =
			id->industryIdentifier + i;
		print_indus_ident(out, indidnt, indent + 2);
	}
	for (size_t i = 0; i < id->nissueData; i++) {
		mddl_issueData_t issd = id->issueData + i;
		print_issue_data(out, issd, indent + 2);
	}

	for (size_t i = 0; i < id->nobjective; i++) {
		mddl_objective_t o = id->objective + i;
		print_objective(out, o, indent + 2);
	}

	for (size_t i = 0; i < id->nfundStrategyType; i++) {
		mddl_fundStrategyType_t fst = id->fundStrategyType + i;
		print_fund_strat_type(out, fst, indent + 2);
	}

	for (size_t i = 0; i < id->ndistributionType; i++) {
		mddl_distributionType_t dt = id->distributionType + i;
		print_distri_type(out, dt, indent + 2);
	}

	print_indent(out, indent);
	fputs("</instrumentDomain>\n", out);
	return;
}

static void
print_header(FILE *out, mddl_header_t hdr, size_t indent)
{
	print_indent(out, indent);
	fputs("<header>\n", out);

	print_indent(out, indent + 2);
	fputs("<dateTime>", out);
	print_zulu(out, hdr->dateTime->AnyDateTime);
	fputs("</dateTime>\n", out);

	print_indent(out, indent + 2);
	fputs("<source>\n", out);
	print_indent(out, indent + 4);
	fputs("<mdString>", out);
	fputs(hdr->source->Simple, out);
	fputs("</mdString>\n", out);
	print_indent(out, indent + 2);
	fputs("</source>\n", out);

	print_indent(out, indent);
	fputs("</header>\n", out);
	return;
}

static void
print_snap(FILE *out, mddl_snap_t snap, size_t indent)
{
	for (size_t i = 0; i < snap->ninstrumentDomain; i++) {
		print_insdom(out, snap->instrumentDomain + i, indent);
	}
	return;
}


void
mddl_print(void *out, mddl_doc_t doc)
{
	const size_t indent = 2;

	fputs("\
<?xml version=\"1.0\"?>\n\
<mddl xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n\
  xmlns=\"http://www.mddl.org/mddl/3.0-beta\"\n\
  version=\"3.0-beta\"\n\
  xsi:schemaLocation=\
\"http://www.mddl.org/mddl/3.0-beta mddl-3.0-beta-full.xsd\">\n", out);
	print_header(out, doc->header, indent);

	for (size_t i = 0; i < doc->nsnap; i++) {
		print_indent(out, indent);
		fputs("<snap>\n", out);
		print_snap(out, doc->snap + i, indent + 2);
		print_indent(out, indent);
		fputs("</snap>\n", out);
	}
	for (size_t i = 0; i < doc->ntimeseries; i++) {
		print_indent(out, indent);
		fputs("<timeseries>\n", out);

		print_indent(out, indent);
		fputs("</timeseries>\n", out);
	}

	fputs("</mddl>\n", out);
	return;
}

/* mddl-print.c ends here */
