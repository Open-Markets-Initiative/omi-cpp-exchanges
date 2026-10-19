#!/usr/bin/env python3
# Integration test: bring the exchange up (--serve) and verify every connection.
import socket, struct, subprocess, sys, time, os

BINARY = os.environ.get("IEX_SIMULATOR", "./build/iex_simulator")
ORDER_ENTRY = ("127.0.0.1", 9001)
# (name, group, port, expected_to_publish)
FEEDS = [
    ("Tops", "233.54.12.1", 10001, True),
    ("Deep", "233.54.12.2", 10002, True),
    ("DeepPlus", "233.54.12.3", 10003, True),
]
SNAPS = [
    ("Deep", 11379),
    ("Tops", 11380),
    ("DeepPlus", 11381),
]

failures = []

def check(name, ok, detail=""):
    print(f"  [{'PASS' if ok else 'FAIL'}] {name} {detail}")
    if not ok:
        failures.append(name)

# Bring the exchange up in serve mode.
proc = subprocess.Popen([BINARY, "--serve"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
time.sleep(1.5)

try:
    # 1) Market data: join each feed's multicast group and receive at least one segment.
    for name, group, port, expected in FEEDS:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind(("", port))
        mreq = struct.pack("4sl", socket.inet_aton(group), socket.INADDR_ANY)
        s.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
        s.settimeout(3)
        try:
            data, _ = s.recvfrom(2048)
            check(f"multicast {name} ({group}:{port})", len(data) >= 40, f"{len(data)} bytes")
        except socket.timeout:
            if expected:
                check(f"multicast {name} ({group}:{port})", False, "no datagrams")
            else:
                print(f"  [SKIP] multicast {name} ({group}:{port}) — no publisher wired")
        finally:
            s.close()

    # 2) Order entry: connect and submit a FIX NewOrderSingle (framed 8=/9=/35=D/.../10=).
    try:
        c = socket.create_connection(ORDER_ENTRY, timeout=3)
        body = b"35=D\x0134=1\x0111=999\x0138=100\x0140=2\x0144=10000\x0154=1\x01"
        head = b"8=FIXT.1.1\x019=" + str(len(body)).encode() + b"\x01"
        checksum = sum(head + body) % 256
        msg = head + body + b"10=%03d\x01" % checksum
        c.sendall(msg)
        check("order-entry connect + NewOrderSingle", True, f"{ORDER_ENTRY[0]}:{ORDER_ENTRY[1]}")
        c.close()
    except OSError as error:
        check("order-entry connect + NewOrderSingle", False, str(error))

    # 3) Snapshot: connect to each Snap server, request, and read Start/Data/End.
    for name, port in SNAPS:
        try:
            c = socket.create_connection(("127.0.0.1", port), timeout=3)
            # SnapshotRequest: [len u16][type 'r'][token40][channel4][session4][minseq8]
            payload = b"r" + b"\x00" * (40 + 4 + 4 + 8)
            c.sendall(struct.pack("<H", len(payload)) + payload)
            c.settimeout(3)
            buf = b""
            try:
                while b"x" not in buf[-1:] or True:   # read until we've framed an End ('x')
                    part = c.recv(65536)
                    if not part:
                        break
                    buf += part
                    # stop once the last complete frame is SnapshotEnd
                    j, last = 0, None
                    while j + 3 <= len(buf):
                        ml = struct.unpack_from("<H", buf, j)[0]
                        if j + 2 + ml > len(buf):
                            break
                        last = chr(buf[j + 2]); j += 2 + ml
                    if last == "x":
                        break
            except socket.timeout:
                pass
            types, i = [], 0
            while i + 3 <= len(buf):
                mlen = struct.unpack_from("<H", buf, i)[0]
                if i + 2 + mlen > len(buf):
                    break
                types.append(chr(buf[i + 2])); i += 2 + mlen
            ok = len(types) >= 2 and types[0] == "s" and types[-1] == "x"
            check(f"snapshot {name} (port {port})", ok, f"types={''.join(types[:6])}{'...' if len(types)>6 else ''}")
            c.close()
        except OSError as error:
            check(f"snapshot {name} (port {port})", False, str(error))

finally:
    proc.terminate()
    try:
        proc.wait(timeout=5)
    except subprocess.TimeoutExpired:
        proc.kill()

print()
if failures:
    print(f"FAILED: {len(failures)} connection test(s): {', '.join(failures)}")
    sys.exit(1)
print("All connection tests passed.")