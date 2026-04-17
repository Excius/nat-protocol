# NAT Protocol Simulator (C++)

This project is a user-space Network Address Translation (NAT) simulator focused on Port Address Translation (PAT).
It translates outbound packets from private hosts to a single public IP by assigning dynamic public ports, then maps
inbound responses back to the correct private endpoint.

## What The Project Does

- Simulates outbound NAT translation (`private_ip:private_port -> public_ip:public_port`).
- Simulates inbound reverse translation (`public_port -> private_ip:private_port`).
- Maintains an in-memory NAT table with automatic timeout expiration.
- Reuses freed ports from expired entries.
- Validates private source IPv4 addresses for outbound traffic (RFC 1918 ranges).
- Logs packet and table events with readable tags.
- Supports interactive and predefined simulation modes.

## Current Architecture

- `Packet`
  - Represents a simulated TCP/UDP packet (`src ip/port`, `dst ip/port`, protocol).
- `NatEntry`
  - Represents one NAT mapping and tracks last usage time for timeout cleanup.
- `NatTable`
  - Stores mappings in two directions:
    - private endpoint -> NAT entry
    - public port -> private endpoint
  - Allocates dynamic public ports from `40001-65535`.
  - Expires stale entries after timeout (default 60 seconds).
  - Can run optional background cleanup thread.
- `NatRouter`
  - Applies translation rules for outbound/inbound packets.
  - Drops invalid outbound private IPs and inbound packets without a mapping.
- `Logger`
  - Prints event tags like `[MAP]`, `[OUT]`, `[IN ]`, `[DROP]`, `[EXPIRE]`, `[ERROR]`.
- `IpUtils`
  - Validates IPv4 strings and checks RFC 1918 private ranges.

## Project Structure

```text
include/
  IpUtils.hpp
  Logger.hpp
  NatEntry.hpp
  NatRouter.hpp
  NatTable.hpp
  Packet.hpp
src/
  IpUtils.cpp
  Logger.cpp
  NatEntry.cpp
  NatRouter.cpp
  NatTable.cpp
  Packet.cpp
  main.cpp
tests/
  basic_tests.cpp
Makefile
```

## Build

Requirements:

- `g++` with C++17 support
- `make`

Build the simulator:

```bash
make
```

This produces the executable:

- `./app`

## Run Modes

The application supports two simulation modes and one cleanup option.

### 1) Predefined scenario mode (default)

Runs a built-in demonstration flow from `main.cpp`:

- sends several outbound packets,
- prints NAT table,
- sends inbound packets (one valid mapped port, one dropped unmapped port).

Run:

```bash
./app
```

### 2) Interactive mode

Starts a menu where you can manually:

- send outbound packets,
- send inbound packets,
- print the NAT table,
- simulate an attack burst (many outbound packets),
- exit.

Run:

```bash
./app --interactive
# or
./app -i
```

### 3) Background cleanup option

Enables a background thread that periodically removes expired mappings.

Use it with either mode:

```bash
./app --background
./app --interactive --background

# short flags
./app -b
./app -i -b
```

Notes:

- Mapping timeout is currently 60 seconds (configured in `main.cpp` via `NatTable`).
- Background cleanup interval defaults to 5 seconds in `NatTable`.

## Example Log Tags

- `[MAP]` new mapping created
- `[OUT]` outbound packet after translation
- `[IN ]` inbound packet after reverse translation
- `[DROP]` packet dropped (invalid private IP, no mapping, table full)
- `[EXPIRE]` mapping expired and removed

## Tests

Build test binary:

```bash
make test
```

Run tests:

```bash
./test
```

Current test coverage includes private IPv4 validation logic in `IpUtils`.

## Clean Build Artifacts

```bash
make clean
```

## Limitations (Current Scope)

- Simulation only (no raw sockets or real packet forwarding).
- Single public IP.
- Basic mapping model (not full-featured NAT/firewall behavior).
- Minimal test suite (currently focused on IP utility validation).
