#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/MessageType.hpp"
#include "../types/IndustryName.hpp"
#include "../types/NumberOfIndustryRecords.hpp"
#include "../structs/IndexData.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Sector Index Report Message
struct sector_index_report_message {

    struct fields_type {
        nnf_orderentry::message_type message_type;
        nnf_orderentry::industry_name industry_name;
        nnf_orderentry::number_of_industry_records number_of_industry_records;
        nnf_orderentry::index_data index_data[10];
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::sector_index_report_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static sector_index_report_message* parse(std::byte* buffer) {
        return reinterpret_cast<sector_index_report_message*>(buffer);
    }

    // parse method const
    static const sector_index_report_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const sector_index_report_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(sector_index_report_message::fields_type, message_type) == 0, "unexpected offset of sector_index_report_message::fields_type::message_type");
static_assert(offsetof(sector_index_report_message::fields_type, industry_name) == 1, "unexpected offset of sector_index_report_message::fields_type::industry_name");
static_assert(offsetof(sector_index_report_message::fields_type, number_of_industry_records) == 16, "unexpected offset of sector_index_report_message::fields_type::number_of_industry_records");
static_assert(offsetof(sector_index_report_message::fields_type, index_data) == 18, "unexpected offset of sector_index_report_message::fields_type::index_data");
static_assert(sizeof(sector_index_report_message::fields_type) == 208, "unexpected sizeof sector_index_report_message::fields_type");
static_assert(sizeof(sector_index_report_message) == sizeof(nnf_orderentry::message_header) + 208, "unexpected sizeof sector_index_report_message");

#pragma pack(pop)
}
