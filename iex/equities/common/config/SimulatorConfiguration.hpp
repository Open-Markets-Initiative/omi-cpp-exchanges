#pragma once

#include <boost/program_options.hpp>
#include <fstream>
#include <sstream>
#include <string>

namespace sim {

struct Endpoint { std::string host; unsigned short port; };

struct RuntimeConfig {
    Endpoint order_entry;
    Endpoint tops_feed;
    Endpoint deep_feed;
    Endpoint deepplus_feed;
    unsigned short deep_snap_port;
    unsigned short tops_snap_port;
    unsigned short deepplus_snap_port;
    std::string recording_directory;
};

// Parse [--config <file>] (default simulator.ini); missing file / keys keep the generated defaults.
inline RuntimeConfig load_config(int argc, char** argv) {
    namespace po = boost::program_options;
    RuntimeConfig c;

    po::options_description options("simulator");
    options.add_options()
        ("order-entry.host", po::value<std::string>(&c.order_entry.host)->default_value("127.0.0.1"))
        ("order-entry.port", po::value<unsigned short>(&c.order_entry.port)->default_value(9001))
        ("feed-tops.group", po::value<std::string>(&c.tops_feed.host)->default_value("233.54.12.1"))
        ("feed-tops.port", po::value<unsigned short>(&c.tops_feed.port)->default_value(10001))
        ("feed-deep.group", po::value<std::string>(&c.deep_feed.host)->default_value("233.54.12.2"))
        ("feed-deep.port", po::value<unsigned short>(&c.deep_feed.port)->default_value(10002))
        ("feed-deepplus.group", po::value<std::string>(&c.deepplus_feed.host)->default_value("233.54.12.3"))
        ("feed-deepplus.port", po::value<unsigned short>(&c.deepplus_feed.port)->default_value(10003))
        ("snap-deep.port", po::value<unsigned short>(&c.deep_snap_port)->default_value(11379))
        ("snap-tops.port", po::value<unsigned short>(&c.tops_snap_port)->default_value(11380))
        ("snap-deepplus.port", po::value<unsigned short>(&c.deepplus_snap_port)->default_value(11381))
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
