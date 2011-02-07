/** mddl.h - mddl stubs and snippets */

#if !defined INCLUDED_mddl_h_
#define INCLUDED_mddl_h_

#undef DECLF
#undef DEFUN
#if defined STATIC_GUTS
# define DEFUN	static
# define DECLF	static
#else  /* !STATIC_GUTS */
# define DEFUN
# define DECLF	extern
#endif	/* STATIC_GUTS */

/* alternatively */
#define DEFMDDL_GROUP(name, types, structs)	\
	struct __g_##name##_s {			\
		enum name##_e {			\
			types			\
		} name##_gt;			\
		size_t n##name;			\
		union {				\
			void *ptr;		\
			structs			\
		};				\
	}
#define STRUCT(structs...)	structs
#define ENUM(types...)		types

#define MDDL_MANY_CNT(name)	n##name
#define MDDL_MANY_SLOT(name)	name
#define MDDL_MANY_OF(name, type)		\
	size_t MDDL_MANY_CNT(name);		\
	type *MDDL_MANY_SLOT(name)
#define MDDL_N_OF(name, type, n)		\
	type MDDL_MANY_SLOT(name)[n]
#define MDDL_ONE_OF(name, type)	MDDL_N_OF(name, type, 1)

/* teh main thing */
typedef struct __e_mddl_s *mddl_doc_t;
typedef struct __e_hdr_s *mddl_hdr_t;
typedef struct __e_snap_s *mddl_snap_t;
typedef struct __dom_instr_s *mddl_dom_instr_t;
typedef struct __p_instr_ident_s *mddl_p_instr_ident_t;
typedef struct __p_issue_data_s *mddl_p_issue_data_t;
typedef struct __p_indus_ident_s *mddl_p_indus_ident_t;
typedef struct __p_code_s *mddl_p_code_t;
typedef struct __p_name_s *mddl_p_name_t;
typedef struct __p_instr_data_s *mddl_p_instr_data_t;
typedef struct __p_currency_s *mddl_p_currency_t;
typedef struct __p_issuer_ref_s *mddl_p_issuer_ref_t;
typedef struct __p_issue_date_s *mddl_p_issue_date_t;
typedef struct __p_issue_amount_s *mddl_p_issue_amount_t;
typedef struct __p_issue_fees_s *mddl_p_issue_fees_t;
typedef struct __p_objective_s *mddl_p_objective_t;
typedef struct __p_crossrate_s *mddl_p_crossrate_t;
typedef struct __p_size_s *mddl_p_size_t;
typedef struct __p_clearing_stlmnt_s *mddl_p_clearing_stlmnt_t;


/* attributes */
typedef int __a_rank_t;
typedef char *__a_role_t;
typedef char *__a_scheme_t;
typedef char *__a_descr_t;
typedef char *__a_code_type_t;
typedef char *__a_nref_t;
typedef char *__a_instr_type_t;
typedef char *__a_instr_form_type_t;
typedef char *__a_comment_t;
typedef char *__a_background_t;
typedef char *__a_country_t;
typedef char *__a_instr_status_type_t;
typedef char *__a_scope_type_t;
typedef char *__a_objctv_type_t;
typedef char *__a_fund_strat_type_t;
typedef char *__a_distri_type_t;
typedef char *__a_stlmnt_type_t;

typedef time_t mddate_time_t;
typedef char *mdstring_t;
typedef char *mduri_t;
typedef double mdprice_t;
typedef double mdrate_t;


/* properties */
struct __p_src_s {
	mdstring_t value;
};

struct __p_snap_type_s {
	enum snap_type_e {
		MDDL_SNAP_TYPE_UNK,
		MDDL_SNAP_TYPE_UPDATE,
		MDDL_SNAP_TYPE_NEW,
		MDDL_SNAP_TYPE_EOD,
	} snap_type_voc;
};

struct __p_name_s {
	MDDL_MANY_OF(rank, __a_rank_t);
	MDDL_MANY_OF(role, __a_role_t);

	/* the actual name contents */
	mdstring_t value;
#if 0
/* not impl'd */
	MDDL_MANY_OF(period, struct __p_period_s);
#endif	/* 0 */
};

struct __p_code_s {
	MDDL_MANY_OF(rank, __a_rank_t);
	MDDL_MANY_OF(nameref, __a_nref_t);
	MDDL_MANY_OF(descr, __a_descr_t);
	MDDL_MANY_OF(code_type, __a_code_type_t);

	/* the scheme, variable in this one */
	__a_scheme_t scheme;

	/* the actual code contents */
	mdstring_t value;
#if 0
/* not impl'd */
	MDDL_MANY_OF(period, struct __p_period_s);
#endif	/* 0 */
};

struct __p_prev_code_s {
/* not implemented */
};

DEFMDDL_GROUP(
	code_name,
	ENUM(
		MDDL_CODE_NAME_UNK,
		MDDL_CODE_NAME_PREVIOUS_CODE,
		MDDL_CODE_NAME_CODE,
		MDDL_CODE_NAME_NAME,
		),
	STRUCT(
		struct __p_name_s *name;
		struct __p_code_s *code;
		struct __p_prev_code_s *prev_code;
		));

struct __p_currency_s {
	__a_scheme_t scheme;
	/* the actual contents */
	mdstring_t value;

#if 0
/* um */
	MDDL_MANY_OF(multiplier, struct __p_multiplier_s);
#endif	/* 0 */
};


struct __p_mkt_ident_s {
};

struct __p_instr_data_s {
	MDDL_MANY_OF(instr_type, __a_instr_type_t);
	MDDL_MANY_OF(instr_form_type, __a_instr_form_type_t);
	MDDL_MANY_OF(background, __a_background_t);
	MDDL_MANY_OF(comment, __a_comment_t);
	MDDL_MANY_OF(currency, struct __p_currency_s);

#if 0
	MDDL_MANY_OF(tra_restr_type, struct __p_tra_restr_type_s);
	MDDL_MANY_OF(last_cae, struct __p_last_cae_s);
#endif	/* 0 */
};

struct __p_issuer_ref_s {
	mduri_t value;
	
	MDDL_MANY_OF(code_name, struct __g_code_name_s);
};

struct __p_issue_date_s {
	mddate_time_t value;

#if 0
	MDDL_MANY_OF(clsf_date_time, struct __g_clsf_date_time_s);
#endif	/* 0 */
};

struct __p_clearing_stlmnt_s {
	MDDL_MANY_OF(settlement_type, __a_stlmnt_type_t);
	MDDL_MANY_OF(mkt_ident, struct __p_mkt_ident_s);
	MDDL_MANY_OF(currency, struct __p_currency_s);

#if 0
/* soon */
	MDDL_MANY_OF(clearing_house, struct __p_clearing_house_s);
	MDDL_MANY_OF(clearing_process, struct __p_clearing_proc_s);
	MDDL_MANY_OF(clearing_system, struct __p_clearing_sys_s);
	MDDL_MANY_OF(depository_name, struct __p_depo_name_s);
	MDDL_MANY_OF(depository_system, struct __p_depo_sys_s);
	MDDL_MANY_OF(parties_involved, struct __p_parties_invlv_s);
#endif	/* 0 */
};

struct __p_crossrate_s {
	__a_scheme_t scheme;
	/* the actual contents */
	mdstring_t value;

#if 0
	MDDL_MANY_OF(multiplier, struct __p_multiplier_s);
#endif	/* 0 */
};

struct __p_size_s {
/* not implemented yet */
};

DEFMDDL_GROUP(
	clsf_price,
	ENUM(
		MDDL_CLSF_PRICE_UNK,
		MDDL_CLSF_PRICE_CURRENCY,
		MDDL_CLSF_PRICE_CROSSRATE,
		MDDL_CLSF_PRICE_SIZE,
		MDDL_CLSF_PRICE_CLSF_RATE,
		),
	STRUCT(
		struct __p_currency_s *currency;
		struct __p_crossrate_s *crossrate;
		struct __p_size_s *size;
		));

struct __p_issue_amount_s {
	mdprice_t value;

	MDDL_MANY_OF(rank, __a_rank_t);
	MDDL_MANY_OF(clsf_price, struct __g_clsf_price_s);

#if 0
	MDDL_MANY_OF(debt_indicators_type, __a_debt_ind_type_t);
#endif	/* 0 */
};

struct __p_issue_fees_s {
	mdprice_t value;

	MDDL_MANY_OF(clsf_price, struct __g_clsf_price_s);
};


struct __p_instr_ident_s {
	MDDL_MANY_OF(country, __a_country_t);
	MDDL_MANY_OF(instr_status_type, __a_instr_status_type_t);
	MDDL_MANY_OF(scope_type, __a_scope_type_t);
	MDDL_MANY_OF(code_name, struct __g_code_name_s);
	MDDL_MANY_OF(mkt_ident, struct __p_mkt_ident_s);
	MDDL_MANY_OF(instr_data, struct __p_instr_data_s);

#if 0
	MDDL_MANY_OF(tranche, struct __p_tranche_s);
	MDDL_MANY_OF(seg_ident, struct __p_seg_ident_s);
#endif
};

struct __p_indus_ident_s {
};

struct __p_issue_data_s {
	MDDL_MANY_OF(issuer_ref, struct __p_issuer_ref_s);
	MDDL_MANY_OF(issue_date, struct __p_issue_date_s);
	MDDL_MANY_OF(issue_amount, struct __p_issue_amount_s);
	MDDL_MANY_OF(clearing_settlement, struct __p_clearing_stlmnt_s);
	MDDL_MANY_OF(issue_fees, struct __p_issue_fees_s);
};

DEFMDDL_GROUP(
	basic_idents,
	ENUM(
		MDDL_BASIC_IDENT_UNK,
		MDDL_BASIC_IDENT_INDUS_IDENT,
		MDDL_BASIC_IDENT_INSTR_IDENT,
		MDDL_BASIC_IDENT_ISSUE_DATA,
		),
	STRUCT(
		struct __p_indus_ident_s *indus_ident;
		struct __p_instr_ident_s *instr_ident;
		struct __p_issue_data_s *issue_data;
		));

struct __p_objective_s {
	MDDL_MANY_OF(objective_type, __a_objctv_type_t);

	mdstring_t value;
};


/* domains */
struct __dom_instr_s {
	MDDL_MANY_OF(basic_idents, struct __g_basic_idents_s);
#if 0
/* not implemented */
	MDDL_MANY_OF(basic_quotes, struct __g_basic_quotes_s);
#endif	/* 0 */

	MDDL_MANY_OF(objective, struct __p_objective_s);
	MDDL_MANY_OF(fund_strat_type, __a_fund_strat_type_t);
	MDDL_MANY_OF(distri_type, __a_distri_type_t);
};

struct __dom_index_s {
	/* not implemented */
};

struct __dom_pfolio_s {
	/* not implemented */
};

struct __dom_indica_s {
	/* not implemented */
};

struct __dom_forex_s {
	/* not implemented */
};

struct __dom_entity_s {
	/* not implemented */
};

struct __dom_comdty_s {
	/* not implemented */
};

struct __dom_cash_s {
	/* not implemented */
};

struct __dom_cae_s {
	/* not implemented */
};

DEFMDDL_GROUP(
	domains,
	ENUM(
		MDDL_DOM_UNK,
		MDDL_DOM_INSTRUMENT,
		MDDL_DOM_INDEX,
		MDDL_DOM_PORTFOLIO,
		MDDL_DOM_INDICATOR,
		MDDL_DOM_FOREX,
		MDDL_DOM_ENTITY,
		MDDL_DOM_COMMODITY,
		MDDL_DOM_CASH,
		MDDL_DOM_CAE,
		),
	STRUCT(
		struct __dom_instr_s *instrument;
		struct __dom_index_s *index;
		struct __dom_pfolio_s *portfolio;
		struct __dom_indica_s *indicator;
		struct __dom_forex_s *forex;
		struct __dom_entity_s *entity;
		struct __dom_comdty_s *commodity;
		struct __dom_cash_s *cash;
		struct __dom_cae_s *cae;
		));


/* elements */
DEFMDDL_GROUP(
	snap_choi,
	ENUM(
		MDDL_SNAP_CHOICE_UNK,
		MDDL_SNAP_CHOICE_DOMAINS,
		),
	STRUCT(
		struct __g_domains_s *domains;
		));

struct __e_snap_s {
	MDDL_ONE_OF(choice, struct __g_snap_choi_s);
};

struct __e_tser_s {
	/* not implemented yet */
};

struct __e_hdr_s {
	mddate_time_t stamp;
	MDDL_ONE_OF(source, struct __p_src_s);
};


DEFMDDL_GROUP(
	mddl_choi,
	ENUM(
		MDDL_CHOICE_UNK,
		MDDL_CHOICE_SNAP,
		MDDL_CHOICE_TIMESERIES,
		),
	STRUCT(
		struct __e_snap_s *snap;
		struct __e_tser_s *timeseries;
		));

struct __e_mddl_s {
	MDDL_ONE_OF(hdr, struct __e_hdr_s);
	MDDL_ONE_OF(choice, struct __g_mddl_choi_s);
};


/* functions */
DECLF mddl_snap_t mddl_add_snap(mddl_doc_t doc);

DECLF mddl_dom_instr_t mddl_snap_add_dom_instr(mddl_snap_t snap);

DECLF mddl_p_instr_ident_t mddl_dom_instr_add_instr_ident(mddl_dom_instr_t);
DECLF mddl_p_issue_data_t mddl_dom_instr_add_issue_data(mddl_dom_instr_t);
DECLF mddl_p_objective_t mddl_dom_instr_add_objective(mddl_dom_instr_t);
DECLF __a_fund_strat_type_t
mddl_dom_instr_add_fund_strat_type(mddl_dom_instr_t, const char *fst);
DECLF __a_distri_type_t
mddl_dom_instr_add_distri_type(mddl_dom_instr_t, const char *dt);

DECLF mddl_p_name_t mddl_instr_ident_add_name(mddl_p_instr_ident_t);
DECLF mddl_p_code_t mddl_instr_ident_add_code(mddl_p_instr_ident_t);
DECLF mddl_p_instr_data_t mddl_instr_ident_add_instr_data(mddl_p_instr_ident_t);

DECLF mddl_p_issuer_ref_t mddl_issue_data_add_issuer_ref(mddl_p_issue_data_t);
DECLF mddl_p_issue_date_t mddl_issue_data_add_issue_date(mddl_p_issue_data_t);
DECLF mddl_p_issue_amount_t
mddl_issue_data_add_issue_amount(mddl_p_issue_data_t);
DECLF mddl_p_issue_fees_t
mddl_issue_data_add_issue_fees(mddl_p_issue_data_t);
DECLF mddl_p_clearing_stlmnt_t
mddl_issue_data_add_clearing_stlmnt(mddl_p_issue_data_t);

DECLF mddl_p_name_t mddl_issuer_ref_add_name(mddl_p_issuer_ref_t);
DECLF mddl_p_code_t mddl_issuer_ref_add_code(mddl_p_issuer_ref_t);

DEFUN mddl_p_currency_t mddl_issue_amount_add_currency(mddl_p_issue_amount_t);
DEFUN mddl_p_crossrate_t mddl_issue_amount_add_crossrate(mddl_p_issue_amount_t);
DEFUN mddl_p_size_t mddl_issue_amount_add_size(mddl_p_issue_amount_t);

DEFUN mddl_p_currency_t mddl_issue_fees_add_currency(mddl_p_issue_fees_t);
DEFUN mddl_p_crossrate_t mddl_issue_fees_add_crossrate(mddl_p_issue_fees_t);
DEFUN mddl_p_size_t mddl_issue_fees_add_size(mddl_p_issue_fees_t);

DECLF __a_instr_type_t
mddl_instr_data_add_instr_type(mddl_p_instr_data_t, const char *type);
DECLF mddl_p_currency_t mddl_instr_data_add_currency(mddl_p_instr_data_t);

DECLF __a_scheme_t mddl_code_add_scheme(mddl_p_code_t code, const char *scheme);
DECLF __a_rank_t mddl_code_add_rank(mddl_p_code_t code, int rank);

DECLF __a_role_t mddl_name_add_role(mddl_p_name_t name, const char *role);
DECLF __a_rank_t mddl_name_add_rank(mddl_p_name_t name, int rank);

/* print service */
DECLF void mddl_print(void *out, mddl_doc_t doc);

#endif	/* INCLUDED_mddl_h_ */
