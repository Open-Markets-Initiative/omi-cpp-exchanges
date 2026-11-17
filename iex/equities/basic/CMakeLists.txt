cmake_minimum_required(VERSION 3.28)
project(iex_simulator CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Header-only components live in subdirectories (fix/ matching/ feed/) alongside the
# Advanced write-path headers (structs/ types/ messages/) and the per-feed dirs. Put the
# project root and every subdirectory on the include path so headers resolve however
# they're spelled (bare "FixWire.hpp" or "deep/messages/...").
file(GLOB SIM_ENTRIES LIST_DIRECTORIES true ${CMAKE_CURRENT_SOURCE_DIR}/common/*)
set(SIM_INCLUDE_DIRS "")
foreach(entry ${SIM_ENTRIES})
    if(IS_DIRECTORY ${entry} AND NOT entry MATCHES "/build$")
        list(APPEND SIM_INCLUDE_DIRS ${entry})
    endif()
endforeach()

add_executable(iex_simulator process/main.cpp)
target_include_directories(iex_simulator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})

# Runtime endpoint configuration (config/SimulatorConfiguration.hpp) parses simulator.ini at process start.
find_package(Boost REQUIRED COMPONENTS program_options)
target_link_libraries(iex_simulator PRIVATE Boost::program_options)

# One thread per component (run/Components.hpp): the gateway, the engine and each feed.
find_package(Threads REQUIRED)
target_link_libraries(iex_simulator PRIVATE Threads::Threads)

# Boost.Test suite over the generated components (matching / wire / session replay / feeds).
# Header-only variant (boost/test/included/unit_test.hpp provides main + framework), so we
# only need the Boost headers on the include path — no unit_test_framework lib to link.
find_package(Boost REQUIRED)
enable_testing()
add_executable(simulator_tests common/tests/SimulatorTests.cpp)
target_include_directories(simulator_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(simulator_tests PRIVATE Boost::headers Threads::Threads)
add_test(NAME simulator_tests COMMAND simulator_tests)

# The per-component version (system/): every component is its own process, and the links between
# them are rings in shared memory rather than queues in one process's own. The rings have to exist
# before any component attaches - the links are a cycle, so there is no start order that would let
# each component make its own - which is what this binary is for.


# A target per component, from the venue's own list - naming them here as well is how one gets
# left out, which is what happened to the recovery services.
