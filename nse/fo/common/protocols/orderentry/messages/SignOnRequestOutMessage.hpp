#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/UserId.hpp"
#include "../types/Reserved8.hpp"
#include "../types/Password.hpp"
#include "../types/SecondReserved8.hpp"
#include "../types/NewPassword.hpp"
#include "../types/TraderName.hpp"
#include "../types/LastPasswordChangeDate.hpp"
#include "../types/BrokerId.hpp"
#include "../types/Reserved1.hpp"
#include "../types/BranchId.hpp"
#include "../types/VersionNumber.hpp"
#include "../types/EndTime.hpp"
#include "../types/SecondReserved1.hpp"
#include "../types/Colour.hpp"
#include "../types/ThirdReserved1.hpp"
#include "../types/UserType.hpp"
#include "../types/SequenceNumber.hpp"
#include "../types/Reserved14.hpp"
#include "../types/BrokerStatus.hpp"
#include "../types/ShowIndex.hpp"
#include "../types/StBrokerEligibilityPerMkt.hpp"
#include "../types/MemberType.hpp"
#include "../types/ClearingStatus.hpp"
#include "../types/BrokerName.hpp"
#include "../types/Reserved16.hpp"
#include "../types/SecondReserved16.hpp"
#include "../types/ThirdReserved16.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Sign On Request Out Message
struct sign_on_request_out_message {

    struct fields_type {
        nnf_orderentry::user_id user_id;
        nnf_orderentry::reserved_8 reserved_8;
        nnf_orderentry::password password;
        nnf_orderentry::second_reserved_8 second_reserved_8;
        nnf_orderentry::new_password new_password;
        nnf_orderentry::trader_name trader_name;
        nnf_orderentry::last_password_change_date last_password_change_date;
        nnf_orderentry::broker_id broker_id;
        nnf_orderentry::reserved_1 reserved_1;
        nnf_orderentry::branch_id branch_id;
        nnf_orderentry::version_number version_number;
        nnf_orderentry::end_time end_time;
        nnf_orderentry::second_reserved_1 second_reserved_1;
        nnf_orderentry::colour colour;
        nnf_orderentry::third_reserved_1 third_reserved_1;
        nnf_orderentry::user_type user_type;
        nnf_orderentry::sequence_number sequence_number;
        nnf_orderentry::reserved_14 reserved_14;
        nnf_orderentry::broker_status broker_status;
        nnf_orderentry::show_index show_index;
        nnf_orderentry::st_broker_eligibility_per_mkt st_broker_eligibility_per_mkt;
        nnf_orderentry::member_type member_type;
        nnf_orderentry::clearing_status clearing_status;
        nnf_orderentry::broker_name broker_name;
        nnf_orderentry::reserved_16 reserved_16;
        nnf_orderentry::second_reserved_16 second_reserved_16;
        nnf_orderentry::third_reserved_16 third_reserved_16;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::sign_on_request_out_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static sign_on_request_out_message* parse(std::byte* buffer) {
        return reinterpret_cast<sign_on_request_out_message*>(buffer);
    }

    // parse method const
    static const sign_on_request_out_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const sign_on_request_out_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(sign_on_request_out_message::fields_type, user_id) == 0, "unexpected offset of sign_on_request_out_message::fields_type::user_id");
static_assert(offsetof(sign_on_request_out_message::fields_type, reserved_8) == 4, "unexpected offset of sign_on_request_out_message::fields_type::reserved_8");
static_assert(offsetof(sign_on_request_out_message::fields_type, password) == 12, "unexpected offset of sign_on_request_out_message::fields_type::password");
static_assert(offsetof(sign_on_request_out_message::fields_type, second_reserved_8) == 20, "unexpected offset of sign_on_request_out_message::fields_type::second_reserved_8");
static_assert(offsetof(sign_on_request_out_message::fields_type, new_password) == 28, "unexpected offset of sign_on_request_out_message::fields_type::new_password");
static_assert(offsetof(sign_on_request_out_message::fields_type, trader_name) == 36, "unexpected offset of sign_on_request_out_message::fields_type::trader_name");
static_assert(offsetof(sign_on_request_out_message::fields_type, last_password_change_date) == 62, "unexpected offset of sign_on_request_out_message::fields_type::last_password_change_date");
static_assert(offsetof(sign_on_request_out_message::fields_type, broker_id) == 66, "unexpected offset of sign_on_request_out_message::fields_type::broker_id");
static_assert(offsetof(sign_on_request_out_message::fields_type, reserved_1) == 71, "unexpected offset of sign_on_request_out_message::fields_type::reserved_1");
static_assert(offsetof(sign_on_request_out_message::fields_type, branch_id) == 72, "unexpected offset of sign_on_request_out_message::fields_type::branch_id");
static_assert(offsetof(sign_on_request_out_message::fields_type, version_number) == 74, "unexpected offset of sign_on_request_out_message::fields_type::version_number");
static_assert(offsetof(sign_on_request_out_message::fields_type, end_time) == 78, "unexpected offset of sign_on_request_out_message::fields_type::end_time");
static_assert(offsetof(sign_on_request_out_message::fields_type, second_reserved_1) == 82, "unexpected offset of sign_on_request_out_message::fields_type::second_reserved_1");
static_assert(offsetof(sign_on_request_out_message::fields_type, colour) == 83, "unexpected offset of sign_on_request_out_message::fields_type::colour");
static_assert(offsetof(sign_on_request_out_message::fields_type, third_reserved_1) == 133, "unexpected offset of sign_on_request_out_message::fields_type::third_reserved_1");
static_assert(offsetof(sign_on_request_out_message::fields_type, user_type) == 134, "unexpected offset of sign_on_request_out_message::fields_type::user_type");
static_assert(offsetof(sign_on_request_out_message::fields_type, sequence_number) == 136, "unexpected offset of sign_on_request_out_message::fields_type::sequence_number");
static_assert(offsetof(sign_on_request_out_message::fields_type, reserved_14) == 144, "unexpected offset of sign_on_request_out_message::fields_type::reserved_14");
static_assert(offsetof(sign_on_request_out_message::fields_type, broker_status) == 158, "unexpected offset of sign_on_request_out_message::fields_type::broker_status");
static_assert(offsetof(sign_on_request_out_message::fields_type, show_index) == 159, "unexpected offset of sign_on_request_out_message::fields_type::show_index");
static_assert(offsetof(sign_on_request_out_message::fields_type, st_broker_eligibility_per_mkt) == 160, "unexpected offset of sign_on_request_out_message::fields_type::st_broker_eligibility_per_mkt");
static_assert(offsetof(sign_on_request_out_message::fields_type, member_type) == 162, "unexpected offset of sign_on_request_out_message::fields_type::member_type");
static_assert(offsetof(sign_on_request_out_message::fields_type, clearing_status) == 164, "unexpected offset of sign_on_request_out_message::fields_type::clearing_status");
static_assert(offsetof(sign_on_request_out_message::fields_type, broker_name) == 165, "unexpected offset of sign_on_request_out_message::fields_type::broker_name");
static_assert(offsetof(sign_on_request_out_message::fields_type, reserved_16) == 190, "unexpected offset of sign_on_request_out_message::fields_type::reserved_16");
static_assert(offsetof(sign_on_request_out_message::fields_type, second_reserved_16) == 206, "unexpected offset of sign_on_request_out_message::fields_type::second_reserved_16");
static_assert(offsetof(sign_on_request_out_message::fields_type, third_reserved_16) == 222, "unexpected offset of sign_on_request_out_message::fields_type::third_reserved_16");
static_assert(sizeof(sign_on_request_out_message::fields_type) == 238, "unexpected sizeof sign_on_request_out_message::fields_type");
static_assert(sizeof(sign_on_request_out_message) == sizeof(nnf_orderentry::message_header) + 238, "unexpected sizeof sign_on_request_out_message");

#pragma pack(pop)
}
