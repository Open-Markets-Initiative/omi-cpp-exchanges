#pragma once

namespace odx::odxequities::pts::ouch::v2_0 {

// Server Heartbeat
struct server_heartbeat {

    static constexpr auto name = "Server Heartbeat";

    // selected when server_packet_type == 'H'
    static constexpr auto field = "server_packet_type";
    static constexpr char value = 'H';

    // action properties
    static constexpr auto session = "Heartbeat";
    static constexpr auto origin = "Exchange";
};

}
