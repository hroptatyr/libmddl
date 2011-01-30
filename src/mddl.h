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

/* teh main thing */
typedef struct __e_mddl_s *mddl_doc_t;


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

typedef char *mdstring_t;
typedef char *mduri_t;


/* properties */
struct __p_src_s {
	mdstring_t value;
};

struct __p_snap_type_s {
	enum {
		MDDL_SNAP_TYPE_UNK,
		MDDL_SNAP_TYPE_UPDATE,
		MDDL_SNAP_TYPE_NEW,
		MDDL_SNAP_TYPE_EOD,
	} snap_type_voc;
};

struct __p_name_s {
	size_t nrank;
	__a_rank_t *rank;

	size_t nrole;
	__a_role_t *role;

	/* the actual name contents */
	mdstring_t value;
#if 0
/* not impl'd */
	size_t nperiod;
	struct __p_period_s *period;
#endif	/* 0 */
};

struct __p_code_s {
	size_t nrank;
	__a_rank_t *rank;

	size_t nnameref;
	__a_nref_t *nameref;

	size_t ndescr;
	__a_descr_t *descr;

	size_t ncode_type;
	__a_code_type_t *code_type;

	/* the scheme, variable in this one */
	__a_scheme_t scheme;

	/* the actual code contents */
	mdstring_t value;
#if 0
/* not impl'd */
	size_t nperiod;
	struct __p_period_s *period;
#endif	/* 0 */
};

struct __p_prev_code_s {
/* not implemented */
};

struct __g_code_name_s {
	enum {
		MDDL_CODE_NAME_UNK,
		MDDL_CODE_NAME_PREVIOUS_CODE,
		MDDL_CODE_NAME_CODE,
		MDDL_CODE_NAME_NAME,
	} code_name_gt;
	size_t ncode_name;
	union {
		struct __p_name_s *name;
		struct __p_code_s *code;
		struct __p_prev_code_s *prev_code;
	};
};

struct __p_currency_s {
	__a_scheme_t scheme;
	/* the actual contents */
	mdstring_t value;

#if 0
/* um */
	size_t nmultiplier;
	struct __p_multiplier_s *multiplier;
#endif	/* 0 */
};


struct __p_mkt_ident_s {
};

struct __p_instr_data_s {
	size_t ninstr_type;
	__a_instr_type_t *instr_type;

	size_t ninstr_form_type;
	__a_instr_form_type_t *instr_form_type;

	size_t nbackground;
	__a_background_t *background;

	size_t ncomment;
	__a_comment_t *comment;

	size_t ncurrency;
	struct __p_currency_s *currency;

#if 0
	size_t ntra_restr_type;
	struct __p_tra_restr_type_s *tra_restr_type;

	size_t nlast_cae;
	struct __p_last_cae_s *last_cae;
#endif	/* 0 */
};

struct __p_issuer_ref_s {
	mduri_t value;
	
	size_t ncode_name;
	struct __g_code_name_s *code_name;
};


struct __p_instr_ident_s {
	size_t ncountry;
	__a_country_t *country;

	size_t ninstr_status_type;
	__a_instr_status_type_t *instr_status_type;

	size_t nscope_type;
	__a_scope_type_t *scope_type;

	size_t ncode_name;
	struct __g_code_name_s *code_name;

	size_t nmkt_ident;
	struct __p_mkt_ident_s *mkt_ident;

	size_t ninstr_data;
	struct __p_instr_data_s *instr_data;

#if 0
	size_t ntranche;
	struct __p_tranche_s *tranche;

	size_t nseg_ident;
	struct __p_seg_ident_s *seg_ident;
#endif
};

struct __p_indus_ident_s {
};

struct __p_issue_data_s {
	size_t nissuer_ref;
	struct __p_issuer_ref_s *issuer_ref;
};

struct __g_basic_idents_s {
	enum {
		MDDL_BASIC_IDENT_UNK,
	} basic_ident_gt;
	size_t nidents;
	union {
		struct __p_indus_ident_s *indus_ident;
		struct __p_instr_ident_s *instr_ident;
		struct __p_issue_data_s *issue_data;
	};
};


/* domains */
struct __dom_instr_s {
	size_t nbasic_idents;
	struct __g_basic_idents_s *basic_idents;
#if 0
/* not implemented */
	size_t nbasic_quotes;
	struct __g_basic_quotes_s *basic_quotes;
#endif	/* 0 */
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

struct __g_domains_s {
	enum {
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
	} domains_gt;
	size_t ndomains;
	union {
		struct __dom_instr_s *instrument;
		struct __dom_index_s *index;
		struct __dom_pfolio_s *portfolio;
		struct __dom_indica_s *indicator;
		struct __dom_forex_s *forex;
		struct __dom_entity_s *entity;
		struct __dom_comdty_s *commodity;
		struct __dom_cash_s *cash;
		struct __dom_cae_s *cae;
	};
};


/* elements */
struct __g_snap_choi_s {
	enum {
		MDDL_SNAP_CHOICE_UNK,
		MDDL_SNAP_CHOICE_DOMAINS,
	} snap_choi_gt;
	time_t stamp;
	struct __p_snap_type_s snap_type[1];
	size_t ndomains;
	union {
		struct __g_domains_s *domains;
	};
};

struct __e_snap_s {
	struct __g_snap_choi_s choice[1];
};

struct __e_tser_s {
	/* not implemented yet */
};

struct __e_hdr_s {
	time_t stamp;
	struct __p_src_s source[1];
};


struct __g_mddl_choi_s {
	enum {
		MDDL_CHOICE_UNK,
		MDDL_CHOICE_SNAP,
		MDDL_CHOICE_TIMESERIES,
	} mddl_choi_gt;
	size_t nchoice;
	union {
		struct __e_snap_s *snap;
		struct __e_tser_s *timeseries;
	};
};

struct __e_mddl_s {
	struct __e_hdr_s hdr[1];
	struct __g_mddl_choi_s choice[1];
};

#endif	/* INCLUDED_mddl_h_ */
