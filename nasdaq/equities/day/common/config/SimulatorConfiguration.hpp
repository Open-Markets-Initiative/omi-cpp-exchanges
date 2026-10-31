#pragma once

#include <boost/program_options.hpp>
#include <array>
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
    // The seconds after a served venue starts that each change of its trading day is asked for.
    std::array<int, 7> day_at;
    std::string recording_directory;
};

// Parse [--config <file>] (default simulator.ini); missing file / keys keep the generated defaults.
inline RuntimeConfig load_config(int argc, char** argv) {
    namespace po = boost::program_options;
    RuntimeConfig c;

    po::options_description options("simulator");
    options.add_options()
        ("order-entry.host", po::value<std::string>(&c.order_entry.host)->default_value("127.0.0.1"))
        ("order-entry.port", po::value<unsigned short>(&c.order_entry.port)->default_value(15101))
        ("feed-totalview.group", po::value<std::string>(&c.totalview_feed.host)->default_value("233.54.12.112"))
        ("feed-totalview.port", po::value<unsigned short>(&c.totalview_feed.port)->default_value(26478))
        ("replay-glimpse.port", po::value<unsigned short>(&c.glimpse_replay_port)->default_value(15102))
        ("replay-totalviewtcp.port", po::value<unsigned short>(&c.totalviewtcp_replay_port)->default_value(15103))
        ("day.preopen", po::value<int>(&c.day_at[0])->default_value(0))
        ("day.open", po::value<int>(&c.day_at[1])->default_value(30))
        ("day.halt", po::value<int>(&c.day_at[2])->default_value(120))
        ("day.resume", po::value<int>(&c.day_at[3])->default_value(150))
        ("day.preclose", po::value<int>(&c.day_at[4])->default_value(240))
        ("day.close", po::value<int>(&c.day_at[5])->default_value(270))
        ("day.end", po::value<int>(&c.day_at[6])->default_value(300))
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
