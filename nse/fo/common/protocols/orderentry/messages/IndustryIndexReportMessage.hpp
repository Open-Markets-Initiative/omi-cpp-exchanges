#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/MessageType.hpp"
#include "../types/Reserved1.hpp"
#include "../types/NumberOfIndustryRecords.hpp"
#include "../structs/IndustryIndex.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Industry Index Report Message
struct industry_index_report_message {

    struct fields_type {
        nnf_orderentry::message_type message_type;
        nnf_orderentry::reserved_1 reserved_1;
        nnf_orderentry::number_of_industry_records number_of_industry_records;
        nnf_orderentry::industry_index industry_index[10];
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::industry_index_report_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static industry_index_report_message* parse(std::byte* buffer) {
        return reinterpret_cast<industry_index_report_message*>(buffer);
    }

    // parse method const
    static const industry_index_report_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const industry_index_report_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(industry_index_report_message::fields_type, message_type) == 0, "unexpected offset of industry_index_report_message::fields_type::message_type");
static_assert(offsetof(industry_index_report_message::fields_type, reserved_1) == 1, "unexpected offset of industry_index_report_message::fields_type::reserved_1");
static_assert(offsetof(industry_index_report_message::fields_type, number_of_industry_records) == 2, "unexpected offset of industry_index_report_message::fields_type::number_of_industry_records");
static_assert(offsetof(industry_index_report_message::fields_type, industry_index) == 4, "unexpected offset of industry_index_report_message::fields_type::industry_index");
static_assert(sizeof(industry_index_report_message::fields_type) == 354, "unexpected sizeof industry_index_report_message::fields_type");
static_assert(sizeof(industry_index_report_message) == sizeof(nnf_orderentry::message_header) + 354, "unexpected sizeof industry_index_report_message");

#pragma pack(pop)
}
