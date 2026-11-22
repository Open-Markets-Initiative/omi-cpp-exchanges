#!/usr/bin/env bash
# Break a match the Nasdaq Stock Market Session venue made, as its supervisor:
#   break_trade.sh <match> <reason> [host] [port]
# <match> is the match's number as the venue told its fills, and <reason> one of
#   E  Erroneous (trade deemed clearly erroneous)
#   C  Consent (both parties agreed to break the trade)
#   S  Supervisory (manually broken by supervisory)
#   X  External (trade broken by an external third party)
# The trading day reads its supervisor on the port [supervisor] port of simulator.ini gives, none
# where 0; [port] is that one unless named, and [host] this machine. It prints the day's answer -
# asked, or why not - and fails unless the break was asked. The engine refuses a number it gave
# no match, or one already broken, and says nothing of it.
set -euo pipefail
usage="usage: break_trade.sh <match> <reason> [host] [port]"
match="${1:?$usage}"
reason="${2:?$usage}"
host="${3:-127.0.0.1}"
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
port="${4:-$(awk -F'=' '/^\[supervisor\]/ { inside = 1; next } /^\[/ { inside = 0 } inside && $1 ~ /^ *port *$/ { gsub(/ /, "", $2); print $2 }' "$here/simulator.ini" 2>/dev/null || true)}"
if [ -z "$port" ] || [ "$port" = 0 ]; then
    echo "no supervisor port: set [supervisor] port in simulator.ini, or name one" >&2
    exit 1
fi
exec 3<>"/dev/tcp/$host/$port"
printf 'break %s %s\n' "$match" "$reason" >&3
answer="no answer"
read -r -t 5 answer <&3 || true
exec 3>&-
echo "$answer"
[ "$answer" = asked ]