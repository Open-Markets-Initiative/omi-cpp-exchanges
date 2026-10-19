# Nse Futures and Options Simulator

Generated exchange simulator for Nse Futures and Options: an order-entry gateway, a continuous
price-time matching engine, market data feed publishers
and traders that exercise the exchange end to end. Every link between two components carries
framed Nse.NseFo.MatchingEngine.Ipc.v1.0 messages: the gateway owns the client sockets, the engine owns the
book, and each feed owns its market data.

The components are generated once, into `common/`, and deployed three ways - `process/` as one
binary, `system/` as a process per component on this host, `cluster/` as a process per component
across machines. Each directory has a ReadMe of its own.

## Layout

```
CMakeLists.txt         every target: the single binary, a binary per component, the tests
Dockerfile             the venue as one image; cluster/compose.yaml runs a component per container
simulator.ini          runtime endpoints, read at process start (docker/ holds the container copy)
common/                everything the three deployments share, generated once
  protocols/           the generated structs, messages and sessions of each protocol
  engine/ traders/     the matching engine and the venue's own traders
  run/                 the links: a queue, a ring in shared memory, a socket
process/               one binary, a thread per component - deterministic, the reference
system/                a process per component on one host, rings in shared memory
cluster/               a process per component across machines, Tcp, tiers that fail apart
```

Each of `process/`, `system/` and `cluster/` has a ReadMe saying what it is and what it is not;
the separated two have a `launch.sh` and a `verify.sh` that runs the venue and checks what its
components carried.

## Protocols

| Role | Model |
| --- | --- |
| Order Entry | Nse.NseFo.OrderEntry.Nnf.v9.50 |
| Market Data (Mtbt) | Nse.NseFo.Mtbt.Binary.v7.1 |
| Internal Messaging | Nse.NseFo.MatchingEngine.Ipc.v1.0 |

## Build

Requirements: C++20, CMake 3.28+, Boost

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

## Run

```bash
./build/nse_simulator                # endpoints from simulator.ini
./build/nse_simulator --config my.ini
./build/simulator_tests              # Boost.Test suite
```

Runtime endpoints are read from simulator.ini at process start:

| Connection | Default |
| --- | --- |
| Order entry (Tcp) | 127.0.0.1:10900 |
| Mtbt feed (Udp multicast) | 239.70.70.61:17761 |


Recording output (order-entry log, market data captures) lands under `recordings/`.

## Latency

What this venue does under load is measured by the latency lab, which reads a capture of it
running in containers through the venue's own declaration: `omi-latency-lab`, generated from
the same specifications, with a worked example per venue. Nothing of the lab is in this tree.