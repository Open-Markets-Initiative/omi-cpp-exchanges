cmake_minimum_required(VERSION 3.28)
project(nasdaq_simulator CXX)

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

add_executable(nasdaq_simulator process/main.cpp)
target_include_directories(nasdaq_simulator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})

# Runtime endpoint configuration (config/SimulatorConfiguration.hpp) parses simulator.ini at process start.
find_package(Boost REQUIRED COMPONENTS program_options)
target_link_libraries(nasdaq_simulator PRIVATE Boost::program_options)

# One thread per component (run/Components.hpp): the gateway, the engine and each feed.
find_package(Threads REQUIRED)
target_link_libraries(nasdaq_simulator PRIVATE Threads::Threads)

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
add_executable(nasdaq_rings system/rings/Main.cpp)
target_include_directories(nasdaq_rings PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nasdaq_rings PRIVATE Threads::Threads)


# A target per component, from the venue's own list - naming them here as well is how one gets
# left out, which is what happened to the recovery services.

add_executable(nasdaq_system_gateway system/gateway/Main.cpp)
target_include_directories(nasdaq_system_gateway PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nasdaq_system_gateway PRIVATE Boost::program_options Threads::Threads)

add_executable(nasdaq_system_engine system/engine/Main.cpp)
target_include_directories(nasdaq_system_engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nasdaq_system_engine PRIVATE Boost::program_options Threads::Threads)

add_executable(nasdaq_system_totalview system/totalview/Main.cpp)
target_include_directories(nasdaq_system_totalview PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nasdaq_system_totalview PRIVATE Boost::program_options Threads::Threads)

add_executable(nasdaq_system_glimpse system/glimpse/Main.cpp)
target_include_directories(nasdaq_system_glimpse PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nasdaq_system_glimpse PRIVATE Boost::program_options Threads::Threads)

add_executable(nasdaq_system_totalviewtcp system/totalviewtcp/Main.cpp)
target_include_directories(nasdaq_system_totalviewtcp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nasdaq_system_totalviewtcp PRIVATE Boost::program_options Threads::Threads)

add_executable(nasdaq_system_maker system/maker/Main.cpp)
target_include_directories(nasdaq_system_maker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nasdaq_system_maker PRIVATE Boost::program_options Threads::Threads)

add_executable(nasdaq_system_taker system/taker/Main.cpp)
target_include_directories(nasdaq_system_taker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nasdaq_system_taker PRIVATE Boost::program_options Threads::Threads)

add_executable(nasdaq_system_replay system/replay/Main.cpp)
target_include_directories(nasdaq_system_replay PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nasdaq_system_replay PRIVATE Boost::program_options Threads::Threads)

add_executable(nasdaq_system_requests system/requests/Main.cpp)
target_include_directories(nasdaq_system_requests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nasdaq_system_requests PRIVATE Boost::program_options Threads::Threads)

add_executable(nasdaq_cluster_gateway cluster/gateway/Main.cpp)
target_include_directories(nasdaq_cluster_gateway PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nasdaq_cluster_gateway PRIVATE Boost::program_options Threads::Threads)

add_executable(nasdaq_cluster_engine cluster/engine/Main.cpp)
target_include_directories(nasdaq_cluster_engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nasdaq_cluster_engine PRIVATE Boost::program_options Threads::Threads)

add_executable(nasdaq_cluster_totalview cluster/totalview/Main.cpp)
target_include_directories(nasdaq_cluster_totalview PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nasdaq_cluster_totalview PRIVATE Boost::program_options Threads::Threads)

add_executable(nasdaq_cluster_glimpse cluster/glimpse/Main.cpp)
target_include_directories(nasdaq_cluster_glimpse PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nasdaq_cluster_glimpse PRIVATE Boost::program_options Threads::Threads)

add_executable(nasdaq_cluster_totalviewtcp cluster/totalviewtcp/Main.cpp)
target_include_directories(nasdaq_cluster_totalviewtcp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nasdaq_cluster_totalviewtcp PRIVATE Boost::program_options Threads::Threads)

add_executable(nasdaq_cluster_maker cluster/maker/Main.cpp)
target_include_directories(nasdaq_cluster_maker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nasdaq_cluster_maker PRIVATE Boost::program_options Threads::Threads)

add_executable(nasdaq_cluster_taker cluster/taker/Main.cpp)
target_include_directories(nasdaq_cluster_taker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nasdaq_cluster_taker PRIVATE Boost::program_options Threads::Threads)

add_executable(nasdaq_cluster_replay cluster/replay/Main.cpp)
target_include_directories(nasdaq_cluster_replay PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nasdaq_cluster_replay PRIVATE Boost::program_options Threads::Threads)

add_executable(nasdaq_cluster_requests cluster/requests/Main.cpp)
target_include_directories(nasdaq_cluster_requests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nasdaq_cluster_requests PRIVATE Boost::program_options Threads::Threads)
