# HTTP/HTTPS Proxy Server (with Filtering & Logging)

## Overview
This project implements a concurrent HTTP/HTTPS proxy server using TCP sockets on Linux.
The proxy supports:

- HTTP request forwarding
- HTTPS tunneling using the CONNECT method
- Domain/IP-based filtering
- Access and block logging
- Concurrent handling of multiple clients

The project is implemented in C using POSIX sockets and pthreads and is designed to be modular,
extensible, and easy to test.


## Features
- TCP-based client–server communication
- Thread-per-connection concurrency model
- Correct parsing of HTTP requests
- Forwarding of HTTP requests and responses
- HTTPS CONNECT tunneling (transparent TCP tunnel)
- Domain/IP filtering using a configuration file
- Logging of allowed and blocked requests
- Clean modular code structure

## Project Structure
```
proxy-project/
├── src/ # Source files
├── include/ # Header files
├── config/ # Configuration files
│ └── blocked_domains.txt
├── logs/ # Log files
│ ├── access.log
│ └── block.log
├── tests/ # Test scripts and test documentation
├── docs/ # Design document and README
├── Makefile
└── proxy # Compiled binary

```

## Build Instructions

### Requirements
- Linux OS
- GCC compiler
- POSIX threads (pthread)

### Build
From the project root directory:

```bash
make

This will generate the executable: 
./proxy 

./proxy 9999
The proxy listens on 0.0.0.0:9999

Leave this terminal running while testing from another terminal or browser

```

### Testing
test_file.sh is provided in the tests folder, run that on your terminal.
Drive link of video proof:
https://drive.google.com/file/d/1NKcNrwwKUsa7_n75saQSEZzsFc-F_lHB/view?usp=sharing

### Architecture Diagram :
```
+--------+        +----------------+        +----------------+
| Client | -----> |   Proxy Server | -----> | Remote Server  |
| (curl) | <----- | (This Project) | <----- | (Web / HTTPS)  |
+--------+        +----------------+        +----------------+
                      |
                      |-- Filter (blocklist)
                      |-- Logger


```
### Component Descriptions:

a) Listener / Server Core (main.cpp)

Creates a TCP listening socket on a configurable port.
Accepts incoming client connections.
Spawns a new thread per connection.
Manages client metadata (IP, port).

b) HTTP Parser (http_parser.cpp)
Parses the HTTP request line and headers.
Extracts:
Method (GET, POST, CONNECT)
Host
Port
Path
Supports both:
Absolute URIs (proxy-style requests)
Relative URIs with Host headers

c) Filtering Module (filter.cpp)
Loads blocked domains/IPs from a configuration file.
Normalizes hostnames (lowercase, trimming).
Performs exact-match warning checks.
Decides whether a request should be allowed or blocked.

d) Forwarder (forwarder.cpp)
Handles HTTP (non-CONNECT) requests.
Establishes a TCP connection to the destination server.
Forwards the raw client request.
Streams the server response back to the client.

e) HTTPS Tunnel (connect_tunnel.cpp)
Handles CONNECT requests for HTTPS.
Establishes a TCP connection to the target host:port.
Responds with 200 Connection Established.
Relays encrypted data bidirectionally using select().

f) Logger (logger.cpp)
Writes access and block events to log files.
Logs include:
Timestamp
Client IP and port
Requested host and path
Action (ALLOWED / BLOCKED)
Status code



