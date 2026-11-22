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
    unsigned short gatewayreplay_port;
    unsigned short gatewayrequests_port;
    unsigned short gatewaymakersecond_port;
    unsigned short gatewaytakersecond_port;
    unsigned short gatewayfixmaker_port;
    unsigned short gatewayfixtaker_port;
    Endpoint totalview_feed;
    std::string glimpse_replay_host;   // the interface its clients dial
    unsigned short glimpse_replay_port;
    std::string totalviewtcp_replay_host;   // the interface its clients dial
    unsigned short totalviewtcp_replay_port;
    // One endpoint per link: the reader listens there and the writer dials it.
    Endpoint to_sequencer_gatewaymakerbasic_link;
    Endpoint to_sequencer_gatewaytakerbasic_link;
    Endpoint to_sequencer_gatewayreplay_link;
    Endpoint to_sequencer_gatewayrequests_link;
    Endpoint to_sequencer_gatewaymakersecond_link;
    Endpoint to_sequencer_gatewaytakersecond_link;
    Endpoint to_sequencer_gatewayfixmaker_link;
    Endpoint to_sequencer_gatewayfixtaker_link;
    Endpoint from_day_link;
    Endpoint sequenced_link;
    Endpoint to_clients_gatewaymakerbasic_link;
    Endpoint to_clients_gatewaytakerbasic_link;
    Endpoint to_clients_gatewayreplay_link;
    Endpoint to_clients_gatewayrequests_link;
    Endpoint to_clients_gatewaymakersecond_link;
    Endpoint to_clients_gatewaytakersecond_link;
    Endpoint to_clients_gatewayfixmaker_link;
    Endpoint to_clients_gatewayfixtaker_link;
    Endpoint to_totalview_link;
    Endpoint to_glimpse_link;
    Endpoint to_totalviewtcp_link;
    Endpoint to_makerbasic_link;
    Endpoint from_makerbasic_link;
    Endpoint to_takerbasic_link;
    Endpoint from_takerbasic_link;
    Endpoint to_replay_link;
    Endpoint from_replay_link;
    Endpoint to_requests_link;
    Endpoint from_requests_link;
    Endpoint to_makersecond_link;
    Endpoint from_makersecond_link;
    Endpoint to_takersecond_link;
    Endpoint from_takersecond_link;
    Endpoint to_fixmaker_link;
    Endpoint from_fixmaker_link;
    Endpoint to_fixtaker_link;
    Endpoint from_fixtaker_link;
    // The seconds after a served venue starts that each change of its trading day is asked for.
    std::array<int, 20> day_at;
    std::string recording_directory;
    // The engine's own book: the file it is also written to, none where empty, and when the day asks for it.
    std::string book_dump;
    std::string book_every;
    // The port the trading day reads a supervisor on, which asks it to break a match; none where 0.
    int supervisor_port;
    // How the venue keeps time: its mode, the time of day its day starts at, and the zone that is in.
    std::string clock_mode;
    std::string clock_start;
    std::string clock_zone;
    int clock_tick_milliseconds;   // how often the venue's time is sent its engine
};

// Parse [--config <file>] (default simulator.ini); missing file / keys keep the generated defaults.
inline RuntimeConfig load_config(int argc, char** argv) {
    namespace po = boost::program_options;
    RuntimeConfig c;

    po::options_description options("simulator");
    options.add_options()
        ("order-entry.host", po::value<std::string>(&c.order_entry.host)->default_value("127.0.0.1"))
        ("order-entry.port", po::value<unsigned short>(&c.order_entry.port)->default_value(15101))
        ("gateway-makerbasic.port", po::value<unsigned short>(&c.gatewaymakerbasic_port)->default_value(15111))
        ("gateway-takerbasic.port", po::value<unsigned short>(&c.gatewaytakerbasic_port)->default_value(15112))
        ("gateway-replay.port", po::value<unsigned short>(&c.gatewayreplay_port)->default_value(15113))
        ("gateway-requests.port", po::value<unsigned short>(&c.gatewayrequests_port)->default_value(15114))
        ("gateway-makersecond.port", po::value<unsigned short>(&c.gatewaymakersecond_port)->default_value(15115))
        ("gateway-takersecond.port", po::value<unsigned short>(&c.gatewaytakersecond_port)->default_value(15116))
        ("gateway-fixmaker.port", po::value<unsigned short>(&c.gatewayfixmaker_port)->default_value(15117))
        ("gateway-fixtaker.port", po::value<unsigned short>(&c.gatewayfixtaker_port)->default_value(15118))
        ("feed-totalview.group", po::value<std::string>(&c.totalview_feed.host)->default_value("233.54.12.112"))
        ("feed-totalview.port", po::value<unsigned short>(&c.totalview_feed.port)->default_value(26478))
        ("replay-glimpse.host", po::value<std::string>(&c.glimpse_replay_host)->default_value("127.0.0.1"))
        ("replay-glimpse.port", po::value<unsigned short>(&c.glimpse_replay_port)->default_value(15102))
        ("replay-totalviewtcp.host", po::value<std::string>(&c.totalviewtcp_replay_host)->default_value("127.0.0.1"))
        ("replay-totalviewtcp.port", po::value<unsigned short>(&c.totalviewtcp_replay_port)->default_value(15103))
        ("link-to_sequencer_gatewaymakerbasic.host", po::value<std::string>(&c.to_sequencer_gatewaymakerbasic_link.host)->default_value("sequencer"))
        ("link-to_sequencer_gatewaymakerbasic.port", po::value<unsigned short>(&c.to_sequencer_gatewaymakerbasic_link.port)->default_value(19000))
        ("link-to_sequencer_gatewaytakerbasic.host", po::value<std::string>(&c.to_sequencer_gatewaytakerbasic_link.host)->default_value("sequencer"))
        ("link-to_sequencer_gatewaytakerbasic.port", po::value<unsigned short>(&c.to_sequencer_gatewaytakerbasic_link.port)->default_value(19001))
        ("link-to_sequencer_gatewayreplay.host", po::value<std::string>(&c.to_sequencer_gatewayreplay_link.host)->default_value("sequencer"))
        ("link-to_sequencer_gatewayreplay.port", po::value<unsigned short>(&c.to_sequencer_gatewayreplay_link.port)->default_value(19002))
        ("link-to_sequencer_gatewayrequests.host", po::value<std::string>(&c.to_sequencer_gatewayrequests_link.host)->default_value("sequencer"))
        ("link-to_sequencer_gatewayrequests.port", po::value<unsigned short>(&c.to_sequencer_gatewayrequests_link.port)->default_value(19003))
        ("link-to_sequencer_gatewaymakersecond.host", po::value<std::string>(&c.to_sequencer_gatewaymakersecond_link.host)->default_value("sequencer"))
        ("link-to_sequencer_gatewaymakersecond.port", po::value<unsigned short>(&c.to_sequencer_gatewaymakersecond_link.port)->default_value(19004))
        ("link-to_sequencer_gatewaytakersecond.host", po::value<std::string>(&c.to_sequencer_gatewaytakersecond_link.host)->default_value("sequencer"))
        ("link-to_sequencer_gatewaytakersecond.port", po::value<unsigned short>(&c.to_sequencer_gatewaytakersecond_link.port)->default_value(19005))
        ("link-to_sequencer_gatewayfixmaker.host", po::value<std::string>(&c.to_sequencer_gatewayfixmaker_link.host)->default_value("sequencer"))
        ("link-to_sequencer_gatewayfixmaker.port", po::value<unsigned short>(&c.to_sequencer_gatewayfixmaker_link.port)->default_value(19006))
        ("link-to_sequencer_gatewayfixtaker.host", po::value<std::string>(&c.to_sequencer_gatewayfixtaker_link.host)->default_value("sequencer"))
        ("link-to_sequencer_gatewayfixtaker.port", po::value<unsigned short>(&c.to_sequencer_gatewayfixtaker_link.port)->default_value(19007))
        ("link-from_day.host", po::value<std::string>(&c.from_day_link.host)->default_value("sequencer"))
        ("link-from_day.port", po::value<unsigned short>(&c.from_day_link.port)->default_value(19008))
        ("link-sequenced.host", po::value<std::string>(&c.sequenced_link.host)->default_value("engine"))
        ("link-sequenced.port", po::value<unsigned short>(&c.sequenced_link.port)->default_value(19009))
        ("link-to_clients_gatewaymakerbasic.host", po::value<std::string>(&c.to_clients_gatewaymakerbasic_link.host)->default_value("gatewaymakerbasic"))
        ("link-to_clients_gatewaymakerbasic.port", po::value<unsigned short>(&c.to_clients_gatewaymakerbasic_link.port)->default_value(19010))
        ("link-to_clients_gatewaytakerbasic.host", po::value<std::string>(&c.to_clients_gatewaytakerbasic_link.host)->default_value("gatewaytakerbasic"))
        ("link-to_clients_gatewaytakerbasic.port", po::value<unsigned short>(&c.to_clients_gatewaytakerbasic_link.port)->default_value(19011))
        ("link-to_clients_gatewayreplay.host", po::value<std::string>(&c.to_clients_gatewayreplay_link.host)->default_value("gatewayreplay"))
        ("link-to_clients_gatewayreplay.port", po::value<unsigned short>(&c.to_clients_gatewayreplay_link.port)->default_value(19012))
        ("link-to_clients_gatewayrequests.host", po::value<std::string>(&c.to_clients_gatewayrequests_link.host)->default_value("gatewayrequests"))
        ("link-to_clients_gatewayrequests.port", po::value<unsigned short>(&c.to_clients_gatewayrequests_link.port)->default_value(19013))
        ("link-to_clients_gatewaymakersecond.host", po::value<std::string>(&c.to_clients_gatewaymakersecond_link.host)->default_value("gatewaymakersecond"))
        ("link-to_clients_gatewaymakersecond.port", po::value<unsigned short>(&c.to_clients_gatewaymakersecond_link.port)->default_value(19014))
        ("link-to_clients_gatewaytakersecond.host", po::value<std::string>(&c.to_clients_gatewaytakersecond_link.host)->default_value("gatewaytakersecond"))
        ("link-to_clients_gatewaytakersecond.port", po::value<unsigned short>(&c.to_clients_gatewaytakersecond_link.port)->default_value(19015))
        ("link-to_clients_gatewayfixmaker.host", po::value<std::string>(&c.to_clients_gatewayfixmaker_link.host)->default_value("gatewayfixmaker"))
        ("link-to_clients_gatewayfixmaker.port", po::value<unsigned short>(&c.to_clients_gatewayfixmaker_link.port)->default_value(19016))
        ("link-to_clients_gatewayfixtaker.host", po::value<std::string>(&c.to_clients_gatewayfixtaker_link.host)->default_value("gatewayfixtaker"))
        ("link-to_clients_gatewayfixtaker.port", po::value<unsigned short>(&c.to_clients_gatewayfixtaker_link.port)->default_value(19017))
        ("link-to_totalview.host", po::value<std::string>(&c.to_totalview_link.host)->default_value("totalview"))
        ("link-to_totalview.port", po::value<unsigned short>(&c.to_totalview_link.port)->default_value(19018))
        ("link-to_glimpse.host", po::value<std::string>(&c.to_glimpse_link.host)->default_value("glimpse"))
        ("link-to_glimpse.port", po::value<unsigned short>(&c.to_glimpse_link.port)->default_value(19019))
        ("link-to_totalviewtcp.host", po::value<std::string>(&c.to_totalviewtcp_link.host)->default_value("totalviewtcp"))
        ("link-to_totalviewtcp.port", po::value<unsigned short>(&c.to_totalviewtcp_link.port)->default_value(19020))
        ("link-to_makerbasic.host", po::value<std::string>(&c.to_makerbasic_link.host)->default_value("makerbasic"))
        ("link-to_makerbasic.port", po::value<unsigned short>(&c.to_makerbasic_link.port)->default_value(19021))
        ("link-from_makerbasic.host", po::value<std::string>(&c.from_makerbasic_link.host)->default_value("gatewaymakerbasic"))
        ("link-from_makerbasic.port", po::value<unsigned short>(&c.from_makerbasic_link.port)->default_value(19022))
        ("link-to_takerbasic.host", po::value<std::string>(&c.to_takerbasic_link.host)->default_value("takerbasic"))
        ("link-to_takerbasic.port", po::value<unsigned short>(&c.to_takerbasic_link.port)->default_value(19023))
        ("link-from_takerbasic.host", po::value<std::string>(&c.from_takerbasic_link.host)->default_value("gatewaytakerbasic"))
        ("link-from_takerbasic.port", po::value<unsigned short>(&c.from_takerbasic_link.port)->default_value(19024))
        ("link-to_replay.host", po::value<std::string>(&c.to_replay_link.host)->default_value("replay"))
        ("link-to_replay.port", po::value<unsigned short>(&c.to_replay_link.port)->default_value(19025))
        ("link-from_replay.host", po::value<std::string>(&c.from_replay_link.host)->default_value("gatewayreplay"))
        ("link-from_replay.port", po::value<unsigned short>(&c.from_replay_link.port)->default_value(19026))
        ("link-to_requests.host", po::value<std::string>(&c.to_requests_link.host)->default_value("requests"))
        ("link-to_requests.port", po::value<unsigned short>(&c.to_requests_link.port)->default_value(19027))
        ("link-from_requests.host", po::value<std::string>(&c.from_requests_link.host)->default_value("gatewayrequests"))
        ("link-from_requests.port", po::value<unsigned short>(&c.from_requests_link.port)->default_value(19028))
        ("link-to_makersecond.host", po::value<std::string>(&c.to_makersecond_link.host)->default_value("makersecond"))
        ("link-to_makersecond.port", po::value<unsigned short>(&c.to_makersecond_link.port)->default_value(19029))
        ("link-from_makersecond.host", po::value<std::string>(&c.from_makersecond_link.host)->default_value("gatewaymakersecond"))
        ("link-from_makersecond.port", po::value<unsigned short>(&c.from_makersecond_link.port)->default_value(19030))
        ("link-to_takersecond.host", po::value<std::string>(&c.to_takersecond_link.host)->default_value("takersecond"))
        ("link-to_takersecond.port", po::value<unsigned short>(&c.to_takersecond_link.port)->default_value(19031))
        ("link-from_takersecond.host", po::value<std::string>(&c.from_takersecond_link.host)->default_value("gatewaytakersecond"))
        ("link-from_takersecond.port", po::value<unsigned short>(&c.from_takersecond_link.port)->default_value(19032))
        ("link-to_fixmaker.host", po::value<std::string>(&c.to_fixmaker_link.host)->default_value("fixmaker"))
        ("link-to_fixmaker.port", po::value<unsigned short>(&c.to_fixmaker_link.port)->default_value(19033))
        ("link-from_fixmaker.host", po::value<std::string>(&c.from_fixmaker_link.host)->default_value("gatewayfixmaker"))
        ("link-from_fixmaker.port", po::value<unsigned short>(&c.from_fixmaker_link.port)->default_value(19034))
        ("link-to_fixtaker.host", po::value<std::string>(&c.to_fixtaker_link.host)->default_value("fixtaker"))
        ("link-to_fixtaker.port", po::value<unsigned short>(&c.to_fixtaker_link.port)->default_value(19035))
        ("link-from_fixtaker.host", po::value<std::string>(&c.from_fixtaker_link.host)->default_value("gatewayfixtaker"))
        ("link-from_fixtaker.port", po::value<unsigned short>(&c.from_fixtaker_link.port)->default_value(19036))
        ("day.preopen", po::value<int>(&c.day_at[0])->default_value(0))
        ("day.halt-zxzzt", po::value<int>(&c.day_at[1])->default_value(0))
        ("day.openinglockin", po::value<int>(&c.day_at[2])->default_value(25))
        ("day.openinglockout", po::value<int>(&c.day_at[3])->default_value(28))
        ("day.openinglate", po::value<int>(&c.day_at[4])->default_value(29))
        ("day.open", po::value<int>(&c.day_at[5])->default_value(30))
        ("day.postpone-zxzzt", po::value<int>(&c.day_at[6])->default_value(45))
        ("day.halt-zwzzt", po::value<int>(&c.day_at[7])->default_value(60))
        ("day.resume-zwzzt", po::value<int>(&c.day_at[8])->default_value(85))
        ("day.quote-zxzzt", po::value<int>(&c.day_at[9])->default_value(90))
        ("day.resume-zxzzt", po::value<int>(&c.day_at[10])->default_value(110))
        ("day.halt", po::value<int>(&c.day_at[11])->default_value(120))
        ("day.quote", po::value<int>(&c.day_at[12])->default_value(140))
        ("day.resume", po::value<int>(&c.day_at[13])->default_value(150))
        ("day.preclose", po::value<int>(&c.day_at[14])->default_value(240))
        ("day.closinglockout", po::value<int>(&c.day_at[15])->default_value(265))
        ("day.closingfreeze", po::value<int>(&c.day_at[16])->default_value(268))
        ("day.close", po::value<int>(&c.day_at[17])->default_value(270))
        ("day.extendedtradingcloseend", po::value<int>(&c.day_at[18])->default_value(275))
        ("day.end", po::value<int>(&c.day_at[19])->default_value(300))
        ("clock.tick-milliseconds", po::value<int>(&c.clock_tick_milliseconds)->default_value(1000))
        ("recording.directory", po::value<std::string>(&c.recording_directory)->default_value("recordings"))
        ("book.dump", po::value<std::string>(&c.book_dump)->default_value(""))
        ("book.every", po::value<std::string>(&c.book_every)->default_value("none"))
        ("supervisor.port", po::value<int>(&c.supervisor_port)->default_value(0))
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
