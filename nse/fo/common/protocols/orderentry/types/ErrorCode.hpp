#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// error_code
struct error_code {

    enum class enum_type : std::int16_t {
        invalid_instrument_type = 293,
        order_number_invalid = 509,
        ord_cxl_initiator_auc_not_allowed = 8049,
        auction_number_invalid = 8485,
        market_closed = 16000,
        e_invalid_user = 16001,
        error_bad_trans_code = 16003,
        e_user_already_signed_on = 16004,
        e_invalid_signoff = 16005,
        e_invalid_signon = 16006,
        e_signon_not_possible = 16007,
        err_invalid_symbol = 16012,
        err_invalid_order_number = 16013,
        e_not_your_order = 16014,
        e_not_your_fill = 16015,
        e_invalid_fill_number = 16016,
        e_stock_not_found = 16019,
        e_order_price_out_of_revised_price_ra = 16020,
        security_not_available = 16035,
        broker_not_found = 16041,
        user_not_found = 16042,
        duplicate_record = 16043,
        e_order_modified = 16044,
        stock_suspended = 16049,
        err_function_not_available = 16052,
        e_change_password = 16053,
        err_invalid_branch = 16054,
        oe_program_error = 16056,
        err_invalid_status = 16063,
        err_data_not_changed = 16070,
        e_dup_trd_cxl_request = 16086,
        err_invalid_buyer_user_id = 16098,
        err_invalid_seller_user_id = 16099,
        e_invalid_version = 16100,
        oe_system_error = 16104,
        err_user_disabled = 16134,
        oe_invalid_stock_status = 16145,
        err_invalid_user_id = 16148,
        err_invalid_trader_id = 16154,
        oe_ato_in_open = 16169,
        e_dup_request = 16198,
        e_only_cp_allowed = 16227,
        e_sl_mit_nt_not_allowed_pclose = 16228,
        e_gtc_gtd_ord_not_allowed_pclose = 16229,
        oe_cont_mod_not_allowed = 16230,
        trd_cont_mod_not_allowed = 16231,
        str_pro_partivipant_invalid = 16233,
        error_invalid_price = 16247,
        oe_diff_trd_mod_vol = 16251,
        error_user_not_exists_in_system = 16260,
        err_already_deleted = 16264,
        record_not_found = 16273,
        oe_markets_closed = 16278,
        oe_security_not_admitted = 16279,
        oe_security_matured = 16280,
        oe_security_expelled = 16281,
        oe_issued_cap_exceeds = 16282,
        oe_price_not_mult = 16283,
        oe_price_exceeds_day_min_max = 16284,
        oe_is_not_active = 16285,
        e_system_wrong_state = 16300,
        oe_auction_pending = 16303,
        oe_qty_freeze_can = 16307,
        oe_price_freeze_can = 16308,
        oe_sol_period_over = 16311,
        oe_comp_period_over = 16312,
        oe_auc_period_greater = 16313,
        oe_limit_trigger = 16315,
        oe_trigger_price_not_mult = 16316,
        oe_no_aon_attrib = 16317,
        oe_no_mf_attrib = 16318,
        oe_no_aon_in_attrib_1 = 16319,
        oe_no_mf_attrib_1 = 16320,
        oe_mf_greater_disc = 16321,
        oe_mf_not_mult = 16322,
        oe_mf_greater_original = 16323,
        oe_disc_greater_original = 16324,
        oe_disc_not_mult = 16325,
        oe_gtd_greater = 16326,
        oe_quantity_gerater_rl = 16327,
        oe_quantity_not_mult_rl = 16328,
        oe_broker_not_permitted = 16329,
        oe_is_suspended = 16330,
        oe_branch_li_mit_exceeded = 16333,
        oe_ord_can_changed = 16343,
        oe_ord_cannot_cancel = 16344,
        oe_init_ord_cancel = 16345,
        oe_ord_cannot_modify = 16346,
        err_trading_not_allowed = 16348,
        oe_nt_rejected = 16357,
        chg_st_exists = 16363,
        oe_security_in_preopen = 16369,
        oe_inq_not_allowed = 16372,
        oe_security_ineligible = 16387,
        e_fok_order_cancelled = 16388,
        turnover_limit_not_provided = 16392,
        err_cannot_mod_auc_order = 16397,
        oe_max_dq_allowed = 16400,
        oe_admin_susp_can = 16404,
        e_invalid_buy_sell_type = 16405,
        e_invalid_book_type = 16406,
        e_invalid_trigger_price = 16408,
        e_invalid_pro_client = 16414,
        e_invalid_instructions = 16415,
        e_invalid_order_parameters = 16416,
        e_nnf_req_exceeded = 16418,
        invalid_order = 16419,
        err_box_rate_exceeded_at_millisecond_level = 16420,
        e_gtd_gt_maturity = 16440,
        dq_not_allowed_in_preopen = 16441,
        st_ord_not_allowed_popen = 16442,
        e_ord_lim_exceeds_ord_val_lim = 16443,
        err_usr_ord_value_limit_exceeded = 16444,
        sl_not_allowed = 16445,
        mit_not_allowed = 16446,
        e_ord_not_allowed_in_preopen = 16447,
        error_sl_lmt_rsnblty_check = 16448,
        e_not_modifiable = 16514,
        e_tm_cm_does_not_exist = 16518,
        e_not_clg_mem = 16521,
        e_user_not_corp_mgr = 16523,
        e_pm_cm_invalid = 16532,
        e_corp_mgr_vu_mod = 16533,
        e_invalid_participant = 16541,
        e_trade_approved_by_cm = 16550,
        e_cm_stock_suspended = 16552,
        e_broker_not_permitted_in_fut = 16554,
        e_broker_not_permitted_in_opt = 16555,
        e_qty_less_than_min_lot = 16556,
        e_disc_qty_less_than_min_lot = 16557,
        e_mf_qty_less_than_min_lot = 16558,
        e_already_rejected = 16560,
        e_nt_orders_not_allowed = 16561,
        e_nt_trade_not_allowed = 16562,
        e_inconsistent_broker_branch = 16566,
        m_post_close_start = 16570,
        m_post_close_ended = 16571,
        m_post_close_trades = 16572,
        e_invalid_msg_length = 16573,
        e_invalid_open_close_type = 16574,
        e_nnf_inq_req_exceeded = 16576,
        e_participant_and_volume_changed = 16577,
        e_invalid_cover_uncover_type = 16578,
        e_illegal_participant = 16580,
        e_invalid_fill_price = 16581,
        e_pro_no_participant = 16583,
        e_invalid_account_no = 16585,
        e_allow_no_participant_order = 16586,
        m_delete_all_orders = 16589,
        e_cum_ur_ord_val_limit_exceede = 16597,
        e_branch_ord_val_limit_exceeded = 16598,
        err_ord_val_exceeded = 16600,
        err_preopen_order_reject = 16601,
        e_dealer_value_limit_exceeds = 16602,
        e_participant_not_found = 16604,
        e_either_leg_failed = 16605,
        e_qty_greater_than_freeze_qty = 16606,
        e_spread_not_allowed = 16607,
        e_spread_allowed_if_stock_open = 16609,
        e_qty_should_be_same = 16610,
        e_ord_mod_qty_frz_not_allowed = 16611,
        e_trade_rec_modified = 16612,
        e_tm_order_cant_be_modified = 16615,
        e_tm_order_cant_be_cancelled = 16616,
        e_tm_trade_cant_be_manipulated = 16617,
        e_cm_of_tm_suspended = 16625,
        e_expdate_not_in_ascending_ord = 16626,
        e_invalid_contract_comb = 16627,
        e_bm_cannot_cancel_cm_orders = 16628,
        e_bm_cannot_cancel_bm_orders = 16629,
        e_cm_cannot_cancel_cm_orders = 16630,
        e_spread_in_different_underlying = 16631,
        e_invalid_cli_ac = 16632,
        e_br_ord_limit_fut_buy_exceeded = 16636,
        e_br_ord_limit_fut_sell_exceeded = 16637,
        e_br_ord_limit_opt_buy_exceeded = 16638,
        e_br_ord_limit_opt_sell_exceeded = 16639,
        e_ur_ord_limit_fut_buy_exceeded = 16640,
        e_ur_ord_limit_fut_sell_exceeded = 16641,
        e_ur_ord_limit_opt_buy_exceeded = 16642,
        e_ur_ord_limit_opt_sell_exceeded = 16643,
        e_cant_appr_bhav_copy_generated = 16645,
        e_collateral_lmt_chk = 16646,
        e_address_not_found = 16656,
        e_stk_in_popen = 16662,
        e_invalid_nnf_field = 16666,
        e_gtcgtd_not_allowed = 16667,
        err_user_already_signed_off = 16683,
        err_no_privilege = 16684,
        closeout_order_reject = 16686,
        closeout_frz_reject = 16687,
        closeout_not_allowed = 16688,
        closeout_trdmod_reject = 16690,
        partial_order_reject = 16706,
        partial_quick_order_cxl_rej = 16708,
        error_invalid_sprd_combination = 16711,
        e_price_diff_out_of_range = 16713,
        rms_rejected_in_preopen = 16725,
        error_algoid_nnfid_mismatch_1 = 16730,
        error_algoid_nnfid_mismatch_2 = 16731,
        error_algo_mkt_not_allowed = 16732,
        error_invalid_nnf_id = 16733,
        error_preopn_ato_mod_can_rej = 16749,
        error_preopn_ato_not_allowed = 16752,
        err_usr_not_found_in_nnf_file = 16778,
        e_vc_order_rejected = 16793,
        e_ssd_order_rejected = 16794,
        e_order_cancelled_for_vc = 16795,
        e_order_cancelled_for_ssd = 16796,
        msg_code_voluntary_close_out_status = 16797,
        msg_code_suspended_status = 16798,
        e_bo_price_out_of_range = 16803,
        e_bo_excess_quantity = 16804,
        e_user_ineligible_for_bulk_orders = 16805,
        e_user_not_allowed_for_regular = 16806,
        e_account_debarred = 16807,
        e_account_debarred_by_pit = 16816,
        err_usr_already_unlcked = 16810,
        err_duplicate_unlck_alrt = 16811,
        err_actv_num_of_usrs_in_brnch_exceeded = 17022,
        ec_trd_mod_rej_cli_cp_mod_not_allowed = 17039,
        error_quantity_lim_exceeds_qty_val_lim = 17045,
        user_trd_mod_disabled = 17046,
        preopen_trade_cancellation_not_allowed = 16055,
        err_depndent_sessn_not_active = 17063,
        e_trd_price_out_of_stock_tpp = 17070,
        e_order_cancelled_for_self_trade = 17071,
        e_invalid_packet = 17101,
        sssssse_hearbeat_not_received = 17102,
        e_invalid_box_id = 17104,
        e_seq_no_mismatch = 17105,
        e_box_rate_exceeded = 17106,
        error_hb_rate_exceeded = 17107,
        e_max_user_count_exceeded = 17142,
        e_invalid_box_ip_combination = 16403,
        err_invalid_pan_id = 17177,
        err_invalid_algo_id = 17179,
        err_invalid_value_in_reserved = 17180,
        err_algo_id_disabled = 17185,
        err_order_cancelled_algoid_disabled = 17186,
        err_checksum_failed_gr = 19028,
        err_multiple_gr_query_rcv = 19029,
        err_mkt_order_not_allowed = 17181,
        err_trade_beyond_markup_price = 17182,
        err_encryption_flag_mismatch = 19030,
        err_md_5_checksum_failure = 19031,
        err_user_having_null_rights = 17184
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 247> from_string_map = {{
        {"Auction Number Invalid", enum_type::auction_number_invalid},
        {"Broker Not Found", enum_type::broker_not_found},
        {"Chg St Exists", enum_type::chg_st_exists},
        {"Closeout Frz Reject", enum_type::closeout_frz_reject},
        {"Closeout Not Allowed", enum_type::closeout_not_allowed},
        {"Closeout Order Reject", enum_type::closeout_order_reject},
        {"Closeout Trdmod Reject", enum_type::closeout_trdmod_reject},
        {"Dq Not Allowed In Preopen", enum_type::dq_not_allowed_in_preopen},
        {"Duplicate Record", enum_type::duplicate_record},
        {"E Account Debarred", enum_type::e_account_debarred},
        {"E Account Debarred By Pit", enum_type::e_account_debarred_by_pit},
        {"E Address Not Found", enum_type::e_address_not_found},
        {"E Allow No Participant Order", enum_type::e_allow_no_participant_order},
        {"E Already Rejected", enum_type::e_already_rejected},
        {"E Bm Cannot Cancel Bm Orders", enum_type::e_bm_cannot_cancel_bm_orders},
        {"E Bm Cannot Cancel Cm Orders", enum_type::e_bm_cannot_cancel_cm_orders},
        {"E Bo Excess Quantity", enum_type::e_bo_excess_quantity},
        {"E Bo Price Out Of Range", enum_type::e_bo_price_out_of_range},
        {"E Box Rate Exceeded", enum_type::e_box_rate_exceeded},
        {"E Br Ord Limit Fut Buy Exceeded", enum_type::e_br_ord_limit_fut_buy_exceeded},
        {"E Br Ord Limit Fut Sell Exceeded", enum_type::e_br_ord_limit_fut_sell_exceeded},
        {"E Br Ord Limit Opt Buy Exceeded", enum_type::e_br_ord_limit_opt_buy_exceeded},
        {"E Br Ord Limit Opt Sell Exceeded", enum_type::e_br_ord_limit_opt_sell_exceeded},
        {"E Branch Ord Val Limit Exceeded", enum_type::e_branch_ord_val_limit_exceeded},
        {"E Broker Not Permitted In Fut", enum_type::e_broker_not_permitted_in_fut},
        {"E Broker Not Permitted In Opt", enum_type::e_broker_not_permitted_in_opt},
        {"E Cant Appr Bhav Copy Generated", enum_type::e_cant_appr_bhav_copy_generated},
        {"E Change Password", enum_type::e_change_password},
        {"E Cm Cannot Cancel Cm Orders", enum_type::e_cm_cannot_cancel_cm_orders},
        {"E Cm Of Tm Suspended", enum_type::e_cm_of_tm_suspended},
        {"E Cm Stock Suspended", enum_type::e_cm_stock_suspended},
        {"E Collateral Lmt Chk", enum_type::e_collateral_lmt_chk},
        {"E Corp Mgr Vu Mod", enum_type::e_corp_mgr_vu_mod},
        {"E Cum Ur Ord Val Limit Exceede", enum_type::e_cum_ur_ord_val_limit_exceede},
        {"E Dealer Value Limit Exceeds", enum_type::e_dealer_value_limit_exceeds},
        {"E Disc Qty Less Than Min Lot", enum_type::e_disc_qty_less_than_min_lot},
        {"E Dup Request", enum_type::e_dup_request},
        {"E Dup Trd Cxl Request", enum_type::e_dup_trd_cxl_request},
        {"E Either Leg Failed", enum_type::e_either_leg_failed},
        {"E Expdate Not In Ascending Ord", enum_type::e_expdate_not_in_ascending_ord},
        {"E Fok Order Cancelled", enum_type::e_fok_order_cancelled},
        {"E Gtc Gtd Ord Not Allowed Pclose", enum_type::e_gtc_gtd_ord_not_allowed_pclose},
        {"E Gtcgtd Not Allowed", enum_type::e_gtcgtd_not_allowed},
        {"E Gtd Gt Maturity", enum_type::e_gtd_gt_maturity},
        {"E Illegal Participant", enum_type::e_illegal_participant},
        {"E Inconsistent Broker Branch", enum_type::e_inconsistent_broker_branch},
        {"E Invalid Account No", enum_type::e_invalid_account_no},
        {"E Invalid Book Type", enum_type::e_invalid_book_type},
        {"E Invalid Box Id", enum_type::e_invalid_box_id},
        {"E Invalid Box Ip Combination", enum_type::e_invalid_box_ip_combination},
        {"E Invalid Buy Sell Type", enum_type::e_invalid_buy_sell_type},
        {"E Invalid Cli Ac", enum_type::e_invalid_cli_ac},
        {"E Invalid Contract Comb", enum_type::e_invalid_contract_comb},
        {"E Invalid Cover Uncover Type", enum_type::e_invalid_cover_uncover_type},
        {"E Invalid Fill Number", enum_type::e_invalid_fill_number},
        {"E Invalid Fill Price", enum_type::e_invalid_fill_price},
        {"E Invalid Instructions", enum_type::e_invalid_instructions},
        {"E Invalid Msg Length", enum_type::e_invalid_msg_length},
        {"E Invalid Nnf Field", enum_type::e_invalid_nnf_field},
        {"E Invalid Open Close Type", enum_type::e_invalid_open_close_type},
        {"E Invalid Order Parameters", enum_type::e_invalid_order_parameters},
        {"E Invalid Packet", enum_type::e_invalid_packet},
        {"E Invalid Participant", enum_type::e_invalid_participant},
        {"E Invalid Pro Client", enum_type::e_invalid_pro_client},
        {"E Invalid Signoff", enum_type::e_invalid_signoff},
        {"E Invalid Signon", enum_type::e_invalid_signon},
        {"E Invalid Trigger Price", enum_type::e_invalid_trigger_price},
        {"E Invalid User", enum_type::e_invalid_user},
        {"E Invalid Version", enum_type::e_invalid_version},
        {"E Max User Count Exceeded", enum_type::e_max_user_count_exceeded},
        {"E Mf Qty Less Than Min Lot", enum_type::e_mf_qty_less_than_min_lot},
        {"E Nnf Inq Req Exceeded", enum_type::e_nnf_inq_req_exceeded},
        {"E Nnf Req Exceeded", enum_type::e_nnf_req_exceeded},
        {"E Not Clg Mem", enum_type::e_not_clg_mem},
        {"E Not Modifiable", enum_type::e_not_modifiable},
        {"E Not Your Fill", enum_type::e_not_your_fill},
        {"E Not Your Order", enum_type::e_not_your_order},
        {"E Nt Orders Not Allowed", enum_type::e_nt_orders_not_allowed},
        {"E Nt Trade Not Allowed", enum_type::e_nt_trade_not_allowed},
        {"E Only Cp Allowed", enum_type::e_only_cp_allowed},
        {"E Ord Lim Exceeds Ord Val Lim", enum_type::e_ord_lim_exceeds_ord_val_lim},
        {"E Ord Mod Qty Frz Not Allowed", enum_type::e_ord_mod_qty_frz_not_allowed},
        {"E Ord Not Allowed In Preopen", enum_type::e_ord_not_allowed_in_preopen},
        {"E Order Cancelled For Self Trade", enum_type::e_order_cancelled_for_self_trade},
        {"E Order Cancelled For Ssd", enum_type::e_order_cancelled_for_ssd},
        {"E Order Cancelled For Vc", enum_type::e_order_cancelled_for_vc},
        {"E Order Modified", enum_type::e_order_modified},
        {"E Order Price Out Of Revised Price Ra", enum_type::e_order_price_out_of_revised_price_ra},
        {"E Participant And Volume Changed", enum_type::e_participant_and_volume_changed},
        {"E Participant Not Found", enum_type::e_participant_not_found},
        {"E Pm Cm Invalid", enum_type::e_pm_cm_invalid},
        {"E Price Diff Out Of Range", enum_type::e_price_diff_out_of_range},
        {"E Pro No Participant", enum_type::e_pro_no_participant},
        {"E Qty Greater Than Freeze Qty", enum_type::e_qty_greater_than_freeze_qty},
        {"E Qty Less Than Min Lot", enum_type::e_qty_less_than_min_lot},
        {"E Qty Should Be Same", enum_type::e_qty_should_be_same},
        {"E Seq No Mismatch", enum_type::e_seq_no_mismatch},
        {"E Signon Not Possible", enum_type::e_signon_not_possible},
        {"E Sl Mit Nt Not Allowed Pclose", enum_type::e_sl_mit_nt_not_allowed_pclose},
        {"E Spread Allowed If Stock Open", enum_type::e_spread_allowed_if_stock_open},
        {"E Spread In Different Underlying", enum_type::e_spread_in_different_underlying},
        {"E Spread Not Allowed", enum_type::e_spread_not_allowed},
        {"E Ssd Order Rejected", enum_type::e_ssd_order_rejected},
        {"E Stk In Popen", enum_type::e_stk_in_popen},
        {"E Stock Not Found", enum_type::e_stock_not_found},
        {"E System Wrong State", enum_type::e_system_wrong_state},
        {"E Tm Cm Does Not Exist", enum_type::e_tm_cm_does_not_exist},
        {"E Tm Order Cant Be Cancelled", enum_type::e_tm_order_cant_be_cancelled},
        {"E Tm Order Cant Be Modified", enum_type::e_tm_order_cant_be_modified},
        {"E Tm Trade Cant Be Manipulated", enum_type::e_tm_trade_cant_be_manipulated},
        {"E Trade Approved By Cm", enum_type::e_trade_approved_by_cm},
        {"E Trade Rec Modified", enum_type::e_trade_rec_modified},
        {"E Trd Price Out Of Stock Tpp", enum_type::e_trd_price_out_of_stock_tpp},
        {"E Ur Ord Limit Fut Buy Exceeded", enum_type::e_ur_ord_limit_fut_buy_exceeded},
        {"E Ur Ord Limit Fut Sell Exceeded", enum_type::e_ur_ord_limit_fut_sell_exceeded},
        {"E Ur Ord Limit Opt Buy Exceeded", enum_type::e_ur_ord_limit_opt_buy_exceeded},
        {"E Ur Ord Limit Opt Sell Exceeded", enum_type::e_ur_ord_limit_opt_sell_exceeded},
        {"E User Already Signed On", enum_type::e_user_already_signed_on},
        {"E User Ineligible For Bulk Orders", enum_type::e_user_ineligible_for_bulk_orders},
        {"E User Not Allowed For Regular", enum_type::e_user_not_allowed_for_regular},
        {"E User Not Corp Mgr", enum_type::e_user_not_corp_mgr},
        {"E Vc Order Rejected", enum_type::e_vc_order_rejected},
        {"Ec Trd Mod Rej Cli Cp Mod Not Allowed", enum_type::ec_trd_mod_rej_cli_cp_mod_not_allowed},
        {"Err Actv Num Of Usrs In Brnch Exceeded", enum_type::err_actv_num_of_usrs_in_brnch_exceeded},
        {"Err Algo Id Disabled", enum_type::err_algo_id_disabled},
        {"Err Already Deleted", enum_type::err_already_deleted},
        {"Err Box Rate Exceeded At Millisecond Level", enum_type::err_box_rate_exceeded_at_millisecond_level},
        {"Err Cannot Mod Auc Order", enum_type::err_cannot_mod_auc_order},
        {"Err Checksum Failed Gr", enum_type::err_checksum_failed_gr},
        {"Err Data Not Changed", enum_type::err_data_not_changed},
        {"Err Depndent Sessn Not Active", enum_type::err_depndent_sessn_not_active},
        {"Err Duplicate Unlck Alrt", enum_type::err_duplicate_unlck_alrt},
        {"Err Encryption Flag Mismatch", enum_type::err_encryption_flag_mismatch},
        {"Err Function Not Available", enum_type::err_function_not_available},
        {"Err Invalid Algo Id", enum_type::err_invalid_algo_id},
        {"Err Invalid Branch", enum_type::err_invalid_branch},
        {"Err Invalid Buyer User Id", enum_type::err_invalid_buyer_user_id},
        {"Err Invalid Order Number", enum_type::err_invalid_order_number},
        {"Err Invalid Pan Id", enum_type::err_invalid_pan_id},
        {"Err Invalid Seller User Id", enum_type::err_invalid_seller_user_id},
        {"Err Invalid Status", enum_type::err_invalid_status},
        {"Err Invalid Symbol", enum_type::err_invalid_symbol},
        {"Err Invalid Trader Id", enum_type::err_invalid_trader_id},
        {"Err Invalid User Id", enum_type::err_invalid_user_id},
        {"Err Invalid Value In Reserved", enum_type::err_invalid_value_in_reserved},
        {"Err Md 5 Checksum Failure", enum_type::err_md_5_checksum_failure},
        {"Err Mkt Order Not Allowed", enum_type::err_mkt_order_not_allowed},
        {"Err Multiple Gr Query Rcv", enum_type::err_multiple_gr_query_rcv},
        {"Err No Privilege", enum_type::err_no_privilege},
        {"Err Ord Val Exceeded", enum_type::err_ord_val_exceeded},
        {"Err Order Cancelled Algoid Disabled", enum_type::err_order_cancelled_algoid_disabled},
        {"Err Preopen Order Reject", enum_type::err_preopen_order_reject},
        {"Err Trade Beyond Markup Price", enum_type::err_trade_beyond_markup_price},
        {"Err Trading Not Allowed", enum_type::err_trading_not_allowed},
        {"Err User Already Signed Off", enum_type::err_user_already_signed_off},
        {"Err User Disabled", enum_type::err_user_disabled},
        {"Err User Having Null Rights", enum_type::err_user_having_null_rights},
        {"Err Usr Already Unlcked", enum_type::err_usr_already_unlcked},
        {"Err Usr Not Found In Nnf File", enum_type::err_usr_not_found_in_nnf_file},
        {"Err Usr Ord Value Limit Exceeded", enum_type::err_usr_ord_value_limit_exceeded},
        {"Error Algo Mkt Not Allowed", enum_type::error_algo_mkt_not_allowed},
        {"Error Algoid Nnfid Mismatch 1", enum_type::error_algoid_nnfid_mismatch_1},
        {"Error Algoid Nnfid Mismatch 2", enum_type::error_algoid_nnfid_mismatch_2},
        {"Error Bad Trans Code", enum_type::error_bad_trans_code},
        {"Error Hb Rate Exceeded", enum_type::error_hb_rate_exceeded},
        {"Error Invalid Nnf Id", enum_type::error_invalid_nnf_id},
        {"Error Invalid Price", enum_type::error_invalid_price},
        {"Error Invalid Sprd Combination", enum_type::error_invalid_sprd_combination},
        {"Error Preopn Ato Mod Can Rej", enum_type::error_preopn_ato_mod_can_rej},
        {"Error Preopn Ato Not Allowed", enum_type::error_preopn_ato_not_allowed},
        {"Error Quantity Lim Exceeds Qty Val Lim", enum_type::error_quantity_lim_exceeds_qty_val_lim},
        {"Error Sl Lmt Rsnblty Check", enum_type::error_sl_lmt_rsnblty_check},
        {"Error User Not Exists In System", enum_type::error_user_not_exists_in_system},
        {"Invalid Instrument Type", enum_type::invalid_instrument_type},
        {"Invalid Order", enum_type::invalid_order},
        {"M Delete All Orders", enum_type::m_delete_all_orders},
        {"M Post Close Ended", enum_type::m_post_close_ended},
        {"M Post Close Start", enum_type::m_post_close_start},
        {"M Post Close Trades", enum_type::m_post_close_trades},
        {"Market Closed", enum_type::market_closed},
        {"Mit Not Allowed", enum_type::mit_not_allowed},
        {"Msg Code Suspended Status", enum_type::msg_code_suspended_status},
        {"Msg Code Voluntary Close Out Status", enum_type::msg_code_voluntary_close_out_status},
        {"Oe Admin Susp Can", enum_type::oe_admin_susp_can},
        {"Oe Ato In Open", enum_type::oe_ato_in_open},
        {"Oe Auc Period Greater", enum_type::oe_auc_period_greater},
        {"Oe Auction Pending", enum_type::oe_auction_pending},
        {"Oe Branch Li Mit Exceeded", enum_type::oe_branch_li_mit_exceeded},
        {"Oe Broker Not Permitted", enum_type::oe_broker_not_permitted},
        {"Oe Comp Period Over", enum_type::oe_comp_period_over},
        {"Oe Cont Mod Not Allowed", enum_type::oe_cont_mod_not_allowed},
        {"Oe Diff Trd Mod Vol", enum_type::oe_diff_trd_mod_vol},
        {"Oe Disc Greater Original", enum_type::oe_disc_greater_original},
        {"Oe Disc Not Mult", enum_type::oe_disc_not_mult},
        {"Oe Gtd Greater", enum_type::oe_gtd_greater},
        {"Oe Init Ord Cancel", enum_type::oe_init_ord_cancel},
        {"Oe Inq Not Allowed", enum_type::oe_inq_not_allowed},
        {"Oe Invalid Stock Status", enum_type::oe_invalid_stock_status},
        {"Oe Is Not Active", enum_type::oe_is_not_active},
        {"Oe Is Suspended", enum_type::oe_is_suspended},
        {"Oe Issued Cap Exceeds", enum_type::oe_issued_cap_exceeds},
        {"Oe Limit Trigger", enum_type::oe_limit_trigger},
        {"Oe Markets Closed", enum_type::oe_markets_closed},
        {"Oe Max Dq Allowed", enum_type::oe_max_dq_allowed},
        {"Oe Mf Greater Disc", enum_type::oe_mf_greater_disc},
        {"Oe Mf Greater Original", enum_type::oe_mf_greater_original},
        {"Oe Mf Not Mult", enum_type::oe_mf_not_mult},
        {"Oe No Aon Attrib", enum_type::oe_no_aon_attrib},
        {"Oe No Aon In Attrib 1", enum_type::oe_no_aon_in_attrib_1},
        {"Oe No Mf Attrib", enum_type::oe_no_mf_attrib},
        {"Oe No Mf Attrib 1", enum_type::oe_no_mf_attrib_1},
        {"Oe Nt Rejected", enum_type::oe_nt_rejected},
        {"Oe Ord Can Changed", enum_type::oe_ord_can_changed},
        {"Oe Ord Cannot Cancel", enum_type::oe_ord_cannot_cancel},
        {"Oe Ord Cannot Modify", enum_type::oe_ord_cannot_modify},
        {"Oe Price Exceeds Day Min Max", enum_type::oe_price_exceeds_day_min_max},
        {"Oe Price Freeze Can", enum_type::oe_price_freeze_can},
        {"Oe Price Not Mult", enum_type::oe_price_not_mult},
        {"Oe Program Error", enum_type::oe_program_error},
        {"Oe Qty Freeze Can", enum_type::oe_qty_freeze_can},
        {"Oe Quantity Gerater Rl", enum_type::oe_quantity_gerater_rl},
        {"Oe Quantity Not Mult Rl", enum_type::oe_quantity_not_mult_rl},
        {"Oe Security Expelled", enum_type::oe_security_expelled},
        {"Oe Security In Preopen", enum_type::oe_security_in_preopen},
        {"Oe Security Ineligible", enum_type::oe_security_ineligible},
        {"Oe Security Matured", enum_type::oe_security_matured},
        {"Oe Security Not Admitted", enum_type::oe_security_not_admitted},
        {"Oe Sol Period Over", enum_type::oe_sol_period_over},
        {"Oe System Error", enum_type::oe_system_error},
        {"Oe Trigger Price Not Mult", enum_type::oe_trigger_price_not_mult},
        {"Ord Cxl Initiator Auc Not Allowed", enum_type::ord_cxl_initiator_auc_not_allowed},
        {"Order Number Invalid", enum_type::order_number_invalid},
        {"Partial Order Reject", enum_type::partial_order_reject},
        {"Partial Quick Order Cxl Rej", enum_type::partial_quick_order_cxl_rej},
        {"Preopen Trade Cancellation Not Allowed", enum_type::preopen_trade_cancellation_not_allowed},
        {"Record Not Found", enum_type::record_not_found},
        {"Rms Rejected In Preopen", enum_type::rms_rejected_in_preopen},
        {"Security Not Available", enum_type::security_not_available},
        {"Sl Not Allowed", enum_type::sl_not_allowed},
        {"Sssssse Hearbeat Not Received", enum_type::sssssse_hearbeat_not_received},
        {"St Ord Not Allowed Popen", enum_type::st_ord_not_allowed_popen},
        {"Stock Suspended", enum_type::stock_suspended},
        {"Str Pro Partivipant Invalid", enum_type::str_pro_partivipant_invalid},
        {"Trd Cont Mod Not Allowed", enum_type::trd_cont_mod_not_allowed},
        {"Turnover Limit Not Provided", enum_type::turnover_limit_not_provided},
        {"User Not Found", enum_type::user_not_found},
        {"User Trd Mod Disabled", enum_type::user_trd_mod_disabled}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::invalid_instrument_type: return "Invalid Instrument Type";
            case enum_type::order_number_invalid: return "Order Number Invalid";
            case enum_type::ord_cxl_initiator_auc_not_allowed: return "Ord Cxl Initiator Auc Not Allowed";
            case enum_type::auction_number_invalid: return "Auction Number Invalid";
            case enum_type::market_closed: return "Market Closed";
            case enum_type::e_invalid_user: return "E Invalid User";
            case enum_type::error_bad_trans_code: return "Error Bad Trans Code";
            case enum_type::e_user_already_signed_on: return "E User Already Signed On";
            case enum_type::e_invalid_signoff: return "E Invalid Signoff";
            case enum_type::e_invalid_signon: return "E Invalid Signon";
            case enum_type::e_signon_not_possible: return "E Signon Not Possible";
            case enum_type::err_invalid_symbol: return "Err Invalid Symbol";
            case enum_type::err_invalid_order_number: return "Err Invalid Order Number";
            case enum_type::e_not_your_order: return "E Not Your Order";
            case enum_type::e_not_your_fill: return "E Not Your Fill";
            case enum_type::e_invalid_fill_number: return "E Invalid Fill Number";
            case enum_type::e_stock_not_found: return "E Stock Not Found";
            case enum_type::e_order_price_out_of_revised_price_ra: return "E Order Price Out Of Revised Price Ra";
            case enum_type::security_not_available: return "Security Not Available";
            case enum_type::broker_not_found: return "Broker Not Found";
            case enum_type::user_not_found: return "User Not Found";
            case enum_type::duplicate_record: return "Duplicate Record";
            case enum_type::e_order_modified: return "E Order Modified";
            case enum_type::stock_suspended: return "Stock Suspended";
            case enum_type::err_function_not_available: return "Err Function Not Available";
            case enum_type::e_change_password: return "E Change Password";
            case enum_type::err_invalid_branch: return "Err Invalid Branch";
            case enum_type::oe_program_error: return "Oe Program Error";
            case enum_type::err_invalid_status: return "Err Invalid Status";
            case enum_type::err_data_not_changed: return "Err Data Not Changed";
            case enum_type::e_dup_trd_cxl_request: return "E Dup Trd Cxl Request";
            case enum_type::err_invalid_buyer_user_id: return "Err Invalid Buyer User Id";
            case enum_type::err_invalid_seller_user_id: return "Err Invalid Seller User Id";
            case enum_type::e_invalid_version: return "E Invalid Version";
            case enum_type::oe_system_error: return "Oe System Error";
            case enum_type::err_user_disabled: return "Err User Disabled";
            case enum_type::oe_invalid_stock_status: return "Oe Invalid Stock Status";
            case enum_type::err_invalid_user_id: return "Err Invalid User Id";
            case enum_type::err_invalid_trader_id: return "Err Invalid Trader Id";
            case enum_type::oe_ato_in_open: return "Oe Ato In Open";
            case enum_type::e_dup_request: return "E Dup Request";
            case enum_type::e_only_cp_allowed: return "E Only Cp Allowed";
            case enum_type::e_sl_mit_nt_not_allowed_pclose: return "E Sl Mit Nt Not Allowed Pclose";
            case enum_type::e_gtc_gtd_ord_not_allowed_pclose: return "E Gtc Gtd Ord Not Allowed Pclose";
            case enum_type::oe_cont_mod_not_allowed: return "Oe Cont Mod Not Allowed";
            case enum_type::trd_cont_mod_not_allowed: return "Trd Cont Mod Not Allowed";
            case enum_type::str_pro_partivipant_invalid: return "Str Pro Partivipant Invalid";
            case enum_type::error_invalid_price: return "Error Invalid Price";
            case enum_type::oe_diff_trd_mod_vol: return "Oe Diff Trd Mod Vol";
            case enum_type::error_user_not_exists_in_system: return "Error User Not Exists In System";
            case enum_type::err_already_deleted: return "Err Already Deleted";
            case enum_type::record_not_found: return "Record Not Found";
            case enum_type::oe_markets_closed: return "Oe Markets Closed";
            case enum_type::oe_security_not_admitted: return "Oe Security Not Admitted";
            case enum_type::oe_security_matured: return "Oe Security Matured";
            case enum_type::oe_security_expelled: return "Oe Security Expelled";
            case enum_type::oe_issued_cap_exceeds: return "Oe Issued Cap Exceeds";
            case enum_type::oe_price_not_mult: return "Oe Price Not Mult";
            case enum_type::oe_price_exceeds_day_min_max: return "Oe Price Exceeds Day Min Max";
            case enum_type::oe_is_not_active: return "Oe Is Not Active";
            case enum_type::e_system_wrong_state: return "E System Wrong State";
            case enum_type::oe_auction_pending: return "Oe Auction Pending";
            case enum_type::oe_qty_freeze_can: return "Oe Qty Freeze Can";
            case enum_type::oe_price_freeze_can: return "Oe Price Freeze Can";
            case enum_type::oe_sol_period_over: return "Oe Sol Period Over";
            case enum_type::oe_comp_period_over: return "Oe Comp Period Over";
            case enum_type::oe_auc_period_greater: return "Oe Auc Period Greater";
            case enum_type::oe_limit_trigger: return "Oe Limit Trigger";
            case enum_type::oe_trigger_price_not_mult: return "Oe Trigger Price Not Mult";
            case enum_type::oe_no_aon_attrib: return "Oe No Aon Attrib";
            case enum_type::oe_no_mf_attrib: return "Oe No Mf Attrib";
            case enum_type::oe_no_aon_in_attrib_1: return "Oe No Aon In Attrib 1";
            case enum_type::oe_no_mf_attrib_1: return "Oe No Mf Attrib 1";
            case enum_type::oe_mf_greater_disc: return "Oe Mf Greater Disc";
            case enum_type::oe_mf_not_mult: return "Oe Mf Not Mult";
            case enum_type::oe_mf_greater_original: return "Oe Mf Greater Original";
            case enum_type::oe_disc_greater_original: return "Oe Disc Greater Original";
            case enum_type::oe_disc_not_mult: return "Oe Disc Not Mult";
            case enum_type::oe_gtd_greater: return "Oe Gtd Greater";
            case enum_type::oe_quantity_gerater_rl: return "Oe Quantity Gerater Rl";
            case enum_type::oe_quantity_not_mult_rl: return "Oe Quantity Not Mult Rl";
            case enum_type::oe_broker_not_permitted: return "Oe Broker Not Permitted";
            case enum_type::oe_is_suspended: return "Oe Is Suspended";
            case enum_type::oe_branch_li_mit_exceeded: return "Oe Branch Li Mit Exceeded";
            case enum_type::oe_ord_can_changed: return "Oe Ord Can Changed";
            case enum_type::oe_ord_cannot_cancel: return "Oe Ord Cannot Cancel";
            case enum_type::oe_init_ord_cancel: return "Oe Init Ord Cancel";
            case enum_type::oe_ord_cannot_modify: return "Oe Ord Cannot Modify";
            case enum_type::err_trading_not_allowed: return "Err Trading Not Allowed";
            case enum_type::oe_nt_rejected: return "Oe Nt Rejected";
            case enum_type::chg_st_exists: return "Chg St Exists";
            case enum_type::oe_security_in_preopen: return "Oe Security In Preopen";
            case enum_type::oe_inq_not_allowed: return "Oe Inq Not Allowed";
            case enum_type::oe_security_ineligible: return "Oe Security Ineligible";
            case enum_type::e_fok_order_cancelled: return "E Fok Order Cancelled";
            case enum_type::turnover_limit_not_provided: return "Turnover Limit Not Provided";
            case enum_type::err_cannot_mod_auc_order: return "Err Cannot Mod Auc Order";
            case enum_type::oe_max_dq_allowed: return "Oe Max Dq Allowed";
            case enum_type::oe_admin_susp_can: return "Oe Admin Susp Can";
            case enum_type::e_invalid_buy_sell_type: return "E Invalid Buy Sell Type";
            case enum_type::e_invalid_book_type: return "E Invalid Book Type";
            case enum_type::e_invalid_trigger_price: return "E Invalid Trigger Price";
            case enum_type::e_invalid_pro_client: return "E Invalid Pro Client";
            case enum_type::e_invalid_instructions: return "E Invalid Instructions";
            case enum_type::e_invalid_order_parameters: return "E Invalid Order Parameters";
            case enum_type::e_nnf_req_exceeded: return "E Nnf Req Exceeded";
            case enum_type::invalid_order: return "Invalid Order";
            case enum_type::err_box_rate_exceeded_at_millisecond_level: return "Err Box Rate Exceeded At Millisecond Level";
            case enum_type::e_gtd_gt_maturity: return "E Gtd Gt Maturity";
            case enum_type::dq_not_allowed_in_preopen: return "Dq Not Allowed In Preopen";
            case enum_type::st_ord_not_allowed_popen: return "St Ord Not Allowed Popen";
            case enum_type::e_ord_lim_exceeds_ord_val_lim: return "E Ord Lim Exceeds Ord Val Lim";
            case enum_type::err_usr_ord_value_limit_exceeded: return "Err Usr Ord Value Limit Exceeded";
            case enum_type::sl_not_allowed: return "Sl Not Allowed";
            case enum_type::mit_not_allowed: return "Mit Not Allowed";
            case enum_type::e_ord_not_allowed_in_preopen: return "E Ord Not Allowed In Preopen";
            case enum_type::error_sl_lmt_rsnblty_check: return "Error Sl Lmt Rsnblty Check";
            case enum_type::e_not_modifiable: return "E Not Modifiable";
            case enum_type::e_tm_cm_does_not_exist: return "E Tm Cm Does Not Exist";
            case enum_type::e_not_clg_mem: return "E Not Clg Mem";
            case enum_type::e_user_not_corp_mgr: return "E User Not Corp Mgr";
            case enum_type::e_pm_cm_invalid: return "E Pm Cm Invalid";
            case enum_type::e_corp_mgr_vu_mod: return "E Corp Mgr Vu Mod";
            case enum_type::e_invalid_participant: return "E Invalid Participant";
            case enum_type::e_trade_approved_by_cm: return "E Trade Approved By Cm";
            case enum_type::e_cm_stock_suspended: return "E Cm Stock Suspended";
            case enum_type::e_broker_not_permitted_in_fut: return "E Broker Not Permitted In Fut";
            case enum_type::e_broker_not_permitted_in_opt: return "E Broker Not Permitted In Opt";
            case enum_type::e_qty_less_than_min_lot: return "E Qty Less Than Min Lot";
            case enum_type::e_disc_qty_less_than_min_lot: return "E Disc Qty Less Than Min Lot";
            case enum_type::e_mf_qty_less_than_min_lot: return "E Mf Qty Less Than Min Lot";
            case enum_type::e_already_rejected: return "E Already Rejected";
            case enum_type::e_nt_orders_not_allowed: return "E Nt Orders Not Allowed";
            case enum_type::e_nt_trade_not_allowed: return "E Nt Trade Not Allowed";
            case enum_type::e_inconsistent_broker_branch: return "E Inconsistent Broker Branch";
            case enum_type::m_post_close_start: return "M Post Close Start";
            case enum_type::m_post_close_ended: return "M Post Close Ended";
            case enum_type::m_post_close_trades: return "M Post Close Trades";
            case enum_type::e_invalid_msg_length: return "E Invalid Msg Length";
            case enum_type::e_invalid_open_close_type: return "E Invalid Open Close Type";
            case enum_type::e_nnf_inq_req_exceeded: return "E Nnf Inq Req Exceeded";
            case enum_type::e_participant_and_volume_changed: return "E Participant And Volume Changed";
            case enum_type::e_invalid_cover_uncover_type: return "E Invalid Cover Uncover Type";
            case enum_type::e_illegal_participant: return "E Illegal Participant";
            case enum_type::e_invalid_fill_price: return "E Invalid Fill Price";
            case enum_type::e_pro_no_participant: return "E Pro No Participant";
            case enum_type::e_invalid_account_no: return "E Invalid Account No";
            case enum_type::e_allow_no_participant_order: return "E Allow No Participant Order";
            case enum_type::m_delete_all_orders: return "M Delete All Orders";
            case enum_type::e_cum_ur_ord_val_limit_exceede: return "E Cum Ur Ord Val Limit Exceede";
            case enum_type::e_branch_ord_val_limit_exceeded: return "E Branch Ord Val Limit Exceeded";
            case enum_type::err_ord_val_exceeded: return "Err Ord Val Exceeded";
            case enum_type::err_preopen_order_reject: return "Err Preopen Order Reject";
            case enum_type::e_dealer_value_limit_exceeds: return "E Dealer Value Limit Exceeds";
            case enum_type::e_participant_not_found: return "E Participant Not Found";
            case enum_type::e_either_leg_failed: return "E Either Leg Failed";
            case enum_type::e_qty_greater_than_freeze_qty: return "E Qty Greater Than Freeze Qty";
            case enum_type::e_spread_not_allowed: return "E Spread Not Allowed";
            case enum_type::e_spread_allowed_if_stock_open: return "E Spread Allowed If Stock Open";
            case enum_type::e_qty_should_be_same: return "E Qty Should Be Same";
            case enum_type::e_ord_mod_qty_frz_not_allowed: return "E Ord Mod Qty Frz Not Allowed";
            case enum_type::e_trade_rec_modified: return "E Trade Rec Modified";
            case enum_type::e_tm_order_cant_be_modified: return "E Tm Order Cant Be Modified";
            case enum_type::e_tm_order_cant_be_cancelled: return "E Tm Order Cant Be Cancelled";
            case enum_type::e_tm_trade_cant_be_manipulated: return "E Tm Trade Cant Be Manipulated";
            case enum_type::e_cm_of_tm_suspended: return "E Cm Of Tm Suspended";
            case enum_type::e_expdate_not_in_ascending_ord: return "E Expdate Not In Ascending Ord";
            case enum_type::e_invalid_contract_comb: return "E Invalid Contract Comb";
            case enum_type::e_bm_cannot_cancel_cm_orders: return "E Bm Cannot Cancel Cm Orders";
            case enum_type::e_bm_cannot_cancel_bm_orders: return "E Bm Cannot Cancel Bm Orders";
            case enum_type::e_cm_cannot_cancel_cm_orders: return "E Cm Cannot Cancel Cm Orders";
            case enum_type::e_spread_in_different_underlying: return "E Spread In Different Underlying";
            case enum_type::e_invalid_cli_ac: return "E Invalid Cli Ac";
            case enum_type::e_br_ord_limit_fut_buy_exceeded: return "E Br Ord Limit Fut Buy Exceeded";
            case enum_type::e_br_ord_limit_fut_sell_exceeded: return "E Br Ord Limit Fut Sell Exceeded";
            case enum_type::e_br_ord_limit_opt_buy_exceeded: return "E Br Ord Limit Opt Buy Exceeded";
            case enum_type::e_br_ord_limit_opt_sell_exceeded: return "E Br Ord Limit Opt Sell Exceeded";
            case enum_type::e_ur_ord_limit_fut_buy_exceeded: return "E Ur Ord Limit Fut Buy Exceeded";
            case enum_type::e_ur_ord_limit_fut_sell_exceeded: return "E Ur Ord Limit Fut Sell Exceeded";
            case enum_type::e_ur_ord_limit_opt_buy_exceeded: return "E Ur Ord Limit Opt Buy Exceeded";
            case enum_type::e_ur_ord_limit_opt_sell_exceeded: return "E Ur Ord Limit Opt Sell Exceeded";
            case enum_type::e_cant_appr_bhav_copy_generated: return "E Cant Appr Bhav Copy Generated";
            case enum_type::e_collateral_lmt_chk: return "E Collateral Lmt Chk";
            case enum_type::e_address_not_found: return "E Address Not Found";
            case enum_type::e_stk_in_popen: return "E Stk In Popen";
            case enum_type::e_invalid_nnf_field: return "E Invalid Nnf Field";
            case enum_type::e_gtcgtd_not_allowed: return "E Gtcgtd Not Allowed";
            case enum_type::err_user_already_signed_off: return "Err User Already Signed Off";
            case enum_type::err_no_privilege: return "Err No Privilege";
            case enum_type::closeout_order_reject: return "Closeout Order Reject";
            case enum_type::closeout_frz_reject: return "Closeout Frz Reject";
            case enum_type::closeout_not_allowed: return "Closeout Not Allowed";
            case enum_type::closeout_trdmod_reject: return "Closeout Trdmod Reject";
            case enum_type::partial_order_reject: return "Partial Order Reject";
            case enum_type::partial_quick_order_cxl_rej: return "Partial Quick Order Cxl Rej";
            case enum_type::error_invalid_sprd_combination: return "Error Invalid Sprd Combination";
            case enum_type::e_price_diff_out_of_range: return "E Price Diff Out Of Range";
            case enum_type::rms_rejected_in_preopen: return "Rms Rejected In Preopen";
            case enum_type::error_algoid_nnfid_mismatch_1: return "Error Algoid Nnfid Mismatch 1";
            case enum_type::error_algoid_nnfid_mismatch_2: return "Error Algoid Nnfid Mismatch 2";
            case enum_type::error_algo_mkt_not_allowed: return "Error Algo Mkt Not Allowed";
            case enum_type::error_invalid_nnf_id: return "Error Invalid Nnf Id";
            case enum_type::error_preopn_ato_mod_can_rej: return "Error Preopn Ato Mod Can Rej";
            case enum_type::error_preopn_ato_not_allowed: return "Error Preopn Ato Not Allowed";
            case enum_type::err_usr_not_found_in_nnf_file: return "Err Usr Not Found In Nnf File";
            case enum_type::e_vc_order_rejected: return "E Vc Order Rejected";
            case enum_type::e_ssd_order_rejected: return "E Ssd Order Rejected";
            case enum_type::e_order_cancelled_for_vc: return "E Order Cancelled For Vc";
            case enum_type::e_order_cancelled_for_ssd: return "E Order Cancelled For Ssd";
            case enum_type::msg_code_voluntary_close_out_status: return "Msg Code Voluntary Close Out Status";
            case enum_type::msg_code_suspended_status: return "Msg Code Suspended Status";
            case enum_type::e_bo_price_out_of_range: return "E Bo Price Out Of Range";
            case enum_type::e_bo_excess_quantity: return "E Bo Excess Quantity";
            case enum_type::e_user_ineligible_for_bulk_orders: return "E User Ineligible For Bulk Orders";
            case enum_type::e_user_not_allowed_for_regular: return "E User Not Allowed For Regular";
            case enum_type::e_account_debarred: return "E Account Debarred";
            case enum_type::e_account_debarred_by_pit: return "E Account Debarred By Pit";
            case enum_type::err_usr_already_unlcked: return "Err Usr Already Unlcked";
            case enum_type::err_duplicate_unlck_alrt: return "Err Duplicate Unlck Alrt";
            case enum_type::err_actv_num_of_usrs_in_brnch_exceeded: return "Err Actv Num Of Usrs In Brnch Exceeded";
            case enum_type::ec_trd_mod_rej_cli_cp_mod_not_allowed: return "Ec Trd Mod Rej Cli Cp Mod Not Allowed";
            case enum_type::error_quantity_lim_exceeds_qty_val_lim: return "Error Quantity Lim Exceeds Qty Val Lim";
            case enum_type::user_trd_mod_disabled: return "User Trd Mod Disabled";
            case enum_type::preopen_trade_cancellation_not_allowed: return "Preopen Trade Cancellation Not Allowed";
            case enum_type::err_depndent_sessn_not_active: return "Err Depndent Sessn Not Active";
            case enum_type::e_trd_price_out_of_stock_tpp: return "E Trd Price Out Of Stock Tpp";
            case enum_type::e_order_cancelled_for_self_trade: return "E Order Cancelled For Self Trade";
            case enum_type::e_invalid_packet: return "E Invalid Packet";
            case enum_type::sssssse_hearbeat_not_received: return "Sssssse Hearbeat Not Received";
            case enum_type::e_invalid_box_id: return "E Invalid Box Id";
            case enum_type::e_seq_no_mismatch: return "E Seq No Mismatch";
            case enum_type::e_box_rate_exceeded: return "E Box Rate Exceeded";
            case enum_type::error_hb_rate_exceeded: return "Error Hb Rate Exceeded";
            case enum_type::e_max_user_count_exceeded: return "E Max User Count Exceeded";
            case enum_type::e_invalid_box_ip_combination: return "E Invalid Box Ip Combination";
            case enum_type::err_invalid_pan_id: return "Err Invalid Pan Id";
            case enum_type::err_invalid_algo_id: return "Err Invalid Algo Id";
            case enum_type::err_invalid_value_in_reserved: return "Err Invalid Value In Reserved";
            case enum_type::err_algo_id_disabled: return "Err Algo Id Disabled";
            case enum_type::err_order_cancelled_algoid_disabled: return "Err Order Cancelled Algoid Disabled";
            case enum_type::err_checksum_failed_gr: return "Err Checksum Failed Gr";
            case enum_type::err_multiple_gr_query_rcv: return "Err Multiple Gr Query Rcv";
            case enum_type::err_mkt_order_not_allowed: return "Err Mkt Order Not Allowed";
            case enum_type::err_trade_beyond_markup_price: return "Err Trade Beyond Markup Price";
            case enum_type::err_encryption_flag_mismatch: return "Err Encryption Flag Mismatch";
            case enum_type::err_md_5_checksum_failure: return "Err Md 5 Checksum Failure";
            case enum_type::err_user_having_null_rights: return "Err User Having Null Rights";
            default: return "unknown";
        }
    }

    static constexpr std::optional<enum_type> from_string(std::string_view str) {
        auto it = std::lower_bound(
            from_string_map.begin(),
            from_string_map.end(),
            str,
            [](const auto& pair, std::string_view s) { return pair.first < s; }
        );
        if (it != from_string_map.end() && it->first == str) {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const char* name = "error_code";
    static constexpr std::size_t size = 2;
    static constexpr bool is_optional = false;

    using result_type = required<error_code::enum_type>;
    using storage_type = result_type;

    constexpr error_code()
     : value{ enum_type{} } {}

    constexpr error_code(enum_type v)
     : value{ v } {}

    [[nodiscard]] constexpr result_type get() const {
        return result_type{value};
    }

    constexpr void set(enum_type v) {
        value = v;
    }

    constexpr void set(result_type value) {
        if (value.has_value())
            set(value.value());
        else
            set(enum_type::err_user_having_null_rights);
    }

  protected:
    enum_type value;
};
}
