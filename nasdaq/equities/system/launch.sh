#!/usr/bin/env bash
# The Nasdaq Stock Market venue as separate processes: one per component, joined by rings in shared memory.
#
# Generated — the components it starts, and the order it stops them in, come from the venue itself.
#
# Usage: bash launch.sh [build-directory] [component arguments...]
#
# Runs until interrupted. Ctrl-C stops the components in producer order, so that nothing is left
# on a link whose reader has already gone, and then removes the rings.
set -u

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
build="${1:-../build}"
[ $# -gt 0 ] && shift

rings="$build/nasdaq_rings"

if [ ! -x "$rings" ]; then
    echo "launch: no nasdaq_rings in $build — build the tree first:" >&2
    echo "        cmake -S .. -B $build -DCMAKE_BUILD_TYPE=Release && cmake --build $build --parallel" >&2
    exit 1
fi

cd "$build" || exit 1

# The rings first: a component only ever attaches, so they have to be there already. The links
# are a cycle, so there is no start order that would let each component make its own.
"./nasdaq_rings" create || exit 1

pids=()
names=()

start() {
    echo "launch: starting $1"
    "./nasdaq_system_$1" "${@:2}" &
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

    # The rings go last, once nothing is holding them.
    "./nasdaq_rings" discard
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