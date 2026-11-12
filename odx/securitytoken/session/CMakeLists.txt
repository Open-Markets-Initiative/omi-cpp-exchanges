cmake_minimum_required(VERSION 3.28)
project(odx_simulator CXX)

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

add_executable(odx_simulator process/main.cpp)
target_include_directories(odx_simulator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})

# Runtime endpoint configuration (config/SimulatorConfiguration.hpp) parses simulator.ini at process start.
find_package(Boost REQUIRED COMPONENTS program_options)
target_link_libraries(odx_simulator PRIVATE Boost::program_options)

# One thread per component (run/Components.hpp): the gateway, the engine and each feed.
find_package(Threads REQUIRED)
target_link_libraries(odx_simulator PRIVATE Threads::Threads)

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
add_executable(odx_rings system/rings/Main.cpp)
target_include_directories(odx_rings PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(odx_rings PRIVATE Threads::Threads)


# A target per component, from the venue's own list - naming them here as well is how one gets
# left out, which is what happened to the recovery services.

add_executable(odx_system_gatewaymakerbasic system/gatewaymakerbasic/Main.cpp)
target_include_directories(odx_system_gatewaymakerbasic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(odx_system_gatewaymakerbasic PRIVATE Boost::program_options Threads::Threads)

add_executable(odx_system_gatewaytakerbasic system/gatewaytakerbasic/Main.cpp)
target_include_directories(odx_system_gatewaytakerbasic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(odx_system_gatewaytakerbasic PRIVATE Boost::program_options Threads::Threads)

add_executable(odx_system_engine system/engine/Main.cpp)
target_include_directories(odx_system_engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(odx_system_engine PRIVATE Boost::program_options Threads::Threads)

add_executable(odx_system_sequencer system/sequencer/Main.cpp)
target_include_directories(odx_system_sequencer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(odx_system_sequencer PRIVATE Boost::program_options Threads::Threads)

add_executable(odx_system_day system/day/Main.cpp)
target_include_directories(odx_system_day PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(odx_system_day PRIVATE Boost::program_options Threads::Threads)

add_executable(odx_system_itch system/itch/Main.cpp)
target_include_directories(odx_system_itch PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(odx_system_itch PRIVATE Boost::program_options Threads::Threads)

add_executable(odx_system_itchtcp system/itchtcp/Main.cpp)
target_include_directories(odx_system_itchtcp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(odx_system_itchtcp PRIVATE Boost::program_options Threads::Threads)

add_executable(odx_system_makerbasic system/makerbasic/Main.cpp)
target_include_directories(odx_system_makerbasic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(odx_system_makerbasic PRIVATE Boost::program_options Threads::Threads)

add_executable(odx_system_takerbasic system/takerbasic/Main.cpp)
target_include_directories(odx_system_takerbasic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(odx_system_takerbasic PRIVATE Boost::program_options Threads::Threads)

add_executable(odx_distributed_gatewaymakerbasic distributed/gatewaymakerbasic/Main.cpp)
target_include_directories(odx_distributed_gatewaymakerbasic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(odx_distributed_gatewaymakerbasic PRIVATE Boost::program_options Threads::Threads)

add_executable(odx_distributed_gatewaytakerbasic distributed/gatewaytakerbasic/Main.cpp)
target_include_directories(odx_distributed_gatewaytakerbasic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(odx_distributed_gatewaytakerbasic PRIVATE Boost::program_options Threads::Threads)

add_executable(odx_distributed_engine distributed/engine/Main.cpp)
target_include_directories(odx_distributed_engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(odx_distributed_engine PRIVATE Boost::program_options Threads::Threads)

add_executable(odx_distributed_sequencer distributed/sequencer/Main.cpp)
target_include_directories(odx_distributed_sequencer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(odx_distributed_sequencer PRIVATE Boost::program_options Threads::Threads)

add_executable(odx_distributed_day distributed/day/Main.cpp)
target_include_directories(odx_distributed_day PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(odx_distributed_day PRIVATE Boost::program_options Threads::Threads)

add_executable(odx_distributed_itch distributed/itch/Main.cpp)
target_include_directories(odx_distributed_itch PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(odx_distributed_itch PRIVATE Boost::program_options Threads::Threads)

add_executable(odx_distributed_itchtcp distributed/itchtcp/Main.cpp)
target_include_directories(odx_distributed_itchtcp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(odx_distributed_itchtcp PRIVATE Boost::program_options Threads::Threads)

add_executable(odx_distributed_makerbasic distributed/makerbasic/Main.cpp)
target_include_directories(odx_distributed_makerbasic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(odx_distributed_makerbasic PRIVATE Boost::program_options Threads::Threads)

add_executable(odx_distributed_takerbasic distributed/takerbasic/Main.cpp)
target_include_directories(odx_distributed_takerbasic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common ${SIM_INCLUDE_DIRS})
target_link_libraries(odx_distributed_takerbasic PRIVATE Boost::program_options Threads::Threads)
