# NAT

#### This project is a user-space simulation of a NAT (Port Address Translation)system that translates private IP packets to a single public IP using dynamic port mapping.

---

# NAT SIMULATOR IMPROVEMENT TODO

PHASE 1 – CORE HARDENING
------------------------

- [x] Add maximum NAT table size limit
    - Add maxEntries member to NatTable
    - Reject new mappings when table is full
    - Log: [DROP: TABLE FULL]

- [x] Implement proper port exhaustion handling
    - Handle nextAvailablePort > 65535
    - Wrap around to starting port
    - Ensure port is not already in use
    - Log: [ERROR: PORT EXHAUSTED]

- [ ] Prevent port collision
    - Before assigning publicPort
    - Check inboundTraffic to ensure port is free

- [ ] Make timeout configurable
    - Add timeoutDuration as member
    - Pass via constructor
    - Remove hardcoded 60 seconds

- [ ] Optimize removeExpired()
    - Store now = steady_clock::now() once per call

- [ ] Improve printTable()
    - Print (empty) if no entries
    - Ensure clean formatting

PHASE 2 – SECURITY IMPROVEMENTS
--------------------------------

- [ ] Add NAT table overflow attack simulation

    - Generate thousands of outbound packets
    - Observe table growth
    - Show drop behavior

- [ ] Add statistics tracking

    - totalMappingsCreated
    - totalMappingsExpired
    - totalInboundPackets
    - totalOutboundPackets
    - totalDroppedInbound
    - totalDroppedTableFull

- [ ] Print statistics at program end

- [ ] Implement port reuse pool

    - Maintain queue of freed ports
    - Reuse expired ports before allocating new ones

PHASE 3 – ARCHITECTURE ENHANCEMENTS
------------------------------------

- [ ] Switch from std::map to std::unordered_map

    - Implement custom hash for PrivateKey
    - Benchmark lookup performance

- [ ] Make logging optional

    - Add verbose flag to NatTable/NatRouter
    - Disable logs when verbose = false

- [ ] Separate expiration into periodic cleanup

    - Create background thread
    - Call removeExpired() every few seconds
    - Protect table with mutex

- [ ] Add proper const correctness across project

PHASE 4 – ADVANCED NAT FEATURES
--------------------------------

- [ ] Implement NAT mode selection

    - Full Cone NAT (current behavior)
    - Symmetric NAT mode
    - Change mapping logic accordingly

- [ ] Validate private IP range

    - Allow only 192.168.x.x or 10.x.x.x
    - Drop spoofed internal IP packets

- [ ] Add firewall rule simulation

    - Maintain blocked destination ports list
    - Drop inbound packets to blocked ports

- [ ] Add rate limiting

    - Limit number of new mappings per second
    - Drop excess attempts

PHASE 5 – DEMO & PRESENTATION
-----------------------------

- [ ] Create interactive simulation menu

    - Send outbound packet
    - Send inbound packet
    - Print NAT table
    - Simulate attack
    - Exit

- [ ] Add detailed logging format

    - [MAP]
    - [OUT]
    - [IN ]
    - [DROP]
    - [EXPIRE]
    - [ERROR]

- [ ] Create attack demonstration in main()

- [ ] Write technical documentation

    - Architecture explanation
    - Security analysis
    - Limitations
    - Future improvements

BONUS – EXTRA CREDIT
--------------------

- [ ] Add multithreaded stress test
- [ ] Add configuration file support
- [ ] Add performance timing metrics
- [ ] Simulate large-scale network scenario
