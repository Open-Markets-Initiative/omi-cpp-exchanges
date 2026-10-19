# Omi C++ Exchanges

[![Build](https://github.com/Open-Markets-Initiative/omi-cpp-exchanges/actions/workflows/build.yml/badge.svg)](https://github.com/Open-Markets-Initiative/omi-cpp-exchanges/actions/workflows/build.yml)

[![Omi](https://github.com/Open-Markets-Initiative/Directory/blob/main/About/Images/Logo.png)](https://github.com/Open-Markets-Initiative/Directory)

**Beta**: this repository is an early release - interfaces, generated layouts, and
protocols are still evolving and may change without notice.

Omi C++ exchanges provide generated exchange simulators for common market data and order entry protocols.

Each exchange generates a standalone C++ project: an order-entry gateway, a continuous
price-time matching engine, market data feed publishers, the replay services a client logs
into, and traders that exercise the exchange end to end. The components are generated once and
deployed three ways - one process with a thread per component, a process per component on one
host joined by rings in shared memory, and a process per component across machines joined by
Tcp - with the same framed internal messages on every link, whatever carries it.

## Exchanges

| Exchange | Directory | Order Entry | Market Data | Snapshot |
| --- | --- | --- | --- | --- |
| Investors Exchange Equities | [iex/equities](iex/equities) | Iex.IexEquities.OrderEntry.Fix.v2.91 | Iex.IexEquities.Tops.IexTp.v1.66, Iex.IexEquities.Deep.IexTp.v1.08, Iex.IexEquities.DeepPlus.IexTp.v1.05 | Iex.IexEquities.Deep.Snap.v1.6, Iex.IexEquities.Tops.Snap.v1.6, Iex.IexEquities.DeepPlus.Snap.v1.06 |
| Nse Futures and Options | [nse/fo](nse/fo) | Nse.NseFo.OrderEntry.Nnf.v9.50 | Nse.NseFo.Mtbt.Binary.v7.1 |  |
| Nasdaq Stock Market | [nasdaq/equities](nasdaq/equities) | Nasdaq.NsmEquities.Orders.Ouch.v5.0 | Nasdaq.NsmEquities.TotalView.Itch.v5.0.2026 |  |

## Build

Requirements: C++20, CMake 3.28+, Boost

```bash
cmake -S iex/equities -B build/iex -DCMAKE_BUILD_TYPE=Release
cmake --build build/iex --parallel
cmake -S nse/fo -B build/nse -DCMAKE_BUILD_TYPE=Release
cmake --build build/nse --parallel
cmake -S nasdaq/equities -B build/nasdaq -DCMAKE_BUILD_TYPE=Release
cmake --build build/nasdaq --parallel
```

## Run

```bash
./build/iex/iex_simulator    # endpoints from iex/equities/simulator.ini (--config <file> to override)
./build/iex/simulator_tests    # Boost.Test suite
./build/nse/nse_simulator    # endpoints from nse/fo/simulator.ini (--config <file> to override)
./build/nse/simulator_tests    # Boost.Test suite
./build/nasdaq/nasdaq_simulator    # endpoints from nasdaq/equities/simulator.ini (--config <file> to override)
./build/nasdaq/simulator_tests    # Boost.Test suite
```

Runtime endpoints (IPs/ports for the order-entry gateway, feed multicast groups, and
snapshot servers) are read from each exchange's simulator.ini at process start; edit and
restart to repoint connections, no regeneration required.

## Three ways to run a venue

| Version | Where | What it is |
| --- | --- | --- |
| Single process | `<venue>/process` | one binary, a thread per component, every link a queue in its own memory. It takes turns and settles between them, so its counts are the same every run - the version a change is measured against. |
| One host | `<venue>/system` | a process per component, every link a ring in shared memory. `launch.sh` makes the rings and brings the components up; `verify.sh` runs it and checks what they carried. |
| Across machines | `<venue>/cluster` | a process per component, every link Tcp, the matching engine and recovery each a tier of their own. `compose.yaml` runs it as one container per component. |

## Latency

The latency lab is a repository of its own, `omi-latency-lab`, generated from the same
declarations: it reads a capture of a venue running in containers through the venue's own
address table and framing, and reports the matching engine, the fan out, the feed and each
trader's own turn as percentiles, as plots and on a page. The exchanges carry nothing of it.

Any questions, comments or requests: Open-Markets-Initiative@outlook.com

Hydra