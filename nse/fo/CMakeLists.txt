cmake_minimum_required(VERSION 3.28)
project(nse_simulator CXX)

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

add_executable(nse_simulator process/main.cpp)
target_include_directories(nse_simulator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})

# Runtime endpoint configuration (config/SimulatorConfiguration.hpp) parses simulator.ini at process start.
find_package(Boost REQUIRED COMPONENTS program_options)
target_link_libraries(nse_simulator PRIVATE Boost::program_options)

# One thread per component (run/Components.hpp): the gateway, the engine and each feed.
find_package(Threads REQUIRED)
target_link_libraries(nse_simulator PRIVATE Threads::Threads)

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
add_executable(nse_rings system/rings/Main.cpp)
target_include_directories(nse_rings PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nse_rings PRIVATE Threads::Threads)


# A target per component, from the venue's own list - naming them here as well is how one gets
# left out, which is what happened to the recovery services.

add_executable(nse_system_gateway system/gateway/Main.cpp)
target_include_directories(nse_system_gateway PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nse_system_gateway PRIVATE Boost::program_options Threads::Threads)

add_executable(nse_system_engine system/engine/Main.cpp)
target_include_directories(nse_system_engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nse_system_engine PRIVATE Boost::program_options Threads::Threads)

add_executable(nse_system_mtbt system/mtbt/Main.cpp)
target_include_directories(nse_system_mtbt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nse_system_mtbt PRIVATE Boost::program_options Threads::Threads)

add_executable(nse_system_maker system/maker/Main.cpp)
target_include_directories(nse_system_maker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nse_system_maker PRIVATE Boost::program_options Threads::Threads)

add_executable(nse_system_taker system/taker/Main.cpp)
target_include_directories(nse_system_taker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nse_system_taker PRIVATE Boost::program_options Threads::Threads)

add_executable(nse_cluster_gateway cluster/gateway/Main.cpp)
target_include_directories(nse_cluster_gateway PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nse_cluster_gateway PRIVATE Boost::program_options Threads::Threads)

add_executable(nse_cluster_engine cluster/engine/Main.cpp)
target_include_directories(nse_cluster_engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nse_cluster_engine PRIVATE Boost::program_options Threads::Threads)

add_executable(nse_cluster_mtbt cluster/mtbt/Main.cpp)
target_include_directories(nse_cluster_mtbt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nse_cluster_mtbt PRIVATE Boost::program_options Threads::Threads)

add_executable(nse_cluster_maker cluster/maker/Main.cpp)
target_include_directories(nse_cluster_maker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nse_cluster_maker PRIVATE Boost::program_options Threads::Threads)

add_executable(nse_cluster_taker cluster/taker/Main.cpp)
target_include_directories(nse_cluster_taker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(nse_cluster_taker PRIVATE Boost::program_options Threads::Threads)
