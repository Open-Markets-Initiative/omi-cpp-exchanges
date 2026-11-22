# Nse Futures and Options Basic Simulator

Generated exchange simulator for Nse Futures and Options Basic: an order-entry gateway, an always open price-time matching engine,
market data feed publishers
and traders that exercise the exchange end to end. Every link between two components carries
framed Nse.NseFo.BasicMatchingEngine.Ipc.v2026 messages: the gateway owns the client sockets, the engine owns the
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
| Order Entry | Nse.NseFo.OrderEntry.Nnf.v9.50 |
| Order Entry Session | Nse.Nnf |
| Market Data (Mtbt) | Nse.NseFo.Mtbt.Tbt.v7.1 |
| Always Open | Nse.NseFo.FuturesAndOptions.Basic.TradingSession |
| Internal Messaging | Nse.NseFo.BasicMatchingEngine.Ipc.v2026 |

## Always Open

The engine runs `Nse.NseFo.FuturesAndOptions.Basic.TradingSession`: one phase, whatever the time, in which an order
trades what it crosses and rests the rest, and a cancel takes it out of the book. The chart
answers no change of phase, so nothing asks it one. What it refuses is said on the internal
messaging, and answered to the client whose order it was.
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