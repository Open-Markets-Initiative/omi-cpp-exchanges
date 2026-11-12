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
| Investors Exchange Equities Open | [iex/equities/open](iex/equities/open) | Always open | process, system, distributed |
| Investors Exchange Equities Session | [iex/equities/session](iex/equities/session) | Trading day | process, system, distributed |
| Nse Futures and Options | [nse/fo](nse/fo) | Continuous | process, system, distributed |
| Odx Equities Open | [odx/equities/open](odx/equities/open) | Trading day | process, system, distributed |
| Odx Equities Session | [odx/equities/session](odx/equities/session) | Trading day | process, system, distributed |
| Odx Security Token Open | [odx/securitytoken/open](odx/securitytoken/open) | Trading day | process, system, distributed |
| Odx Security Token Session | [odx/securitytoken/session](odx/securitytoken/session) | Trading day | process, system, distributed |

## Protocols

### Investors Exchange Equities Open

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

### Investors Exchange Equities Session

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
| Trading Day | Iex.IexEquities.Equities.Session.Trading |
| Internal Messaging | Iex.IexEquities.MatchingEngine.Ipc.v1.0 |

### Nse Futures and Options

| Role | Model |
| --- | --- |
| Order Entry | Nse.NseFo.OrderEntry.Nnf.v9.50 |
| Order Entry Session | Nse.Nnf |
| Market Data (Mtbt) | Nse.NseFo.Mtbt.Tbt.v7.1 |
| Internal Messaging | Nse.NseFo.MatchingEngine.Ipc.v1.0 |

### Odx Equities Open

| Role | Model |
| --- | --- |
| Order Entry | Odx.OdxEquities.Pts.Ouch.v2.0 |
| Order Entry Session | Nasdaq.SoupBinTcp |
| Market Data (Itch) | Odx.OdxEquities.Pts.Itch.v2.2 |
| Snapshot of Itch (Glimpse) | Odx.OdxEquities.Pts.Glimpse.v2.0 |
| Stream of Itch (ItchTcp) | Odx.OdxEquities.TcpPts.Itch.v2.2 |
| Trading Day | Odx.OdxEquities.Equities.Open.Trading |
| Internal Messaging | Odx.OdxEquities.MatchingEngine.Ipc.v2026 |

### Odx Equities Session

| Role | Model |
| --- | --- |
| Order Entry | Odx.OdxEquities.Pts.Ouch.v2.0 |
| Order Entry Session | Nasdaq.SoupBinTcp |
| Market Data (Itch) | Odx.OdxEquities.Pts.Itch.v2.2 |
| Snapshot of Itch (Glimpse) | Odx.OdxEquities.Pts.Glimpse.v2.0 |
| Stream of Itch (ItchTcp) | Odx.OdxEquities.TcpPts.Itch.v2.2 |
| Trading Day | Odx.OdxEquities.Equities.Session.Trading |
| Internal Messaging | Odx.OdxEquities.MatchingEngine.Ipc.v2026 |

### Odx Security Token Open

| Role | Model |
| --- | --- |
| Order Entry | Odx.OdxSecurityToken.OrderEntry.Fix.v1.1 |
| Order Entry Session | Fix.Fix |
| Market Data (Itch) | Odx.OdxSecurityToken.Pts.Itch.v1.2 |
| Stream of Itch (ItchTcp) | Odx.OdxSecurityToken.Pts.Itch.v1.2 |
| Trading Day | Odx.OdxSecurityToken.SecurityToken.Open.Trading |
| Internal Messaging | Odx.OdxSecurityToken.MatchingEngine.Ipc.v2026 |

### Odx Security Token Session

| Role | Model |
| --- | --- |
| Order Entry | Odx.OdxSecurityToken.OrderEntry.Fix.v1.1 |
| Order Entry Session | Fix.Fix |
| Market Data (Itch) | Odx.OdxSecurityToken.Pts.Itch.v1.2 |
| Stream of Itch (ItchTcp) | Odx.OdxSecurityToken.Pts.Itch.v1.2 |
| Trading Day | Odx.OdxSecurityToken.SecurityToken.Session.Trading |
| Internal Messaging | Odx.OdxSecurityToken.MatchingEngine.Ipc.v2026 |

## Build

Requirements: C++20, CMake 3.28+, Boost

```bash
cmake -S iex/equities/open -B build/iex/equities/open -DCMAKE_BUILD_TYPE=Release
cmake --build build/iex/equities/open --parallel
cmake -S iex/equities/session -B build/iex/equities/session -DCMAKE_BUILD_TYPE=Release
cmake --build build/iex/equities/session --parallel
cmake -S nse/fo -B build/nse/fo -DCMAKE_BUILD_TYPE=Release
cmake --build build/nse/fo --parallel
cmake -S odx/equities/open -B build/odx/equities/open -DCMAKE_BUILD_TYPE=Release
cmake --build build/odx/equities/open --parallel
cmake -S odx/equities/session -B build/odx/equities/session -DCMAKE_BUILD_TYPE=Release
cmake --build build/odx/equities/session --parallel
cmake -S odx/securitytoken/open -B build/odx/securitytoken/open -DCMAKE_BUILD_TYPE=Release
cmake --build build/odx/securitytoken/open --parallel
cmake -S odx/securitytoken/session -B build/odx/securitytoken/session -DCMAKE_BUILD_TYPE=Release
cmake --build build/odx/securitytoken/session --parallel
```

## Run

```bash
./build/iex/equities/open/iex_simulator    # endpoints from iex/equities/open/simulator.ini (--config <file> to override)
./build/iex/equities/open/simulator_tests    # Boost.Test suite
./build/iex/equities/session/iex_simulator    # endpoints from iex/equities/session/simulator.ini (--config <file> to override)
./build/iex/equities/session/simulator_tests    # Boost.Test suite
./build/nse/fo/nse_simulator    # endpoints from nse/fo/simulator.ini (--config <file> to override)
./build/nse/fo/simulator_tests    # Boost.Test suite
./build/odx/equities/open/odx_simulator    # endpoints from odx/equities/open/simulator.ini (--config <file> to override)
./build/odx/equities/open/simulator_tests    # Boost.Test suite
./build/odx/equities/session/odx_simulator    # endpoints from odx/equities/session/simulator.ini (--config <file> to override)
./build/odx/equities/session/simulator_tests    # Boost.Test suite
./build/odx/securitytoken/open/odx_simulator    # endpoints from odx/securitytoken/open/simulator.ini (--config <file> to override)
./build/odx/securitytoken/open/simulator_tests    # Boost.Test suite
./build/odx/securitytoken/session/odx_simulator    # endpoints from odx/securitytoken/session/simulator.ini (--config <file> to override)
./build/odx/securitytoken/session/simulator_tests    # Boost.Test suite
```

Runtime endpoints (IPs/ports for the order-entry gateway, feed multicast groups, and
snapshot servers) are read from each exchange's simulator.ini at process start; edit and
restart to repoint connections, no regeneration required.

## Three ways to run a venue

| Version | Where | What it is |
| --- | --- | --- |
| Single process | `<simulator>/process` | one binary, a thread per component, every link a queue in its own memory. It takes turns and settles between them, so its counts are the same every run - the version a change is measured against. |
| One host | `<simulator>/system` | a process per component, every link a ring in shared memory. `launch.sh` makes the rings and brings the components up; `verify.sh` runs it and checks what they carried. |
| Across machines | `<simulator>/distributed` | a process per component, every link Tcp, the matching engine and recovery each a tier of their own. `compose.yaml` runs it as one container per component. |

Every simulator has the single process; the Deployments column above says which have the others.

## Latency

The latency lab is a repository of its own, `omi-latency-lab`, generated from the same
declarations: it reads a capture of a venue running in containers through the venue's own
address table and framing, and reports the matching engine, the fan out, the feed and each
trader's own turn as percentiles, as plots and on a page. The exchanges carry nothing of it.

## Left out

An exchange whose declarations the generators cannot write is left out rather than half
written:

- **Nasdaq Stock Market Open**: Nasdaq.NsmEquities.MatchingEngine.Ipc.v2026: no value of its Refusal Reason is marked Refusal = Lifetime, so an order refused so could not say why
- **Nasdaq Stock Market Session**: Nasdaq.NsmEquities.MatchingEngine.Ipc.v2026: no value of its Refusal Reason is marked Refusal = Lifetime, so an order refused so could not say why

Any questions, comments or requests: Open-Markets-Initiative@outlook.com