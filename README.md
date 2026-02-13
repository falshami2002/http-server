# HTTP Server in C

A minimal HTTP/1.1 file server written from scratch in C using POSIX sockets.

This project implements a basic streaming web server without any external frameworks or libraries. It handles raw TCP connections, parses HTTP requests, and serves static files from a `www/` directory.

---

## Features

- Raw POSIX socket programming (`socket`, `bind`, `listen`, `accept`)
- HTTP request parsing
- Streaming file transfer (chunked send loop)
- Proper `Content-Length` handling
- Partial send handling (`send_all`)
- Basic directory traversal protection

## Build

```bash
make```

## Run

```bash
./server

server listens on:
http://localhost:8080```

