#!/usr/bin/env bash
# The Nasdaq Stock Market venue as separate processes: one per component, joined by sockets, all on this machine.
#
# Which makes it a smoke test of the distributed venue rather than the distributed venue:
# nothing is on its own machine, so nothing can fail on its own. compose.yaml is the one
# where the tiers are really apart.
#
# Generated — the components it starts, and the order it stops them in, come from the venue itself.
#
# Usage: bash launch.sh [build-directory] [component arguments...]
#
# Runs until interrupted. Ctrl-C stops the components in producer order, so that nothing is left
# on a link whose reader has already gone.
set -u

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
build="${1:-../build}"
[ $# -gt 0 ] && shift

if [ ! -x "$build/nasdaq_cluster_gateway" ]; then
    echo "launch: no nasdaq_cluster_* in $build — build the tree first:" >&2
    echo "        cmake -S .. -B $build -DCMAKE_BUILD_TYPE=Release && cmake --build $build --parallel" >&2
    exit 1
fi

cd "$build" || exit 1

# Nothing to create: a socket is made by the component that listens on it, and a writer whose
# reader is not up yet keeps trying rather than failing.

pids=()
names=()

start() {
    echo "launch: starting $1"
    "./nasdaq_cluster_$1" --config "$here/../simulator.ini" "${@:2}" &
    pids+=($!)
    names+=("$1")
}

# Consumers before producers, so nothing writes to a link nobody is reading yet.
start gateway "$@"
start engine "$@"
start totalview "$@"
start glimpse "$@"
start totalviewtcp "$@"
start maker "$@"
start taker "$@"
start replay "$@"
start requests "$@"

# Taking it down. Producer order: each one quiet before whoever reads from it goes, so that
# nothing is still being written to a component that has already stopped.
#
# This runs from the signal handler rather than setting a flag for the loop to notice, because a
# flag is only read between sleeps and a venue should come down when it is asked to.
down() {
    trap - INT TERM

    for index in 8 7 6 5 4 3 2 1 0; do
        pid="${pids[$index]}"
        name="${names[$index]}"
        if kill -0 "$pid" 2>/dev/null; then
            echo "launch: stopping $name"
            kill -INT "$pid" 2>/dev/null
            for _ in $(seq 1 50); do kill -0 "$pid" 2>/dev/null || break; sleep 0.1; done
            kill -9 "$pid" 2>/dev/null
        fi
    done

    wait 2>/dev/null

    echo "launch: down"
    exit 0
}

trap down INT TERM

echo "launch: 9 components up — Ctrl-C to stop"

while :; do
    # A component that dies on its own takes the venue with it: a venue missing one of its parts
    # is not a venue, and carrying on would hide the failure.
    for index in "${!pids[@]}"; do
        if ! kill -0 "${pids[$index]}" 2>/dev/null; then
            echo "launch: ${names[$index]} stopped on its own — bringing the venue down" >&2
            down
        fi
    done
    sleep 1
done