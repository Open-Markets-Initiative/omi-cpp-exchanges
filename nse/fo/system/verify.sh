#!/usr/bin/env bash
# Generated check of the Nse Futures and Options venue as 5 separate processes, joined by rings in shared memory.
#
# Usage: bash verify.sh [build-directory] [seconds]
#
# Brings the venue up through its own launcher, lets it trade, brings it down, and reads what each
# component said on the way out. Exits non-zero if anything did not add up, so it can be a step in
# a build rather than something someone looks at.
set -u

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
build="${1:-../build}"
seconds="${2:-8}"
out="$(mktemp)"
pass=0
fail=0

ok()   { pass=$((pass + 1)); echo "  ok    $1"; }
bad()  { fail=$((fail + 1)); echo "  FAIL  $1"; }

# Did anything say this? Each component names itself, so this is how one asks whether a particular
# component got as far as stopping cleanly.
said()  { if grep -q -- "$1" "$out"; then ok "$2"; else bad "$2 — nothing said '$1'"; fi; }

# And its opposite, for the things that should never be said at all.
quiet() { if grep -q -- "$1" "$out"; then bad "$2 — $(grep -c -- "$1" "$out") line(s)"; else ok "$2"; fi; }

same()    { if [ -n "$2" ] && [ "$2" = "$3" ]; then ok "$1 ($2)"; else bad "$1 — $2 is not $3"; fi; }
atleast() { if [ -n "$2" ] && [ "$2" -ge "$3" ] 2>/dev/null; then ok "$1 ($2)"; else bad "$1 — $2 is not $3 or more"; fi; }

# The nth number on the line that said something, with the saying itself cut off first so that a
# component whose own name has a digit in it does not count as one.
number() { grep -m1 -- "$1" "$out" | sed "s/.*$1//" | grep -oE '[0-9]+' | sed -n "$2p"; }

echo "verify: Nse Futures and Options — system, 5 components, ${seconds}s"

bash "$here/launch.sh" "$build" > "$out" 2>&1 &
launcher=$!

for _ in $(seq 1 80); do grep -q 'components up' "$out" 2>/dev/null && break; sleep 0.25; done

if ! kill -0 $launcher 2>/dev/null; then
    echo "verify: the venue did not come up" >&2
    cat "$out" >&2
    exit 1
fi

sleep "$seconds"

# How many were running before it was asked to stop: a component that died on its own would have
# taken the venue down with it, and that is worth saying as its own failure rather than as a
# missing count further down.
up=$(pgrep -c -f "nse_system_" || true)

# SIGTERM, not SIGINT: a script started as a background job inherits SIGINT ignored, and bash
# cannot trap a signal that was ignored when it started.
kill -TERM $launcher 2>/dev/null
for _ in $(seq 1 120); do kill -0 $launcher 2>/dev/null || break; sleep 0.5; done
sleep 1

trades=$(number "nse-engine: stopped" 1)
events=$(number "nse-engine: stopped" 2)
mtbt_published=$(number "nse-mtbt: stopped" 1)
maker_entered=$(number "nse-maker: stopped" 1)
taker_entered=$(number "nse-taker: stopped" 1)
echo "verify: what the venue did"
same "components up while it ran" "$up" 5
said "launch: down" "the launcher brought it down in order"
said "nse-gateway: stopped" "gateway ran and stopped cleanly"
said "nse-engine: stopped" "engine ran and stopped cleanly"
said "nse-mtbt: stopped" "mtbt ran and stopped cleanly"
said "nse-maker: stopped" "maker ran and stopped cleanly"
said "nse-taker: stopped" "taker ran and stopped cleanly"
echo "verify: what it carried"
atleast "the engine matched trades" "$trades" 1
atleast "the engine emitted events" "$events" 1
same "mtbt published what the engine emitted" "$mtbt_published" "$events"
atleast "maker entered orders" "$maker_entered" 1
atleast "taker entered orders" "$taker_entered" 1

echo "verify: what it did not lose"
quiet "gap(s) on" "no link reported a gap"
same "components left running" "$(pgrep -c -f "nse_system_" || true)" 0
same "rings left in /dev/shm" "$(ls /dev/shm 2>/dev/null | grep -c -E 'gateway|engine|mtbt|maker|taker' || true)" 0

echo "verify: $pass ok, $fail failed"
rm -f "$out"
[ "$fail" -eq 0 ]