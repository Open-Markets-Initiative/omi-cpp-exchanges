#pragma once

#include <boost/program_options.hpp>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "../run/Clock.hpp"

namespace sim {

struct Endpoint { std::string host; unsigned short port; };

struct RuntimeConfig {
    Endpoint order_entry;
    // Each trader's gateway listens on a port of its own, on the order entry's host.
    unsigned short gatewaymakerbasic_port;
    unsigned short gatewaytakerbasic_port;
    Endpoint itch_feed;
    std::string itchtcp_replay_host;   // the interface its clients dial
    unsigned short itchtcp_replay_port;
    // One endpoint per link: the reader listens there and the writer dials it.
    Endpoint to_sequencer_gatewaymakerbasic_link;
    Endpoint to_sequencer_gatewaytakerbasic_link;
    Endpoint from_day_link;
    Endpoint sequenced_link;
    Endpoint to_clients_gatewaymakerbasic_link;
    Endpoint to_clients_gatewaytakerbasic_link;
    Endpoint to_itch_link;
    Endpoint to_itchtcp_link;
    Endpoint to_makerbasic_link;
    Endpoint from_makerbasic_link;
    Endpoint to_takerbasic_link;
    Endpoint from_takerbasic_link;
    // The seconds after a served venue starts that each change of its trading day is asked for.
    std::array<int, 8> day_at;
    std::string recording_directory;
    // How the venue keeps time: its mode, the time of day its day starts at, and the zone that is in.
    std::string clock_mode;
    std::string clock_start;
    std::string clock_zone;
};

// Parse [--config <file>] (default simulator.ini); missing file / keys keep the generated defaults.
inline RuntimeConfig load_config(int argc, char** argv) {
    namespace po = boost::program_options;
    RuntimeConfig c;

    po::options_description options("simulator");
    options.add_options()
        ("order-entry.host", po::value<std::string>(&c.order_entry.host)->default_value("127.0.0.1"))
        ("order-entry.port", po::value<unsigned short>(&c.order_entry.port)->default_value(17101))
        ("gateway-makerbasic.port", po::value<unsigned short>(&c.gatewaymakerbasic_port)->default_value(17111))
        ("gateway-takerbasic.port", po::value<unsigned short>(&c.gatewaytakerbasic_port)->default_value(17112))
        ("replay-itchtcp.host", po::value<std::string>(&c.itchtcp_replay_host)->default_value("127.0.0.1"))
        ("replay-itchtcp.port", po::value<unsigned short>(&c.itchtcp_replay_port)->default_value(17103))
        ("link-to_sequencer_gatewaymakerbasic.host", po::value<std::string>(&c.to_sequencer_gatewaymakerbasic_link.host)->default_value("sequencer"))
        ("link-to_sequencer_gatewaymakerbasic.port", po::value<unsigned short>(&c.to_sequencer_gatewaymakerbasic_link.port)->default_value(19000))
        ("link-to_sequencer_gatewaytakerbasic.host", po::value<std::string>(&c.to_sequencer_gatewaytakerbasic_link.host)->default_value("sequencer"))
        ("link-to_sequencer_gatewaytakerbasic.port", po::value<unsigned short>(&c.to_sequencer_gatewaytakerbasic_link.port)->default_value(19001))
        ("link-from_day.host", po::value<std::string>(&c.from_day_link.host)->default_value("sequencer"))
        ("link-from_day.port", po::value<unsigned short>(&c.from_day_link.port)->default_value(19002))
        ("link-sequenced.host", po::value<std::string>(&c.sequenced_link.host)->default_value("engine"))
        ("link-sequenced.port", po::value<unsigned short>(&c.sequenced_link.port)->default_value(19003))
        ("link-to_clients_gatewaymakerbasic.host", po::value<std::string>(&c.to_clients_gatewaymakerbasic_link.host)->default_value("gatewaymakerbasic"))
        ("link-to_clients_gatewaymakerbasic.port", po::value<unsigned short>(&c.to_clients_gatewaymakerbasic_link.port)->default_value(19004))
        ("link-to_clients_gatewaytakerbasic.host", po::value<std::string>(&c.to_clients_gatewaytakerbasic_link.host)->default_value("gatewaytakerbasic"))
        ("link-to_clients_gatewaytakerbasic.port", po::value<unsigned short>(&c.to_clients_gatewaytakerbasic_link.port)->default_value(19005))
        ("link-to_itch.host", po::value<std::string>(&c.to_itch_link.host)->default_value("itch"))
        ("link-to_itch.port", po::value<unsigned short>(&c.to_itch_link.port)->default_value(19006))
        ("link-to_itchtcp.host", po::value<std::string>(&c.to_itchtcp_link.host)->default_value("itchtcp"))
        ("link-to_itchtcp.port", po::value<unsigned short>(&c.to_itchtcp_link.port)->default_value(19007))
        ("link-to_makerbasic.host", po::value<std::string>(&c.to_makerbasic_link.host)->default_value("makerbasic"))
        ("link-to_makerbasic.port", po::value<unsigned short>(&c.to_makerbasic_link.port)->default_value(19008))
        ("link-from_makerbasic.host", po::value<std::string>(&c.from_makerbasic_link.host)->default_value("gatewaymakerbasic"))
        ("link-from_makerbasic.port", po::value<unsigned short>(&c.from_makerbasic_link.port)->default_value(19009))
        ("link-to_takerbasic.host", po::value<std::string>(&c.to_takerbasic_link.host)->default_value("takerbasic"))
        ("link-to_takerbasic.port", po::value<unsigned short>(&c.to_takerbasic_link.port)->default_value(19010))
        ("link-from_takerbasic.host", po::value<std::string>(&c.from_takerbasic_link.host)->default_value("gatewaytakerbasic"))
        ("link-from_takerbasic.port", po::value<unsigned short>(&c.from_takerbasic_link.port)->default_value(19011))
        ("day.start", po::value<int>(&c.day_at[0])->default_value(0))
        ("day.preopen", po::value<int>(&c.day_at[1])->default_value(2))
        ("day.nocancel", po::value<int>(&c.day_at[2])->default_value(25))
        ("day.open", po::value<int>(&c.day_at[3])->default_value(30))
        ("day.preclose", po::value<int>(&c.day_at[4])->default_value(35))
        ("day.nocancel-2", po::value<int>(&c.day_at[5])->default_value(55))
        ("day.close", po::value<int>(&c.day_at[6])->default_value(60))
        ("day.end", po::value<int>(&c.day_at[7])->default_value(65))
        ("recording.directory", po::value<std::string>(&c.recording_directory)->default_value("recordings"))
        ("clock.mode", po::value<std::string>(&c.clock_mode)->default_value("Elapsed"))
        ("clock.start", po::value<std::string>(&c.clock_start)->default_value("00:00:00"))
        ("clock.zone", po::value<std::string>(&c.clock_zone)->default_value("UTC"));

    std::string path = "simulator.ini";
    for (int i = 1; i + 1 < argc; ++i)
        if (std::string(argv[i]) == "--config") path = argv[i + 1];

    po::variables_map vm;
    std::ifstream file(path);

    // A key the file leaves out takes the default declared above, and so does every key when
    // there is no file at all: store is what writes those defaults in, so it is always called.
    std::istringstream nothing;
    po::store(file ? po::parse_config_file(file, options, /*allow_unregistered*/true)
                  : po::parse_config_file(nothing, options, /*allow_unregistered*/true), vm);
    po::notify(vm);
    // The venue's clock, as the settings say, set before anything is stamped.
    try {
        sim::clock::use(c.clock_mode, c.clock_start, c.clock_zone);
    }
    catch (const std::runtime_error& refused) {
        std::fprintf(stderr, "%s: %s\n", argc > 0 ? argv[0] : "simulator", refused.what());
        std::exit(1);
    }
    return c;
}

}
