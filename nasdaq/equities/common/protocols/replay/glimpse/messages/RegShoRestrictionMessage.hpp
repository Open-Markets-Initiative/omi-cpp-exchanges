#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/LocateCode.hpp"
#include "../types/TrackingNumber.hpp"
#include "../types/Timestamp.hpp"
#include "../types/Stock.hpp"
#include "../types/RegShoAction.hpp"
#include "../types/SequencedMessageType.hpp"

namespace nasdaq::nsmequities::totalview::glimpse::v5_0 {

namespace glimpse_totalview = ::nasdaq::nsmequities::totalview::glimpse::v5_0;

#pragma pack(push, 1)

// Reg Sho Restriction Message
struct reg_sho_restriction_message {

    struct fields_type {
        glimpse_totalview::locate_code locate_code;
        glimpse_totalview::tracking_number tracking_number;
        glimpse_totalview::timestamp timestamp;
        glimpse_totalview::stock stock;
        glimpse_totalview::reg_sho_action reg_sho_action;
    };

    glimpse_totalview::server_packet_header header = {std::uint16_t(sizeof(glimpse_totalview::server_packet_header) + sizeof(fields_type) + sizeof(glimpse_totalview::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    glimpse_totalview::sequenced_message_type sequenced_message_type = glimpse_totalview::sequenced_message_type::enum_type::reg_sho_restriction_message;

    fields_type fields;

    // parse method
    static reg_sho_restriction_message* parse(std::byte* buffer) {
        return reinterpret_cast<reg_sho_restriction_message*>(buffer);
    }

    // parse method const
    static const reg_sho_restriction_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const reg_sho_restriction_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(reg_sho_restriction_message::fields_type, locate_code) == 0, "unexpected offset of reg_sho_restriction_message::fields_type::locate_code");
static_assert(offsetof(reg_sho_restriction_message::fields_type, tracking_number) == 2, "unexpected offset of reg_sho_restriction_message::fields_type::tracking_number");
static_assert(offsetof(reg_sho_restriction_message::fields_type, timestamp) == 4, "unexpected offset of reg_sho_restriction_message::fields_type::timestamp");
static_assert(offsetof(reg_sho_restriction_message::fields_type, stock) == 10, "unexpected offset of reg_sho_restriction_message::fields_type::stock");
static_assert(offsetof(reg_sho_restriction_message::fields_type, reg_sho_action) == 18, "unexpected offset of reg_sho_restriction_message::fields_type::reg_sho_action");
static_assert(sizeof(reg_sho_restriction_message::fields_type) == 19, "unexpected sizeof reg_sho_restriction_message::fields_type");
static_assert(sizeof(reg_sho_restriction_message) == sizeof(glimpse_totalview::server_packet_header) + sizeof(glimpse_totalview::sequenced_message_type) + 19, "unexpected sizeof reg_sho_restriction_message");

#pragma pack(pop)
}
