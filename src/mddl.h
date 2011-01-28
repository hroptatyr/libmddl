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


/* properties */
struct __p_src_s {
	char *string;
};

struct __p_snap_type_s {
	enum {
		MDDL_SNAP_TYPE_UNK,
		MDDL_SNAP_TYPE_UPDATE,
		MDDL_SNAP_TYPE_NEW,
		MDDL_SNAP_TYPE_EOD,
	} voc_snap_type;
};


/* domains */
struct __dom_instr_s {
};

struct __dom_index_s {
};

struct __dom_pfolio_s {
};

struct __dom_indica_s {
};

struct __dom_forex_s {
};

struct __dom_entity_s {
};

struct __dom_comdty_s {
};

struct __dom_cash_s {
};

struct __dom_cae_s {
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
	} dom;
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
	} snap_choice;
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


struct __g_mddl_choice_s {
	enum {
		MDDL_CHOICE_UNK,
		MDDL_CHOICE_SNAP,
		MDDL_CHOICE_TIMESERIES,
	} mddl_choice;
	union {
		struct __e_snap_s snap[1];
		struct __e_tser_s timeseries[1];
	};
};

struct __e_mddl_s {
	struct __e_hdr_s hdr[1];
	size_t nchoice;
	struct __g_mddl_choice_s choice[];
};

#endif	/* INCLUDED_mddl_h_ */
