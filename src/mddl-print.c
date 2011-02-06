#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>
#include "mddl.h"

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
print_zulu(FILE *out, mddate_time_t stamp)
{
	struct tm tm[1] = {{0}};
	char buf[32];
	gmtime_r(&stamp, tm);
	strftime(buf, sizeof(buf), "%FT%T%z", tm);
	fputs(buf, out);
	return;
}

static void
print_date(FILE *out, mddate_time_t stamp)
{
	struct tm tm[1] = {{0}};
	char buf[32];
	gmtime_r(&stamp, tm);
	strftime(buf, sizeof(buf), "%F", tm);
	fputs(buf, out);
	return;
}

static void
print_name(FILE *out, mddl_p_name_t name, size_t indent)
{
	print_indent(out, indent);
	fputs("<name>\n", out);

	print_indent(out, indent + 2);
	fputs("<mdString>", out);
	fputs(name->value, out);
	fputs("</mdString>\n", out);

	for (size_t i = 0; i < name->nrole; i++) {
		print_indent(out, indent + 2);
		fputs("<role>", out);
		fputs(name->role[i], out);
		fputs("</role>\n", out);
	}
	for (size_t i = 0; i < name->nrank; i++) {
		print_indent(out, indent + 2);
		fprintf(out, "<rank>%i</rank>\n", name->rank[i]);
	}

	print_indent(out, indent);
	fputs("</name>\n", out);
	return;
}

static void
print_code(FILE *out, mddl_p_code_t code, size_t indent)
{
	print_indent(out, indent);
	fprintf(out, "<code scheme=\"%s\">\n", code->scheme);

	print_indent(out, indent + 2);
	fputs("<mdString>", out);
	fputs(code->value, out);
	fputs("</mdString>\n", out);
	for (size_t i = 0; i < code->nrank; i++) {
		print_indent(out, indent + 2);
		fprintf(out, "<rank>%i</rank>\n", code->rank[i]);
	}

	print_indent(out, indent);
	fputs("</code>\n", out);
	return;
}

static void
print_currency(FILE *out, mddl_p_currency_t ccy, size_t indent)
{
	print_indent(out, indent);
	fputs("<currency>", out);
	fputs(ccy->value, out);
	fputs("</currency>\n", out);
	return;
}

static void
print_instr_type(FILE *out, __a_instr_type_t type, size_t indent)
{
	print_indent(out, indent);
	fputs("<instrumentType>", out);
	fputs(type, out);
	fputs("</instrumentType>\n", out);
	return;
}

static void
print_instr_data(FILE *out, mddl_p_instr_data_t id, size_t indent)
{
	print_indent(out, indent);
	fputs("<instrumentData>\n", out);

	for (size_t i = 0; i < id->ninstr_type; i++) {
		__a_instr_type_t t = id->instr_type[i];
		print_instr_type(out, t, indent + 2);
	}
	for (size_t i = 0; i < id->ncurrency; i++) {
		mddl_p_currency_t ccy = id->currency + i;
		print_currency(out, ccy, indent + 2);
	}

	print_indent(out, indent);
	fputs("</instrumentData>\n", out);
	return;
}

static void
print_indus_ident(FILE *out, mddl_p_indus_ident_t iid, size_t indent)
{
	return;
}

static void
print_instr_ident(FILE *out, mddl_p_instr_ident_t iid, size_t indent)
{
	print_indent(out, indent);
	fputs("<instrumentIdentifier>\n", out);

	for (size_t i = 0; i < iid->ncode_name; i++) {
		struct __g_code_name_s *cn = iid->code_name + i;
		switch (cn->code_name_gt) {
		case MDDL_CODE_NAME_NAME:
			for (size_t j = 0; j < cn->ncode_name; j++) {
				mddl_p_name_t n = cn->name + j;
				print_name(out, n, indent + 2);
			}
			break;
		case MDDL_CODE_NAME_CODE:
			for (size_t j = 0; j < cn->ncode_name; j++) {
				mddl_p_code_t c = cn->code + j;
				print_code(out, c, indent + 2);
			}
			break;
		default:
			break;
		}
	}
	for (size_t j = 0; j < iid->ninstr_data; j++) {
		print_instr_data(out, iid->instr_data + j, indent + 2);
	}

	print_indent(out, indent);
	fputs("</instrumentIdentifier>\n", out);
	return;
}

static void
print_clsf_price(FILE *out, struct __g_clsf_price_s *cp, size_t indent)
{
	switch (cp->clsf_price_gt) {
	case MDDL_CLSF_PRICE_CURRENCY:
		for (size_t j = 0; j < cp->nclsf_price; j++) {
			mddl_p_currency_t ccy = cp->currency + j;
			print_currency(out, ccy, indent);
		}
		break;
	default:
		break;
	}
	return;
}

static void
print_issue_amount(FILE *out, mddl_p_issue_amount_t iamt, size_t indent)
{
	print_indent(out, indent);
	fputs("<issueAmount>\n", out);

	print_indent(out, indent + 2);
	fprintf(out, "<mdPrice>%2.4f</mdPrice>\n", iamt->value);

	for (size_t i = 0; i < iamt->nclsf_price; i++) {
		struct __g_clsf_price_s *cp = iamt->clsf_price + i;
		print_clsf_price(out, cp, indent + 2);
	}

	print_indent(out, indent);
	fputs("</issueAmount>\n", out);
	return;
}

static void
print_issue_fees(FILE *out, mddl_p_issue_fees_t ifee, size_t indent)
{
	print_indent(out, indent);
	fputs("<issueFees>\n", out);

	print_indent(out, indent + 2);
	fprintf(out, "<mdPrice>%2.4f</mdPrice>\n", ifee->value);

	for (size_t i = 0; i < ifee->nclsf_price; i++) {
		struct __g_clsf_price_s *cp = ifee->clsf_price + i;
		print_clsf_price(out, cp, indent + 2);
	}

	print_indent(out, indent);
	fputs("</issueFees>\n", out);
	return;
}

static void
print_issuer_ref(FILE *out, mddl_p_issuer_ref_t iref, size_t indent)
{
	print_indent(out, indent);
	fputs("<issuerRef>\n", out);

	for (size_t k = 0; k < iref->ncode_name; k++) {
		struct __g_code_name_s *cn = iref->code_name + k;

		switch (cn->code_name_gt) {
		case MDDL_CODE_NAME_NAME:
			for (size_t j = 0; j < cn->ncode_name; j++) {
				mddl_p_name_t n = cn->name + j;
				print_name(out, n, indent + 2);
			}
			break;
		case MDDL_CODE_NAME_CODE:
			for (size_t j = 0; j < cn->ncode_name; j++) {
				mddl_p_code_t c = cn->code + j;
				print_code(out, c, indent + 2);
			}
			break;
		default:
			break;
		}
	}

	print_indent(out, indent);
	fputs("</issuerRef>\n", out);
	return;
}

static void
print_issue_date(FILE *out, mddl_p_issue_date_t idate, size_t indent)
{
	print_indent(out, indent);
	fputs("<issueDate>\n", out);

	print_indent(out, indent + 2);
	fputs("<mdDateTime>", out);
	print_date(out, idate->value);
	fputs("</mdDateTime>\n", out);

	print_indent(out, indent);
	fputs("</issueDate>\n", out);
	return;
}

static void
print_issue_data(FILE *out, mddl_p_issue_data_t id, size_t indent)
{
	print_indent(out, indent);
	fputs("<issueData>\n", out);

	for (size_t i = 0; i < id->nissuer_ref; i++) {
		struct __p_issuer_ref_s *iref = id->issuer_ref + i;
		print_issuer_ref(out, iref, indent + 2);
	}
	for (size_t i = 0; i < id->nissue_date; i++) {
		struct __p_issue_date_s *idate = id->issue_date + i;
		print_issue_date(out, idate, indent + 2);
	}
	for (size_t i = 0; i < id->nissue_amount; i++) {
		struct __p_issue_amount_s *iamt = id->issue_amount + i;
		print_issue_amount(out, iamt, indent + 2);
	}
	for (size_t i = 0; i < id->nissue_fees; i++) {
		struct __p_issue_fees_s *ifee = id->issue_fees + i;
		print_issue_fees(out, ifee, indent + 2);
	}

	print_indent(out, indent);
	fputs("</issueData>\n", out);
	return;
}

static void
print_objective(FILE *out, mddl_p_objective_t obj, size_t indent)
{
	print_indent(out, indent);
	fputs("<objective>\n", out);
	print_indent(out, indent + 2);
	fputs(obj->value, out);
	fputc('\n', out);
	print_indent(out, indent);
	fputs("</objective>\n", out);
	return;
}

static void
print_fund_strat_type(FILE *out, __a_fund_strat_type_t fst, size_t indent)
{
	print_indent(out, indent);
	fputs("<fundStrategyType>", out);
	fputs(fst, out);
	fputs("</fundStrategyType>\n", out);
	return;
}

static void
print_distri_type(FILE *out, __a_distri_type_t dt, size_t indent)
{
	print_indent(out, indent);
	fputs("<distributionType>", out);
	fputs(dt, out);
	fputs("</distributionType>\n", out);
	return;
}

static void
print_insdom(FILE *out, mddl_dom_instr_t id, size_t indent)
{
	print_indent(out, indent);
	fputs("<instrumentDomain>\n", out);

	for (size_t i = 0; i < id->nbasic_idents; i++) {
		struct __g_basic_idents_s *bi = id->basic_idents + i;
		switch (bi->basic_idents_gt) {
		case MDDL_BASIC_IDENT_INDUS_IDENT:
			for (size_t j = 0; j < bi->nbasic_idents; j++) {
				mddl_p_indus_ident_t indidnt =
					bi->indus_ident + j;
				print_indus_ident(out, indidnt, indent + 2);
			}
			break;
		case MDDL_BASIC_IDENT_INSTR_IDENT:
			for (size_t j = 0; j < bi->nbasic_idents; j++) {
				mddl_p_instr_ident_t insidnt =
					bi->instr_ident + j;
				print_instr_ident(out, insidnt, indent + 2);
			}
			break;
		case MDDL_BASIC_IDENT_ISSUE_DATA:
			for (size_t j = 0; j < bi->nbasic_idents; j++) {
				mddl_p_issue_data_t issd =
					bi->issue_data + j;
				print_issue_data(out, issd, indent + 2);
			}
			break;
		default:
			break;
		}
	}

	for (size_t i = 0; i < id->nobjective; i++) {
		mddl_p_objective_t o = id->objective + i;
		print_objective(out, o, indent + 2);
	}

	for (size_t i = 0; i < id->nfund_strat_type; i++) {
		__a_fund_strat_type_t fst = id->fund_strat_type[i];
		print_fund_strat_type(out, fst, indent + 2);
	}

	for (size_t i = 0; i < id->ndistri_type; i++) {
		__a_distri_type_t dt = id->distri_type[i];
		print_distri_type(out, dt, indent + 2);
	}

	print_indent(out, indent);
	fputs("</instrumentDomain>\n", out);
	return;
}

static void
print_header(FILE *out, struct __e_hdr_s *hdr, size_t indent)
{
	print_indent(out, indent);
	fputs("<header>\n", out);

	print_indent(out, indent + 2);
	fputs("<mdDateTime>", out);
	print_zulu(out, hdr->stamp);
	fputs("</mdDateTime>\n", out);

	print_indent(out, indent + 2);
	fputs("<source>\n", out);
	print_indent(out, indent + 4);
	fputs("<mdString>", out);
	fputs(hdr->source->value, out);
	fputs("</mdString>\n", out);
	print_indent(out, indent + 2);
	fputs("</source>\n", out);

	print_indent(out, indent);
	fputs("</header>\n", out);
	return;
}

static void
print_domain(FILE *out, struct __g_domains_s *dom, size_t indent)
{
	switch (dom->domains_gt) {
	case MDDL_DOM_INSTRUMENT:
		for (size_t i = 0; i < dom->ndomains; i++) {
			print_insdom(out, dom->instrument + i, indent);
		}
		break;
	case MDDL_DOM_INDEX:
	case MDDL_DOM_PORTFOLIO:
	case MDDL_DOM_INDICATOR:
	case MDDL_DOM_FOREX:
	case MDDL_DOM_ENTITY:
	case MDDL_DOM_COMMODITY:
	case MDDL_DOM_CASH:
	case MDDL_DOM_CAE:
	default:
		break;
	}
	return;
}

static void
print_snap_choi(FILE *out, struct __g_snap_choi_s *snch, size_t indent)
{
	switch (snch->snap_choi_gt) {
	case MDDL_SNAP_CHOICE_DOMAINS:
		for (size_t i = 0; i < snch->nsnap_choi; i++) {
			print_domain(out, snch->domains + i, indent);
		}
		break;
	default:
		break;
	}
	return;
}

static void
print_snap(FILE *out, struct __e_snap_s *snap, size_t indent)
{
	print_snap_choi(out, snap->choice, indent);
	return;
}

static void
print_choice(FILE *out, struct __g_mddl_choi_s *choice, size_t indent)
{
	switch (choice->mddl_choi_gt) {
	case MDDL_CHOICE_SNAP:
		for (size_t i = 0; i < choice->nmddl_choi; i++) {
			print_indent(out, indent);
			fputs("<snap>\n", out);
			print_snap(out, choice->snap + i, indent + 2);
			print_indent(out, indent);
			fputs("</snap>\n", out);
		}
		break;

	case MDDL_CHOICE_TIMESERIES:
		for (size_t i = 0; i < choice->nmddl_choi; i++) {
			print_indent(out, indent);
			fputs("<timeseries>\n", out);
		
			print_indent(out, indent);
			fputs("</timeseries>\n", out);
		}
		break;

	default:
		break;
	}
	return;
}


void
mddl_print(void *out, mddl_doc_t doc)
{
	fputs("\
<?xml version=\"1.0\"?>\n\
<mddl xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n\
  xmlns=\"http://www.mddl.org/mddl/3.0-beta\"\n\
  version=\"3.0-beta\"\n\
  xsi:schemaLocation=\
\"http://www.mddl.org/mddl/3.0-beta mddl-3.0-beta-full.xsd\">\n", out);
	print_header(out, doc->hdr, 2);
	print_choice(out, doc->choice, 2);
	fputs("</mddl>\n", out);
	return;
}

/* mddl-print.c ends here */
