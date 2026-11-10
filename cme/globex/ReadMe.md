# Cme Globex Simulator

Generated exchange simulator for Cme Globex: an order-entry gateway, a continuous price-time matching engine,
market data feed publishers
and traders that exercise the exchange end to end. Every link between two components carries
framed Cme.Globex.MatchingEngine.Ipc.v1.0 messages: the gateway owns the client sockets, the engine owns the
book, and each feed owns its market data.

The components are generated once, into `common/`, and deployed as `process/`, one binary with a thread per component. Each directory has a ReadMe of its own.

## Layout

```
CMakeLists.txt         every target: the single binary and the tests
Dockerfile             the venue as one image
simulator.ini          runtime endpoints, read at process start (docker/ holds the container copy)
common/                everything the deployments share, generated once
  protocols/           the generated structs, messages and sessions of each protocol
  engine/ traders/     the matching engine and the venue's own traders
  run/                 the links: a queue, a ring in shared memory, a socket
process/               one binary, a thread per component - deterministic, the reference
```

`process/` has a ReadMe saying what it is and what it is not.

## Protocols

| Role | Model |
| --- | --- |
| Order Entry | Cme.Globex.iLink3.Sbe.v8.9 |
| Order Entry Session | Cme.iLink3 |
| Internal Messaging | Cme.Globex.MatchingEngine.Ipc.v1.0 |

## Build

Requirements: C++20, CMake 3.28+, Boost

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

## Run

```bash
./build/cme_simulator                # endpoints from simulator.ini
./build/cme_simulator --config my.ini
./build/simulator_tests              # Boost.Test suite
```

Runtime endpoints are read from simulator.ini at process start:

| Connection | Default |
| --- | --- |
| Order entry (Tcp) | 127.0.0.1:11000 |


Recording output (order-entry log, market data captures) lands under `recordings/`.

## Latency

What this venue does under load is measured by the latency lab, which reads a capture of it
running in containers through the venue's own declaration: `omi-latency-lab`, generated from
the same specifications, with a worked example per venue. Nothing of the lab is in this tree.