# Odx Security Token Session Simulator

Generated exchange simulator for Odx Security Token Session: an order-entry gateway, a price-time matching engine running the trading day,
market data feed publishers, the replay services a client logs into (ItchTcp),
and traders that exercise the exchange end to end. Every link between two components carries
framed Odx.OdxSecurityToken.MatchingEngine.Ipc.v2026 messages: the gateway owns the client sockets, the engine owns the
book, and each feed owns its market data.

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
| Order Entry | Odx.OdxSecurityToken.OrderEntry.Fix.v1.1 |
| Order Entry Session | Fix.Fix |
| Market Data (Itch) | Odx.OdxSecurityToken.Pts.Itch.v1.2 |
| Stream of Itch (ItchTcp) | Odx.OdxSecurityToken.Pts.Itch.v1.2 |
| Trading Day | Odx.OdxSecurityToken.SecurityToken.Session.Trading |
| Internal Messaging | Odx.OdxSecurityToken.MatchingEngine.Ipc.v2026 |

## Trading Day

The engine runs `Odx.OdxSecurityToken.SecurityToken.Session.Trading`: each phase of the day decides whether an order
trades, waits for the next cross, or is refused. Each change of phase is announced on the
internal messaging, and so is each cross completed, with the price it crossed at and the
shares that traded there. The day the simulator is generated with:

| Change | Before startup run turn | Served (seconds after start) |
| --- | --- | --- |
| Start | 0 | 0 |
| PreOpen | 1 | 2 |
| NoCancel | 6 | 25 |
| Open | 8 | 30 |
| PreClose | 10 | 35 |
| NoCancel | 15 | 55 |
| Close | 17 | 60 |
| End | 20 | 65 |

A turn past the run's last is made after it. The startup run asks for each change between its
turns, so it is the same every run; a served venue makes them on the clock.

While an auction collects, the day asks the engine what it would cross at, and the engine
answers on the internal messaging with the price, the shares that would pair there, and those
left over and on which side. The startup run asks once a turn; a served venue asks on this
cadence, in seconds:

| Auction | First ask after the collection starts | Then every | Then every, over the last part | Last part |
| --- | --- | --- | --- | --- |
| Opening | 0 | 5 | - | - |
| Closing | 0 | 5 | - | - |

The engine keeps one book and one kind of order, so the price it would cross at is the only
one it has: a feed that publishes several prices for an auction is given that one for each,
which is an approximation.

## Build

Requirements: C++20, CMake 3.28+, Boost, and a standard library with the C++20 time zone database - GCC 14 or newer (g++-14, `-DCMAKE_CXX_COMPILER=g++-14`; Ubuntu 24.04's default g++ is 13) - with tzdata installed

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

## Run

```bash
./build/odx_simulator                # endpoints from simulator.ini
./build/odx_simulator --config my.ini
./build/simulator_tests              # Boost.Test suite
```

Runtime endpoints are read from simulator.ini at process start:

| Connection | Default |
| --- | --- |
| Order entry (Tcp) | 127.0.0.1:17101 |
| Itch feed (Udp multicast) | :0 |
| ItchTcp stream (Tcp) | 127.0.0.1:17103 |

Recording output (order-entry log, market data captures) lands under `recordings/`.

## Latency

What this venue does under load is measured by the latency lab, which reads a capture of it
running in containers through the venue's own declaration: `omi-latency-lab`, generated from
the same specifications, with a worked example per venue. Nothing of the lab is in this tree.