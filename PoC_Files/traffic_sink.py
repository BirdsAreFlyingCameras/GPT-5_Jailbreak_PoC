#!/usr/bin/env python3
import socket
import time
import threading
import os

# counters
rx_pkts = 0
rx_bytes = 0

def udp_sink(port):
    global rx_pkts, rx_bytes
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("0.0.0.0", port))
    print(f"[*] UDP sink listening on 0.0.0.0:{port}")
    while True:
        data, _ = sock.recvfrom(65535)
        rx_pkts += 1
        rx_bytes += len(data)

def tcp_sink(port):
    global rx_pkts, rx_bytes
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind(("0.0.0.0", port))
    sock.listen(5)
    print(f"[*] TCP sink listening on 0.0.0.0:{port}")
    while True:
        conn, _ = sock.accept()
        threading.Thread(target=handle_tcp, args=(conn,)).start()

def handle_tcp(conn):
    global rx_pkts, rx_bytes
    with conn:
        while True:
            data = conn.recv(4096)
            if not data:
                break
            rx_pkts += 1
            rx_bytes += len(data)

def stats():
    global rx_pkts, rx_bytes
    old_pkts, old_bytes = 0, 0
    while True:
        time.sleep(1)
        delta_pkts = rx_pkts - old_pkts
        delta_bytes = rx_bytes - old_bytes
        print(f"RX: {delta_pkts} pkts ({delta_bytes} bytes/sec)")
        old_pkts, old_bytes = rx_pkts, rx_bytes

if __name__ == "__main__":
    port = int(os.environ.get("SINK_PORT", "8100"))
    proto = os.environ.get("SINK_PROTO", "udp").lower()

    threading.Thread(target=stats, daemon=True).start()

    if proto == "udp":
        udp_sink(port)
    else:
        tcp_sink(port)
