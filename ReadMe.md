# Omi C++ Exchanges

[![Build](https://github.com/Open-Markets-Initiative/omi-cpp-exchanges/actions/workflows/build.yml/badge.svg)](https://github.com/Open-Markets-Initiative/omi-cpp-exchanges/actions/workflows/build.yml)

[![Omi](https://github.com/Open-Markets-Initiative/Directory/blob/main/About/Images/Logo.png)](https://github.com/Open-Markets-Initiative/Directory)

**Beta**: this repository is an early release - interfaces, generated layouts, and
protocols are still evolving and may change without notice.

Omi C++ exchanges provide generated exchange simulators for common market data and order entry protocols.

Each exchange generates a standalone C++ project: a FIX order-entry gateway, a continuous
price-time matching engine, market data feed publishers, snapshot (replay) servers, and
trading bots that verify the exchange end to end. Components communicate over a generated
internal messaging protocol, so the threading and process model is a deployment choice.

## Exchanges

| Exchange | Order Entry | Market Data | Snapshot |
| --- | --- | --- | --- |
| Investors Exchange Equities | Iex.IexEquities.OrderEntry.Fix.v2.91 | Iex.IexEquities.Tops.IexTp.v1.66, Iex.IexEquities.Deep.IexTp.v1.08, Iex.IexEquities.DeepPlus.IexTp.v1.04 | Iex.IexEquities.Deep.Snap.v1.6, Iex.IexEquities.Tops.Snap.v1.6, Iex.IexEquities.DeepPlus.Snap.v1.05 |

## Build

Requirements: C++20, CMake 3.28+, Boost

```bash
cd iex/equities
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

## Run

```bash
./build/iex_simulator                # endpoints from simulator.ini (--config <file> to override)
./build/simulator_tests              # Boost.Test suite
```

Runtime endpoints (IPs/ports for the order-entry gateway, feed multicast groups, and
snapshot servers) are read from simulator.ini at process start; edit and restart to
repoint connections, no regeneration required.

Any questions, comments or requests: Open-Markets-Initiative@outlook.com

Hydra