# Nasdaq Stock Market Simulator

Generated exchange simulator for Nasdaq Stock Market: an order-entry gateway, a continuous
price-time matching engine, market data feed publishers, the replay services a client logs into (Glimpse, TotalViewTcp),
and traders that exercise the exchange end to end. Every link between two components carries
framed Nasdaq.NsmEquities.MatchingEngine.Ipc.v2026 messages: the gateway owns the client sockets, the engine owns the
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
| Order Entry | Nasdaq.NsmEquities.Orders.Ouch.v5.0 |
| Market Data (TotalView) | Nasdaq.NsmEquities.TotalView.Itch.v5.0.2026 |
| Snapshot of TotalView (Glimpse) | Nasdaq.NsmEquities.TotalView.Glimpse.v5.0 |
| Stream of TotalView (TotalViewTcp) | Nasdaq.NsmEquities.TotalView.Itch.v5.0.2026 |
| Internal Messaging | Nasdaq.NsmEquities.MatchingEngine.Ipc.v2026 |

## Build

Requirements: C++20, CMake 3.28+, Boost

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

## Run

```bash
./build/nasdaq_simulator                # endpoints from simulator.ini
./build/nasdaq_simulator --config my.ini
./build/simulator_tests              # Boost.Test suite
```

Runtime endpoints are read from simulator.ini at process start:

| Connection | Default |
| --- | --- |
| Order entry (Tcp) | 127.0.0.1:15001 |
| TotalView feed (Udp multicast) | 233.54.12.111:26477 |
| Glimpse snapshot (Tcp) | 127.0.0.1:15002 |
| TotalViewTcp stream (Tcp) | 127.0.0.1:15003 |


Recording output (order-entry log, market data captures) lands under `recordings/`.

## Latency

What this venue does under load is measured by the latency lab, which reads a capture of it
running in containers through the venue's own declaration: `omi-latency-lab`, generated from
the same specifications, with a worked example per venue. Nothing of the lab is in this tree.