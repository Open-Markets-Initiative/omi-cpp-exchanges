#pragma once

#include <boost/program_options.hpp>
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
    Endpoint mtbt_feed;
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
        ("order-entry.port", po::value<unsigned short>(&c.order_entry.port)->default_value(10900))
        ("feed-mtbt.group", po::value<std::string>(&c.mtbt_feed.host)->default_value("239.70.70.61"))
        ("feed-mtbt.port", po::value<unsigned short>(&c.mtbt_feed.port)->default_value(17761))
        ("recording.directory", po::value<std::string>(&c.recording_directory)->default_value("recordings"))
        ("clock.mode", po::value<std::string>(&c.clock_mode)->default_value("Elapsed"))
        ("clock.start", po::value<std::string>(&c.clock_start)->default_value("09:15"))
        ("clock.zone", po::value<std::string>(&c.clock_zone)->default_value("Asia/Kolkata"));

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
