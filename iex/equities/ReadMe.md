# Investors Exchange Equities Simulator

Generated exchange simulator for Investors Exchange equities: a FIX order-entry
gateway, a continuous price-time matching engine, market data feed publishers,
snapshot (replay) servers, and trading bots that verify the exchange end to end.
Components communicate over the generated internal messaging protocol
(Iex.IexEquities.MatchingEngine.Ipc.v1.0), so the threading and process model is a deployment choice.

## Protocols

| Role | Model |
| --- | --- |
| Order Entry | Iex.IexEquities.OrderEntry.Fix.v2.91 |
| Market Data (Tops) | Iex.IexEquities.Tops.IexTp.v1.66 |
| Market Data (Deep) | Iex.IexEquities.Deep.IexTp.v1.08 |
| Market Data (DeepPlus) | Iex.IexEquities.DeepPlus.IexTp.v1.04 |
| Snapshot (Deep) | Iex.IexEquities.Deep.Snap.v1.6 |
| Snapshot (Tops) | Iex.IexEquities.Tops.Snap.v1.6 |
| Snapshot (DeepPlus) | Iex.IexEquities.DeepPlus.Snap.v1.05 |
| Internal Messaging | Iex.IexEquities.MatchingEngine.Ipc.v1.0 |

## Components

- FIX wire codec, session (with resend/replay), gateway, and trading bots (SimpleMaker / SimpleTaker)
- Continuous price-time-priority matching engine
- IexTp feed publishers with market data recording and Snap snapshot (replay) servers
- FIX message log: fixed-width bracketed records with the raw message bytes

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

Recording output (FIX log, market data captures) lands under `recordings/`.