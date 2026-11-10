# Investors Exchange Equities Open Simulator

Generated exchange simulator for Investors Exchange Equities Open: an order-entry gateway, an always open price-time matching engine,
market data feed publishers, snapshot (replay) servers,
and traders that exercise the exchange end to end. Every link between two components carries
framed Iex.IexEquities.MatchingEngine.Ipc.v1.0 messages: the gateway owns the client sockets, the engine owns the
book, and each feed owns its market data and the snapshots it serves.

The components are generated once, into `common/`, and deployed three ways - `process/`, one binary with a thread per component, `system/`, a process per component on this host, `distributed/`, a process per component across machines. Each directory has a ReadMe of its own.

## Layout

```
CMakeLists.txt         every target: the single binary, a binary per component, the tests
Dockerfile             the venue as one image; distributed/compose.yaml runs a component per container
simulator.ini          runtime endpoints, read at process start (docker/ holds the container copy)
common/                everything the deployments share, generated once
  protocols/           the generated structs, messages and sessions of each protocol
  engine/ traders/     the matching engine and the venue's own traders
  run/                 the links: a queue, a ring in shared memory, a socket
process/               one binary, a thread per component - deterministic, the reference
system/                a process per component on one host, rings in shared memory
distributed/           a process per component across machines, Tcp, tiers that fail apart
```

Each of `process/`, `system/`, `distributed/` has a ReadMe saying what it is and what it is not; the separated ones have a `launch.sh`
and a `verify.sh` that runs the venue and checks what its components carried.

## Protocols

| Role | Model |
| --- | --- |
| Order Entry | Iex.IexEquities.OrderEntry.Fix.v2.91 |
| Order Entry Session | Fix.Fix |
| Market Data (Tops) | Iex.IexEquities.Tops.IexTp.v1.66 |
| Market Data (Deep) | Iex.IexEquities.Deep.IexTp.v1.08 |
| Market Data (DeepPlus) | Iex.IexEquities.DeepPlus.IexTp.v1.05 |
| Snapshot (Deep) | Iex.IexEquities.Deep.Snap.v1.6 |
| Snapshot (Tops) | Iex.IexEquities.Tops.Snap.v1.6 |
| Snapshot (DeepPlus) | Iex.IexEquities.DeepPlus.Snap.v1.06 |
| Always Open | Iex.IexEquities.Equities.Open.Trading |
| Internal Messaging | Iex.IexEquities.MatchingEngine.Ipc.v1.0 |

## Always Open

The engine runs `Iex.IexEquities.Equities.Open.Trading`: one phase, whatever the time, in which an order
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
./build/iex_simulator                # endpoints from simulator.ini
./build/iex_simulator --config my.ini
./build/simulator_tests              # Boost.Test suite
```

Runtime endpoints are read from simulator.ini at process start:

| Connection | Default |
| --- | --- |
| Order entry (Tcp) | 127.0.0.1:9001 |
| Tops feed (Udp multicast) | 233.54.12.1:10001 |
| Deep feed (Udp multicast) | 233.54.12.2:10002 |
| DeepPlus feed (Udp multicast) | 233.54.12.3:10003 |
| Deep snapshot (Tcp) | port 11379 |
| Tops snapshot (Tcp) | port 11380 |
| DeepPlus snapshot (Tcp) | port 11381 |

Recording output (order-entry log, market data captures) lands under `recordings/`.

## Latency

What this venue does under load is measured by the latency lab, which reads a capture of it
running in containers through the venue's own declaration: `omi-latency-lab`, generated from
the same specifications, with a worked example per venue. Nothing of the lab is in this tree.