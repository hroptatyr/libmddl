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
#define DEFMDDL_GROUP(name, types, structs, rest...)	\
	struct __g_##name##_s {				\
		enum name##_e {				\
			types				\
		} name##_gt;				\
		size_t n##name;				\
		union {					\
			void *ptr;			\
			structs				\
		};					\
		rest					\
	}
#define STRUCT(structs...)	structs
#define ENUM(types...)		types

#define MDDL_PROP(name)		struct __p_##name##_s
#define MDDL_GROUP(name)	struct __g_##name##_s

#define DECL_ADD_F(name, nty, slot, slotty)				\
	DECLF slotty(slot) *mddl_##name##_add_##slot(nty(name)*)

#define DECLP_ADD_PROPF(name, prop)					\
	DECL_ADD_F(name, MDDL_PROP, prop, MDDL_PROP)
#define DECLP_ADD_GROUPF(name, grp)					\
	DECL_ADD_F(name, MDDL_PROP, grp, MDDL_GROUP)
#define DECLG_ADD_PROPF(name, prop)					\
	DECL_ADD_F(name, MDDL_GROUP, prop, MDDL_PROP)
#define DECLG_ADD_GROUPF(name, grp)					\
	DECL_ADD_F(name, MDDL_GROUP, grp, MDDL_GROUP)

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
typedef struct __p_multiplier_s *mddl_p_multiplier_t;


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
typedef char *__a_seg_type_t;
typedef char *__a_tranche_type_t;
typedef char *__a_cap_type_t;
typedef char *__a_day_rule_type_t;
typedef char *__a_period_type_t;
typedef enum __a_dow_e __a_dow_t;
typedef char *__a_mkt_type_t;
typedef char *__a_mkt_status_type_t;
typedef char *__a_calc_type_t;
typedef char *__a_indicators_type_t;
typedef struct __a_fraction_s *__a_fraction_t;
typedef char *__a_frac_hdl_type_t;
typedef char *__a_quant_descr_type_t;
typedef char *__a_quant_type_t;
typedef char *__a_valua_type_t;
typedef char *__a_snap_type_t;
typedef char *__a_debt_ind_type_t;
typedef char *__a_timezone_t;
typedef char *__a_agt_type_t;
typedef char *__a_entity_type_t;
typedef char *__a_party_status_type_t;
typedef char *__a_loc_type_t;
typedef char *__a_website_t;
typedef char *__a_telephone_type_t;
typedef char *__a_incl_type_t;
typedef char *__a_cae_type_t;
typedef char *__a_obook_type_t;
typedef char *__a_yield_type_t;
typedef char *__a_change_type_t;
typedef char *__a_change_dir_t;
typedef char *__a_open_type_t;
typedef char *__a_close_type_t;

typedef time_t mddate_time_t;
typedef time_t mdtime_t;
typedef char *mdstring_t;
typedef char *mduri_t;
typedef double mddecimal_t;
typedef double mdprice_t;
typedef double mdrate_t;
typedef struct __t_dura_s *__t_dura_t;
/* special bool, -1 means unset, 0 means false, 1 means true */
typedef int mdbool_t;
typedef int mdint_t;


/* data types and enums*/
struct __t_dura_s {
	int y;
	int m;
	int d;
	int hr;
	int min;
	int sec;
	int ns;
};

enum __a_dow_e {
	SUNDAY,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY,
};

struct __a_fraction_s {
	int act_denom;
	int display_denom;
	int display_numer;
};

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

struct __p_hours_s {
	MDDL_MANY_OF(duration, struct __t_dura_s);
	MDDL_MANY_OF(period_type, __a_period_type_t);
	MDDL_MANY_OF(start_time, mddate_time_t);
	MDDL_MANY_OF(end_time, mddate_time_t);
};

struct __p_day_of_week_s {
	__a_dow_t value;

	MDDL_MANY_OF(hours, struct __p_hours_s);
};

struct __p_ordinal_day_s {
	MDDL_MANY_OF(day_rule_type, __a_day_rule_type_t);
	MDDL_MANY_OF(duration, struct __t_dura_s);

	mdint_t value;

	MDDL_MANY_OF(day_of_week, struct __p_day_of_week_s);
};

struct __p_start_s {
	mddate_time_t value;

	MDDL_MANY_OF(ordinal_day, struct __p_ordinal_day_s);
};

struct __p_end_s {
	mddate_time_t value;

	MDDL_MANY_OF(ordinal_day, struct __p_ordinal_day_s);
};

struct __p_period_s {
	MDDL_MANY_OF(day_rule_type, __a_day_rule_type_t);
	MDDL_MANY_OF(duration, struct __t_dura_s);
	MDDL_MANY_OF(period_type, __a_period_type_t);
	/* spec says this is 0..* but that hardly makes sense */
	mdbool_t recurring;

	MDDL_MANY_OF(start, struct __p_start_s);
	MDDL_MANY_OF(end, struct __p_end_s);
};

struct __p_name_s {
	MDDL_MANY_OF(rank, __a_rank_t);
	MDDL_MANY_OF(role, __a_role_t);

	/* the actual name contents */
	mdstring_t value;

	MDDL_MANY_OF(period, struct __p_period_s);
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

	MDDL_MANY_OF(period, struct __p_period_s);
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

DEFMDDL_GROUP(
	clsf_rate_novalue,
	ENUM(
		MDDL_CLSF_RATE_NOVALUE_UNK,
		MDDL_CLSF_RATE_NOVALUE_MULTIPLIER,
		),
	STRUCT(
		struct __p_multiplier_s *multiplier;
		),
	MDDL_MANY_OF(calc_type, __a_calc_type_t);
	MDDL_MANY_OF(date_time, mddate_time_t);
	MDDL_MANY_OF(indicators_type, __a_indicators_type_t);
	MDDL_MANY_OF(fraction, struct __a_fraction_s);
	MDDL_MANY_OF(fraction_handling_type, __a_frac_hdl_type_t);
	);

DEFMDDL_GROUP(
	clsf_price_novalue,
	ENUM(
		MDDL_CLSF_PRICE_NOVALUE_UNK,
		MDDL_CLSF_PRICE_NOVALUE_CLSF_RATE_NOVALUE,
		MDDL_CLSF_PRICE_NOVALUE_CROSSRATE,
		MDDL_CLSF_PRICE_NOVALUE_CURRENCY,
		),
	STRUCT(
		struct __g_clsf_rate_novalue_s *clsf_rate_novalue;
		struct __p_crossrate_s *crossrate;
		struct __p_currency_s *currency;
		),
	STRUCT(
		MDDL_MANY_OF(quantity_descr_type, __a_quant_descr_type_t);
		MDDL_MANY_OF(quantity_type, __a_quant_type_t);
		));

struct __p_days_s {
	MDDL_MANY_OF(day_of_week, struct __p_day_of_week_s);
};

struct __p_valua_base_s {
	mdprice_t value;

	MDDL_MANY_OF(clsf_price_novalue, struct __g_clsf_price_novalue_s);
};

struct __p_mkt_cntr_s {
	MDDL_MANY_OF(mkt_status_type, __a_mkt_status_type_t);

	MDDL_MANY_OF(code_name, struct __g_code_name_s);
};

struct __p_mkt_cond_s {
	MDDL_MANY_OF(delay_factor, __t_dura_t);
	MDDL_MANY_OF(timezone, __a_timezone_t);

	/* Conditions of the market relating to time of operations. */
	mdstring_t value;

	MDDL_MANY_OF(period, struct __p_period_s);
	MDDL_MANY_OF(days, struct __p_days_s);
};

struct __p_valua_ref_s {
	mduri_t value;

	MDDL_MANY_OF(valua_base, struct __p_valua_base_s);
};

DEFMDDL_GROUP(
	clsf_rate,
	ENUM(
		MDDL_CLSF_RATE_UNK,
		MDDL_CLSF_RATE_MULTIPLIER,
		MDDL_CLSF_RATE_MKT_CNTR,
		MDDL_CLSF_RATE_VALUA_REF,
		),
	STRUCT(
		struct __p_multiplier_s *multiplier;
		struct __p_mkt_cntr_s *mkt_cntr;
		struct __p_valua_ref_s *valua_ref;
		),
	STRUCT(
		MDDL_MANY_OF(calc_type, __a_calc_type_t);
		MDDL_MANY_OF(date_time, mddate_time_t);
		MDDL_MANY_OF(fraction, struct __a_fraction_s);
		MDDL_MANY_OF(fraction_handling_type, __a_frac_hdl_type_t);
		MDDL_MANY_OF(indicators_type, __a_indicators_type_t);
		MDDL_MANY_OF(prev_date_time, mddate_time_t);
		MDDL_MANY_OF(quantity_descr_type, __a_quant_descr_type_t);
		MDDL_MANY_OF(quantity_type, __a_quant_type_t);
		MDDL_MANY_OF(valuation_type, __a_valua_type_t);
		));

struct __p_multiplier_s {
	mddecimal_t value;

	MDDL_MANY_OF(clsf_rate, struct __g_clsf_rate_s);
};

struct __p_currency_s {
	__a_scheme_t scheme;
	/* the actual contents */
	mdstring_t value;

	MDDL_MANY_OF(multiplier, struct __p_multiplier_s);
};

struct __p_tra_restr_type_s {
	MDDL_MANY_OF(incl_type, __a_incl_type_t);

	mdstring_t value;
};

struct __p_last_cae_s {
	MDDL_MANY_OF(cae_type, __a_cae_type_t);
	MDDL_MANY_OF(date_time, mddate_time_t);
};

DEFMDDL_GROUP(
	markets,
	ENUM(
		MDDL_MARKETS_UNK,
		MDDL_MARKETS_MKT_CNTR,
		MDDL_MARKETS_MKT_COND,
		MDDL_MARKETS_CODE_NAME,
		),
	STRUCT(
		struct __p_mkt_cntr_s *mkt_cntr;
		struct __p_mkt_cond_s *mkt_cond;
		struct __g_code_name_s *code_name;
		),
	STRUCT(
		MDDL_MANY_OF(mkt_status_type, __a_mkt_status_type_t);
		MDDL_MANY_OF(mkt_type, __a_mkt_type_t);
		));

struct __p_mkt_ident_s {
	MDDL_MANY_OF(markets, struct __g_markets_s);
};

struct __p_instr_data_s {
	MDDL_MANY_OF(instr_type, __a_instr_type_t);
	MDDL_MANY_OF(instr_form_type, __a_instr_form_type_t);
	MDDL_MANY_OF(background, __a_background_t);
	MDDL_MANY_OF(comment, __a_comment_t);
	MDDL_MANY_OF(currency, struct __p_currency_s);

	MDDL_MANY_OF(tra_restr_type, struct __p_tra_restr_type_s);
	MDDL_MANY_OF(last_cae, struct __p_last_cae_s);
};

struct __p_issuer_ref_s {
	mduri_t value;
	
	MDDL_MANY_OF(code_name, struct __g_code_name_s);
};

struct __p_issue_date_s {
	mddate_time_t value;

	MDDL_MANY_OF(clsf_date_time, struct __g_clsf_date_time_s);
};

struct __p_clearing_house_s {
	mdstring_t value;

	MDDL_MANY_OF(period, struct __p_period_s);
};

struct __p_clearing_proc_s {
	mdstring_t value;

	MDDL_MANY_OF(period, struct __p_period_s);
};

struct __p_clearing_sys_s {
	mdstring_t value;

	MDDL_MANY_OF(period, struct __p_period_s);
};

struct __p_depo_name_s {
	mdstring_t value;

	MDDL_MANY_OF(period, struct __p_period_s);
};

struct __p_depo_sys_s {
	mdstring_t value;

	MDDL_MANY_OF(period, struct __p_period_s);
};

struct __p_parties_invlv_s {
	MDDL_MANY_OF(agent, struct __p_agent_s);
};

struct __p_agent_s {
	MDDL_MANY_OF(agent_type, __a_agt_type_t);
	MDDL_MANY_OF(party, struct __g_party_s);
};

struct __p_location_s {
	MDDL_MANY_OF(loc_data, struct __g_loc_data_s);
};

struct __p_address_s {
	MDDL_MANY_OF(rank, __a_rank_t);

	mdstring_t value;
};

struct __p_telephone_s {
	MDDL_MANY_OF(telephone_type, __a_telephone_type_t);

	mdstring_t value;
};

struct __p_municipality_s {
	mdstring_t value;

	MDDL_MANY_OF(code_name, struct __g_code_name_s);
};

struct __p_region_s {
	MDDL_MANY_OF(code_name, struct __g_code_name_s);
};

struct __p_state_province_s {
	MDDL_MANY_OF(code_name, struct __g_code_name_s);
};

struct __p_post_code_s {
	MDDL_MANY_OF(descr, __a_descr_t);
	MDDL_MANY_OF(code_type, __a_code_type_t);

	mdstring_t value;

	MDDL_MANY_OF(period, struct __p_period_s);
};

DEFMDDL_GROUP(
	loc_data,
	ENUM(
		MDDL_LOC_DATA_UNK,
		MDDL_LOC_DATA_ADDRESS,
		MDDL_LOC_DATA_MUNICIPALITY,
		MDDL_LOC_DATA_REGION,
		MDDL_LOC_DATA_STATE_OR_PROVINCE,
		MDDL_LOC_DATA_TELEPHONE,
		MDDL_LOC_DATA_PERIOD,
		MDDL_LOC_DATA_POSTAL_CODE,
		),
	STRUCT(
		struct __p_address_s *address;
		struct __p_municipality_s *municipality;
		struct __p_region_s *region;
		struct __p_state_province_s *state_or_province;
		struct __p_telephone_s *telephone;
		struct __p_period_s *period;
		struct __p_post_code_s *postal_code;
		),
	STRUCT(
		MDDL_MANY_OF(country, __a_country_t);
		MDDL_MANY_OF(loc_type, __a_loc_type_t);
		MDDL_MANY_OF(website, __a_website_t);
		));

DEFMDDL_GROUP(
	party,
	ENUM(
		MDDL_PARTY_UNK,
		MDDL_PARTY_INDUS_IDENT,
		MDDL_PARTY_LOCATION,
		MDDL_PARTY_CODE_NAME,
		),
	STRUCT(
		struct __p_indus_ident_s *indus_ident;
		struct __p_location_s *location;
		struct __g_code_name_s *code_name;
		),
	STRUCT(
		MDDL_MANY_OF(entity_type, __a_entity_type_t);
		MDDL_MANY_OF(party_status_type, __a_party_status_type_t);
		));

struct __p_clearing_stlmnt_s {
	MDDL_MANY_OF(settlement_type, __a_stlmnt_type_t);
	MDDL_MANY_OF(mkt_ident, struct __p_mkt_ident_s);
	MDDL_MANY_OF(currency, struct __p_currency_s);

	MDDL_MANY_OF(clearing_house, struct __p_clearing_house_s);
	MDDL_MANY_OF(clearing_process, struct __p_clearing_proc_s);
	MDDL_MANY_OF(clearing_system, struct __p_clearing_sys_s);
	MDDL_MANY_OF(depository_name, struct __p_depo_name_s);
	MDDL_MANY_OF(depository_system, struct __p_depo_sys_s);
	MDDL_MANY_OF(parties_involved, struct __p_parties_invlv_s);
};

struct __p_crossrate_s {
	__a_scheme_t scheme;
	/* the actual contents */
	mdstring_t value;

	MDDL_MANY_OF(multiplier, struct __p_multiplier_s);
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
		struct __g_clsf_rate_t *clsf_rate;
		));

struct __p_issue_amount_s {
	mdprice_t value;

	MDDL_MANY_OF(rank, __a_rank_t);
	MDDL_MANY_OF(clsf_price, struct __g_clsf_price_s);
	MDDL_MANY_OF(debt_indicators_type, __a_debt_ind_type_t);
};

struct __p_issue_fees_s {
	mdprice_t value;

	MDDL_MANY_OF(clsf_price, struct __g_clsf_price_s);
};

struct __p_seg_ident_s {
	MDDL_MANY_OF(seg_type, __a_seg_type_t);
	MDDL_MANY_OF(code_name, struct __g_code_name_s);
};

struct __p_tranche_s {
	MDDL_MANY_OF(nameref, __a_nref_t);
	MDDL_MANY_OF(rank, __a_rank_t);

	__a_tranche_type_t value;
};

DEFMDDL_GROUP(
	sum_price,
	ENUM(
		MDDL_SUM_PRICE_UNK,
		MDDL_SUM_PRICE_CLSF_PRICE,
		MDDL_SUM_PRICE_PERIOD,
		),
	STRUCT(
		struct __g_clsf_price_s *clsf_price;
		struct __p_period_s *period;
		));

DEFMDDL_GROUP(
	sum_amt,
	ENUM(
		MDDL_SUM_AMT_UNK,
		MDDL_SUM_AMT_CLSF_AMT,
		MDDL_SUM_AMT_PERIOD,
		),
	STRUCT(
		struct __g_clsf_amt_s *clsf_amt;
		struct __p_period_s *period;
		));

struct __p_mkt_cap_s {
	MDDL_MANY_OF(cap_type, __a_cap_type_t);
	MDDL_MANY_OF(summarised_amount, struct __g_sum_amt_s);
};


struct __p_instr_ident_s {
	MDDL_MANY_OF(country, __a_country_t);
	MDDL_MANY_OF(instr_status_type, __a_instr_status_type_t);
	MDDL_MANY_OF(scope_type, __a_scope_type_t);
	MDDL_MANY_OF(code_name, struct __g_code_name_s);
	MDDL_MANY_OF(mkt_ident, struct __p_mkt_ident_s);
	MDDL_MANY_OF(instr_data, struct __p_instr_data_s);
	MDDL_MANY_OF(seg_ident, struct __p_seg_ident_s);
	MDDL_MANY_OF(tranche, struct __p_tranche_s);
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

struct __p_mid_s {
	MDDL_MANY_OF(mid_spread, struct __g_mid_spread_s);
};

struct __p_ask_s {
	MDDL_MANY_OF(ask_bid, struct __g_ask_bid_s);
};

struct __p_bid_s {
	MDDL_MANY_OF(ask_bid, struct __g_ask_bid_s);
};

struct __p_spread_s {
	MDDL_MANY_OF(mid_spread, struct __g_mid_spread_s);
};

struct __p_last_s {
	mdprice_t value;
};

struct __p_orderbook_s {
	MDDL_MANY_OF(orderbook_type, __a_obook_type_t);

	MDDL_MANY_OF(bid, struct __p_bid_s);
	MDDL_MANY_OF(ask, struct __p_ask_s);
};

struct __p_yield_s {
	MDDL_MANY_OF(yield_type, __a_yield_type_t);

	mdprice_t value;

	MDDL_MANY_OF(clsf_price, struct __g_clsf_price_s);
};

struct __p_change_s {
	MDDL_MANY_OF(change_direction, __a_change_dir_t);
	MDDL_MANY_OF(change_type, __a_change_type_t);

	mdprice_t value;

	MDDL_MANY_OF(summarised_price, struct __g_sum_price_s);
};

struct __p_open_s {
	MDDL_MANY_OF(open_type, __a_open_type_t);

	mdprice_t value;

	MDDL_MANY_OF(summarised_price, struct __g_sum_price_s);
};

struct __p_low_s {
	mdprice_t value;

	MDDL_MANY_OF(summarised_price, struct __g_sum_price_s);
};

struct __p_high_s {
	mdprice_t value;

	MDDL_MANY_OF(summarised_price, struct __g_sum_price_s);
};

struct __p_close_s {
	MDDL_MANY_OF(close_type, __a_close_type_t);

	mdprice_t value;

	MDDL_MANY_OF(clsf_price, struct __g_clsf_price_s);
};

DEFMDDL_GROUP(
	mid_spread,
	ENUM(
		MDDL_MID_SPREAD_UNK,
		MDDL_MID_SPREAD_ASK_BID,
		),
	STRUCT(
		struct __g_ask_bid_s *ask_bid;
		));

DEFMDDL_GROUP(
	ask_bid,
	ENUM(
		MDDL_ASK_BID_UNK,
		MDDL_ASK_BID_BASIC_PRICE,
		MDDL_ASK_BID_YIELD,
		MDDL_ASK_BID_LIMITS,
		),
	STRUCT(
		struct __g_basic_price_s *basic_price;
		struct __p_yield_s *yield;
		struct __g_limits_s *limits;
		),
	STRUCT(
		MDDL_MANY_OF(rank, __a_rank_t);
		));

DEFMDDL_GROUP(
	basic_price,
	ENUM(
		MDDL_BASIC_PRICE_UNK,
		MDDL_BASIC_PRICE_MKT_IDENT,
		MDDL_BASIC_PRICE_LAST
		),
	STRUCT(
		struct __p_mkt_ident_s *mkt_ident;
		struct __p_last_s *last;
		));

DEFMDDL_GROUP(
	limits,
	ENUM(
		MDDL_LIMITS_UNK,
		MDDL_LIMITS_CHANGE,
		MDDL_LIMITS_OPEN,
		MDDL_LIMITS_HIGH,
		MDDL_LIMITS_LOW,
		MDDL_LIMITS_CLOSE,
		),
	STRUCT(
		struct __p_change_s *change;
		struct __p_open_s *open;
		struct __p_high_s *high;
		struct __p_low_s *low;
		struct __p_close_s *close;
		));

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

DEFMDDL_GROUP(
	basic_quotes,
	ENUM(
		MDDL_BASIC_QUOTES_UNK,
		MDDL_BASIC_QUOTES_MID,
		MDDL_BASIC_QUOTES_ASK,
		MDDL_BASIC_QUOTES_BID,
		MDDL_BASIC_QUOTES_SPREAD,
		MDDL_BASIC_QUOTES_ORDERBOOK,
		),
	STRUCT(
		struct __p_mid_s *mid;
		struct __p_ask_s *ask;
		struct __p_bid_s *bid;
		struct __p_spread_s *spread;
		struct __p_orderbook_s *orderbook;
		));

struct __p_objective_s {
	MDDL_MANY_OF(objective_type, __a_objctv_type_t);

	mdstring_t value;
};

DEFMDDL_GROUP(
	entity_grp,
	ENUM(
		MDDL_ENTITY_GRP_UNK,
		),
	STRUCT(
		),
	STRUCT(
		));

struct __p_issuer_s {
	MDDL_MANY_OF(comment, __a_comment_t);

	MDDL_MANY_OF(instr_ident, MDDL_PROP(instr_ident));
	MDDL_MANY_OF(mkt_cap, MDDL_PROP(mkt_cap));
	MDDL_MANY_OF(entity_grp, MDDL_GROUP(entity_grp));
};	

DECLP_ADD_PROPF(issuer, instr_ident);
DECLP_ADD_PROPF(issuer, mkt_cap);
DECLP_ADD_GROUPF(issuer, entity_grp);


/* domains */
struct __dom_instr_s {
	MDDL_MANY_OF(basic_idents, struct __g_basic_idents_s);
	MDDL_MANY_OF(basic_quotes, struct __g_basic_quotes_s);

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
		),
	STRUCT(
		MDDL_MANY_OF(date_time, mddate_time_t);
		MDDL_MANY_OF(snap_type, __a_snap_type_t);
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
