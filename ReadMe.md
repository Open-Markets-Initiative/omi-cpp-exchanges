# Omi C++ Exchanges

[![Build](https://github.com/Open-Markets-Initiative/omi-cpp-exchanges/actions/workflows/build.yml/badge.svg)](https://github.com/Open-Markets-Initiative/omi-cpp-exchanges/actions/workflows/build.yml)

[![Omi](https://github.com/Open-Markets-Initiative/Directory/blob/main/About/Images/Logo.png)](https://github.com/Open-Markets-Initiative/Directory)

**Beta**: this repository is an early release - interfaces, generated layouts, and
protocols are still evolving and may change without notice.

Omi C++ exchanges provide generated exchange simulators for common market data and order entry protocols.

Each simulator is a standalone C++ project: an order-entry gateway, a price-time matching engine -
trading continuously, or running the venue's trading day - market data feed publishers, the replay
services a client logs into, and traders that exercise the exchange end to end. The components are
generated once and deployed in up to three ways - one process with a thread per component, a
process per component on one host joined by rings in shared memory, and a process per component
across machines joined by Tcp - with the same framed internal messages on every link, whatever
carries it.

## Exchanges

| Simulator | Directory | Matching | Deployments |
| --- | --- | --- | --- |
| Investors Exchange Equities | [iex/equities](iex/equities) | Continuous | process |

## Protocols

### Investors Exchange Equities

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
| Internal Messaging | Iex.IexEquities.MatchingEngine.Ipc.v1.0 |

## Build

Requirements: C++20, CMake 3.28+, Boost

```bash
cmake -S iex/equities -B build/iex/equities -DCMAKE_BUILD_TYPE=Release
cmake --build build/iex/equities --parallel
```

## Run

```bash
./build/iex/equities/iex_simulator    # endpoints from iex/equities/simulator.ini (--config <file> to override)
./build/iex/equities/simulator_tests    # Boost.Test suite
```

Runtime endpoints (IPs/ports for the order-entry gateway, feed multicast groups, and
snapshot servers) are read from each exchange's simulator.ini at process start; edit and
restart to repoint connections, no regeneration required.

## Three ways to run a venue

| Version | Where | What it is |
| --- | --- | --- |
| Single process | `<simulator>/process` | one binary, a thread per component, every link a queue in its own memory. It takes turns and settles between them, so its counts are the same every run - the version a change is measured against. |
| One host | `<simulator>/system` | a process per component, every link a ring in shared memory. `launch.sh` makes the rings and brings the components up; `verify.sh` runs it and checks what they carried. |
| Across machines | `<simulator>/cluster` | a process per component, every link Tcp, the matching engine and recovery each a tier of their own. `compose.yaml` runs it as one container per component. |

Every simulator has the single process; the Deployments column above says which have the others.

## Latency

The latency lab is a repository of its own, `omi-latency-lab`, generated from the same
declarations: it reads a capture of a venue running in containers through the venue's own
address table and framing, and reports the matching engine, the fan out, the feed and each
trader's own turn as percentiles, as plots and on a page. The exchanges carry nothing of it.

## Left out

An exchange whose declarations the generators cannot write is left out rather than half
written:

- **Nse Futures and Options**: Session model not found: Nse.Nnf (under C:\Users\wwteg\Desktop\Scaled\OmiModels)
- **Nasdaq Stock Market Day**: Session model not found: Nasdaq.SoupBinTcp (under C:\Users\wwteg\Desktop\Scaled\OmiModels)
- **Nasdaq Stock Market Open**: Session model not found: Nasdaq.SoupBinTcp (under C:\Users\wwteg\Desktop\Scaled\OmiModels)

Any questions, comments or requests: Open-Markets-Initiative@outlook.com