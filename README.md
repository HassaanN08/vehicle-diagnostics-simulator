# Vehicle Diagnostics Simulator

A C++23/Linux automotive diagnostics project built around SocketCAN, ISO-TP transport, UDS-inspired diagnostic services, and a nonblocking event loop.

The simulator models multiple ECUs and routes diagnostic CAN traffic through persistent transport and application-layer state. It includes ISO-TP segmentation and reassembly, Flow Control handling, block size and STmin behavior, transport timeouts, and acknowledgement-aware transmission over Linux SocketCAN.

The runtime uses a nonblocking CAN socket with `poll()` to bridge Linux CAN traffic to the diagnostic stack without blocking the application loop. The project is built with CMake and Ninja, tested through CTest, and continuously verified with warning-enabled builds, AddressSanitizer, UndefinedBehaviorSanitizer, and GitHub Actions.

The implementation is intentionally focused in scope and is not presented as complete ISO 15765 or ISO 14229 compliance.


## Engineering Highlights

- **Linux SocketCAN integration**
  - Raw CAN sockets using `AF_CAN`, `SOCK_RAW`, and `CAN_RAW`
  - Interface binding through `sockaddr_can`
  - Nonblocking file descriptors
  - Explicit handling of `EINTR`, `EAGAIN`, and `EWOULDBLOCK`
  - RAII-based file descriptor ownership

- **Event-driven runtime**
  - `poll()`-based event loop
  - Incoming CAN traffic handled without blocking the runtime
  - ISO-TP timeout checks performed independently of incoming traffic
  - Pending outgoing frames retained when a nonblocking write would block

- **Stateful ISO-TP transport**
  - Single Frame, First Frame, Consecutive Frame, and Flow Control handling
  - Multi-frame payload segmentation and reassembly
  - Block Size support
  - STmin timing
  - Consecutive Frame sequence tracking
  - Sender and receiver timeout handling
  - Explicit acknowledgement of outgoing CAN frames before transport timing/state advances

- **UDS-inspired diagnostics**
  - Diagnostic Session Control (`0x10`)
  - Clear Diagnostic Information (`0x14`)
  - Read DTC Information (`0x19`)
  - Read Data By Identifier (`0x22`)
  - Positive and negative response generation
  - ECU-specific diagnostic state and DTC storage

- **Explicit ownership and state boundaries**
  - `Vehicle` owns ECU domain state
  - `DiagnosticRuntime` owns persistent diagnostic coordinators
  - Each coordinator owns a persistent ISO-TP endpoint
  - ISO-TP owns transport state
  - UDS processing remains separate from transport framing
  - Non-owning ECU references are used only within defined lifetime guarantees

- **Build and verification**
  - CMake with reusable `vds-core` library target
  - Ninja builds
  - CTest integration
  - `-Wall -Wextra -Wpedantic`
  - AddressSanitizer and UndefinedBehaviorSanitizer
  - GitHub Actions for normal and sanitized builds


## Architecture

The runtime is divided into layers so that Linux I/O, CAN transport, diagnostic coordination, protocol handling, and ECU state remain separate.

```text
                        Linux CAN interface
                              vcan0
                                │
                                ▼
                           CanSocket
                  nonblocking SocketCAN I/O
                                │
                                ▼
                         poll() event loop
                                │
                                ▼
                      DiagnosticRuntime
                routes frames by diagnostic CAN ID
                                │
                                ▼
                   DiagnosticCoordinator
                 one diagnostic channel per ECU
                                │
                  ┌─────────────┴─────────────┐
                  │                           │
                  ▼                           ▼
                IsoTp                     UDSServer
          transport state machine       diagnostic logic
           ┌────────┴────────┐                 │
           │                 │                 ▼
           ▼                 ▼                ECU
        Sender            Receiver       diagnostic state,
                                             DTCs,
                                          CAN metadata
```

## Responsibility Boundaries

**CanSocket**
Owns the Linux CAN socket and translates between Linux can_frame objects and the simulator's CANFrame representation. It does not interpret ISO-TP or UDS data.

**Event loop**
Uses poll() to wait for incoming CAN traffic while continuing to service ISO-TP timing and outgoing frames. It also preserves a frame when a nonblocking socket write returns EAGAIN or EWOULDBLOCK.

**DiagnosticRuntime**
Owns the persistent set of diagnostic coordinators and routes incoming CAN frames to the coordinator associated with the matching request CAN ID.

**DiagnosticCoordinator**
Connects one ECU's ISO-TP transport state to UDS request processing. It prevents a new UDS response from replacing one that is still outstanding.

**IsoTp**
Coordinates sender and receiver transport state. It handles segmentation, reassembly, Flow Control, sequencing, timing, and transport timeouts without knowing the meaning of the diagnostic payload.

**Sender / Receiver**
Implement the two directions of the ISO-TP state machine. Transport timing only advances after the relevant outgoing CAN frame has been confirmed as accepted by the SocketCAN layer.

**UDSServer**
Parses the diagnostic service ID, dispatches the request to the appropriate service processor, and builds the logical UDS response. It does not create CAN or ISO-TP frames.

**Vehicle / ECU**
Own the simulated vehicle and diagnostic domain state, including ECU diagnostic sessions, DTCs, supported status masks, and diagnostic CAN identifiers.