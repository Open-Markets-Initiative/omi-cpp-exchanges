# Nasdaq Stock Market Day Simulator

Generated exchange simulator for Nasdaq Stock Market Day: an order-entry gateway, a price-time matching engine running the trading day,
market data feed publishers, the replay services a client logs into (Glimpse, TotalViewTcp),
and traders that exercise the exchange end to end. Every link between two components carries
framed Nasdaq.NsmEquities.MatchingEngine.Ipc.v2026 messages: the gateway owns the client sockets, the engine owns the
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
| Order Entry | Nasdaq.NsmEquities.Orders.Ouch.v5.0 |
| Order Entry Session | Nasdaq.SoupBinTcp |
| Market Data (TotalView) | Nasdaq.NsmEquities.TotalView.Itch.v5.0.2026 |
| Snapshot of TotalView (Glimpse) | Nasdaq.NsmEquities.TotalView.Glimpse.v5.0 |
| Stream of TotalView (TotalViewTcp) | Nasdaq.NsmEquities.TotalView.Itch.v5.0.2026 |
| Trading Day | Nasdaq.NsmEquities.Equities.Trading |
| Internal Messaging | Nasdaq.NsmEquities.MatchingEngine.Ipc.v2026 |

## Trading Day

The engine runs `Nasdaq.NsmEquities.Equities.Trading`: each phase of the day decides whether an order
trades, waits for the next cross, or is refused. Each change of phase is announced on the
internal messaging, and so is each cross completed, with the price it crossed at and the
shares that traded there. The day the simulator is generated with:

| Change | Before startup run turn | Served (seconds after start) |
| --- | --- | --- |
| PreOpen | 0 | 0 |
| Open | 4 | 30 |
| Halt | 8 | 120 |
| Resume | 10 | 150 |
| PreClose | 14 | 240 |
| Close | 18 | 270 |
| End | 20 | 300 |

A turn past the run's last is made after it. The startup run asks for each change between its
turns, so it is the same every run; a served venue makes them on the clock.

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
| Order entry (Tcp) | 127.0.0.1:15101 |
| TotalView feed (Udp multicast) | 233.54.12.112:26478 |
| Glimpse snapshot (Tcp) | 127.0.0.1:15102 |
| TotalViewTcp stream (Tcp) | 127.0.0.1:15103 |

Recording output (order-entry log, market data captures) lands under `recordings/`.

## Latency

What this venue does under load is measured by the latency lab, which reads a capture of it
running in containers through the venue's own declaration: `omi-latency-lab`, generated from
the same specifications, with a worked example per venue. Nothing of the lab is in this tree.