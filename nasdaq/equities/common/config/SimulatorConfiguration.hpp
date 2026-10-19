#pragma once

#include <boost/program_options.hpp>
#include <fstream>
#include <sstream>
#include <string>

namespace sim {

struct Endpoint { std::string host; unsigned short port; };

struct RuntimeConfig {
    Endpoint order_entry;
    Endpoint totalview_feed;
    unsigned short glimpse_replay_port;
    unsigned short totalviewtcp_replay_port;
    // One endpoint per link: the reader listens there and the writer dials it.
    Endpoint to_engine_link;
    Endpoint to_clients_link;
    Endpoint to_totalview_link;
    Endpoint to_glimpse_link;
    Endpoint to_totalviewtcp_link;
    Endpoint to_maker_link;
    Endpoint from_maker_link;
    Endpoint to_taker_link;
    Endpoint from_taker_link;
    Endpoint to_replay_link;
    Endpoint from_replay_link;
    Endpoint to_requests_link;
    Endpoint from_requests_link;
    std::string recording_directory;
};

// Parse [--config <file>] (default simulator.ini); missing file / keys keep the generated defaults.
inline RuntimeConfig load_config(int argc, char** argv) {
    namespace po = boost::program_options;
    RuntimeConfig c;

    po::options_description options("simulator");
    options.add_options()
        ("order-entry.host", po::value<std::string>(&c.order_entry.host)->default_value("127.0.0.1"))
        ("order-entry.port", po::value<unsigned short>(&c.order_entry.port)->default_value(15001))
        ("feed-totalview.group", po::value<std::string>(&c.totalview_feed.host)->default_value("233.54.12.111"))
        ("feed-totalview.port", po::value<unsigned short>(&c.totalview_feed.port)->default_value(26477))
        ("replay-glimpse.port", po::value<unsigned short>(&c.glimpse_replay_port)->default_value(15002))
        ("replay-totalviewtcp.port", po::value<unsigned short>(&c.totalviewtcp_replay_port)->default_value(15003))
        ("link-to_engine.host", po::value<std::string>(&c.to_engine_link.host)->default_value("engine"))
        ("link-to_engine.port", po::value<unsigned short>(&c.to_engine_link.port)->default_value(19000))
        ("link-to_clients.host", po::value<std::string>(&c.to_clients_link.host)->default_value("gateway"))
        ("link-to_clients.port", po::value<unsigned short>(&c.to_clients_link.port)->default_value(19001))
        ("link-to_totalview.host", po::value<std::string>(&c.to_totalview_link.host)->default_value("totalview"))
        ("link-to_totalview.port", po::value<unsigned short>(&c.to_totalview_link.port)->default_value(19002))
        ("link-to_glimpse.host", po::value<std::string>(&c.to_glimpse_link.host)->default_value("glimpse"))
        ("link-to_glimpse.port", po::value<unsigned short>(&c.to_glimpse_link.port)->default_value(19003))
        ("link-to_totalviewtcp.host", po::value<std::string>(&c.to_totalviewtcp_link.host)->default_value("totalviewtcp"))
        ("link-to_totalviewtcp.port", po::value<unsigned short>(&c.to_totalviewtcp_link.port)->default_value(19004))
        ("link-to_maker.host", po::value<std::string>(&c.to_maker_link.host)->default_value("maker"))
        ("link-to_maker.port", po::value<unsigned short>(&c.to_maker_link.port)->default_value(19005))
        ("link-from_maker.host", po::value<std::string>(&c.from_maker_link.host)->default_value("gateway"))
        ("link-from_maker.port", po::value<unsigned short>(&c.from_maker_link.port)->default_value(19006))
        ("link-to_taker.host", po::value<std::string>(&c.to_taker_link.host)->default_value("taker"))
        ("link-to_taker.port", po::value<unsigned short>(&c.to_taker_link.port)->default_value(19007))
        ("link-from_taker.host", po::value<std::string>(&c.from_taker_link.host)->default_value("gateway"))
        ("link-from_taker.port", po::value<unsigned short>(&c.from_taker_link.port)->default_value(19008))
        ("link-to_replay.host", po::value<std::string>(&c.to_replay_link.host)->default_value("replay"))
        ("link-to_replay.port", po::value<unsigned short>(&c.to_replay_link.port)->default_value(19009))
        ("link-from_replay.host", po::value<std::string>(&c.from_replay_link.host)->default_value("gateway"))
        ("link-from_replay.port", po::value<unsigned short>(&c.from_replay_link.port)->default_value(19010))
        ("link-to_requests.host", po::value<std::string>(&c.to_requests_link.host)->default_value("requests"))
        ("link-to_requests.port", po::value<unsigned short>(&c.to_requests_link.port)->default_value(19011))
        ("link-from_requests.host", po::value<std::string>(&c.from_requests_link.host)->default_value("gateway"))
        ("link-from_requests.port", po::value<unsigned short>(&c.from_requests_link.port)->default_value(19012))
        ("recording.directory", po::value<std::string>(&c.recording_directory)->default_value("recordings"));

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
    return c;
}

}
