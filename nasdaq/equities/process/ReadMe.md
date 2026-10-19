# Nasdaq Stock Market — the single binary

One process. Every component runs on its own thread, and every link between them is a lock-free
queue of framed Nasdaq.NsmEquities.MatchingEngine.Ipc.v2026 messages held in this process's own memory: a component hands
bytes to a queue rather than calling the next component.

```
clients --(SoupBin Tcp)--> [gateway] --(ipc)--> [engine] --(ipc)--> [gateway] --> clients
                                                        \--(ipc)--> [feeds] --> Udp + recordings
```

Everything it runs lives in `../common/`: the protocols, the book, the components themselves and
the queues. This directory holds only `main.cpp`, which wires them together — the same components
the per-process version under `../system/` wires to shared-memory rings instead.

## The two modes

Run with no arguments and it takes a **startup run**: a turn is given to one trader, the venue is
allowed to settle, then the next trader acts. Twenty rounds of that, and then it reports what each
trader entered and was told. Because every trader acts alone and the venue settles between turns,
the same orders meet in the same order on every run, whatever the threads' timing — so the counts
it prints are a fixed number that a change either reproduces or does not. **This is the version to
check a change against.**

Run with `--serve` and it is a live venue instead: order entry listens on Tcp, market data goes
out as Udp multicast, the traders run free, and it keeps going until interrupted. Nothing is
deterministic in that mode and nothing is meant to be.

## Build and run

```bash
cmake -S .. -B ../build -DCMAKE_BUILD_TYPE=Release
cmake --build ../build --parallel

../build/nasdaq_simulator                   # the startup run, then a report
../build/nasdaq_simulator --serve           # a live venue, until Ctrl-C
../build/nasdaq_simulator --config my.ini   # endpoints from somewhere else
../build/simulator_tests                 # the Boost.Test suite
```

Endpoints are read from `../simulator.ini` at process start; editing it and restarting is enough,
and no regeneration is needed. Recordings land under `recordings/`.