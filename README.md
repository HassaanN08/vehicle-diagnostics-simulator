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
  - Explicit outgoing-frame acknowledgement before send-dependent timers or transaction completion advance

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
Linux SocketCAN
      │
      ▼
   CanSocket
      │
      ▼
 poll() event loop
      │
      ▼
DiagnosticRuntime
      │
      ▼
DiagnosticCoordinator
      │
      ▼
    IsoTp
receive / reassemble
      │
      ▼
DiagnosticCoordinator
      │
      ▼
  UDSServer
      │
      ▼
 ECU state
      │
      ▼
UDS response payload
      │
      ▼
DiagnosticCoordinator
      │
      ▼
    IsoTp
segment / transmit
      │
      ▼
DiagnosticRuntime
      │
      ▼
  CanSocket
      │
      ▼
Linux SocketCAN
```

### Responsibility Boundaries

- **`CanSocket`**

  Owns the Linux CAN socket and translates between Linux `can_frame` objects and the simulator's `CANFrame` representation. It does not interpret ISO-TP or UDS data.

- **Event loop**

  Uses `poll()` to wait for incoming CAN traffic while continuing to service ISO-TP timing and outgoing frames. It also preserves a frame when a nonblocking socket write returns `EAGAIN` or `EWOULDBLOCK`.

- **`DiagnosticRuntime`**

  Owns the persistent set of diagnostic coordinators and routes incoming CAN frames to the coordinator associated with the matching request CAN ID.

- **`DiagnosticCoordinator`**

  Connects one ECU's ISO-TP transport state to UDS request processing. It prevents a new UDS response from replacing one that is still outstanding.

- **`IsoTp`**

  Coordinates sender and receiver transport state. It handles segmentation, reassembly, Flow Control, sequencing, timing, and transport timeouts without knowing the meaning of the diagnostic payload.

- **`Sender` / `Receiver`**

  Implement the two directions of the ISO-TP state machine. The transport tracks outgoing-frame acknowledgement explicitly. Send-dependent timers and terminal sender reset occur only after the relevant CAN frame has been confirmed as accepted by the SocketCAN layer.

- **`UDSServer`**

  Parses the diagnostic service ID, dispatches the request to the appropriate service processor, and builds the logical UDS response. It does not create CAN or ISO-TP frames.

- **`Vehicle` / `ECU`**

  Own the simulated vehicle and diagnostic domain state, including ECU diagnostic sessions, DTCs, supported status masks, and diagnostic CAN identifiers.

## Diagnostic and Transport Capabilities

### UDS-Inspired Diagnostics

The diagnostic layer operates on complete payloads delivered by ISO-TP. It currently implements a focused subset of UDS-style behavior rather than the complete ISO 14229 service set.

| Service ID | Service | Implemented Behavior |
|---|---|---|
| `0x10` | Diagnostic Session Control | Supports Default Session (`0x01`) and Extended Session (`0x03`) |
| `0x14` | Clear Diagnostic Information | Supports clearing all stored DTCs using group `0xFFFFFF`; clearing requires Extended Session |
| `0x19` | Read DTC Information | Supports subfunction `0x02` with status-mask filtering |
| `0x22` | Read Data By Identifier | Supports DID `0xF186`, which reports the ECU's current diagnostic session |

#### Diagnostic Session Control (`0x10`)

Requests must contain exactly two bytes:

```text
10 [session]
```

Supported session values:

- `0x01` → Default Session
- `0x03` → Extended Session

A successful request returns the positive response SID `0x50` followed by the selected session.

#### Read Data By Identifier (`0x22`)

Requests must contain exactly three bytes:

```text
22 [DID high byte] [DID low byte]
```

The currently supported DID is:

```text
F186
```

It reports the ECU's current diagnostic session:

- `0x01` for Default Session
- `0x03` for Extended Session

For example:

```text
Request:
22 F1 86

Positive response in Extended Session:
62 F1 86 03
```

Unsupported DIDs produce a Request Out Of Range negative response.

#### Clear Diagnostic Information (`0x14`)

Requests must contain exactly four bytes.

The currently supported DTC group is:

```text
FFFFFF
```

which clears all DTCs stored by the selected ECU.

Clearing is allowed only while that ECU is in Extended Session. A successful clear produces the positive response SID `0x54`.

#### Read DTC Information (`0x19`)

The currently supported subfunction is:

```text
0x02
```

Requests use the form:

```text
19 02 [status mask]
```

The ECU filters its stored DTCs using the requested status mask and its own supported-status mask.

A positive response contains:

```text
59 02 [supported status mask]
      [3-byte DTC] [status]
      [3-byte DTC] [status]
      ...
```

Each stored DTC uses a 24-bit diagnostic identifier and an 8-bit status value.

If no DTC matches the requested mask, the request still receives a positive response containing the subfunction and ECU supported-status mask, with no DTC records appended.

#### Positive and Negative Responses

Successful diagnostic responses use the request SID plus `0x40`.

Examples:

```text
0x10 → 0x50
0x14 → 0x54
0x19 → 0x59
0x22 → 0x62
```

Negative responses use:

```text
7F [request SID] [NRC]
```

The current implementation can generate:

| NRC | Current Use |
|---|---|
| `0x11` | Unsupported service |
| `0x12` | Unsupported subfunction |
| `0x13` | Incorrect request length |
| `0x31` | Requested DID or DTC group is outside the implemented range |
| `0x7F` | Clear Diagnostic Information requested while the ECU is still in Default Session |


### ISO-TP Transport

Each diagnostic coordinator owns a persistent `IsoTp` endpoint containing independent sender and receiver state. ISO-TP operates only on byte payloads and does not interpret UDS services or ECU data.

#### Segmentation

Outgoing payload handling is currently:

- 1–7 bytes → Single Frame
- 8–4095 bytes → First Frame followed by Consecutive Frames
- empty payload → rejected
- payload larger than 4095 bytes → rejected

A First Frame carries the first six payload bytes. Each Consecutive Frame carries up to seven additional payload bytes.

Consecutive Frame sequence numbers begin at `1` and wrap after `15`.

#### Reassembly

The receiver supports:

- Single Frame payload extraction
- First Frame length decoding
- Consecutive Frame reassembly
- Consecutive Frame sequence validation
- completion based on the length declared by the First Frame
- rejection of frames arriving on the wrong receive CAN ID
- transport-state reset after malformed transport input or timeout

A reassembled payload is exposed to the diagnostic layer only after the complete message has been received.

#### Flow Control

After receiving a valid First Frame, the receiver generates a Clear To Send Flow Control frame:

```text
30 [block size] [STmin]
```

The sender recognizes:

- `0x30` → Clear To Send
- `0x31` → Wait
- `0x32` → Abort

A nonzero Block Size limits how many Consecutive Frames may be transmitted before another Flow Control frame is required.

A Block Size of `0` allows transmission to continue without another Flow Control frame for the remainder of the payload.

The sender accepts up to five consecutive Wait Flow Control frames before aborting the current transmission.

#### STmin

The sender interprets the Flow Control STmin field as:

- `0x00`–`0x7F` → 0–127 milliseconds
- `0xF1`–`0xF9` → 100–900 microseconds

Unsupported STmin encodings abort the current sender transaction.

Consecutive Frame spacing is measured from the confirmation of the previously transmitted Consecutive Frame rather than from the moment that frame was merely constructed.

#### Transport Timeouts

Both transport directions use a one-second timeout for the states currently modeled:

- the sender times out while waiting for Flow Control
- the receiver times out while waiting for the next Consecutive Frame

Timeout expiry resets the affected transport state so that a later diagnostic exchange can begin cleanly.

#### Outgoing-Frame Acknowledgement

Frame construction and successful transmission are treated as separate events.

When ISO-TP produces an outgoing CAN frame, the endpoint records that a frame is awaiting acknowledgement and will not produce another outgoing frame until the current one has been confirmed.

The runtime propagates successful SocketCAN transmission back through:

```text
CanSocket
→ DiagnosticRuntime
→ DiagnosticCoordinator
→ IsoTp
→ Sender / Receiver
```

This confirmation is used for behavior that depends on the frame actually having been handed to the kernel, including:

- starting the Flow Control wait timeout after a First Frame
- starting STmin timing after a Consecutive Frame
- starting the next Flow Control wait after a completed block
- completing/resetting the sender after a Single Frame or final Consecutive Frame
- starting the receiver's Consecutive Frame timeout after its Flow Control frame has been sent

## Build, Test, and Run

### Requirements

The runtime currently targets Linux because the SocketCAN adapter uses Linux CAN APIs.

Required development tools:

- C++23-capable compiler
- CMake 3.15 or newer
- Ninja
- Linux SocketCAN support

For the virtual CAN demo, the following are also useful:

- `iproute2`
- `can-utils`

The executable currently binds to the `vcan0` interface.

### Build

Configure a Debug build with CMake and Ninja:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
```

Build the project:

```bash
cmake --build build
```

This produces:

```text
build/vehicle-diagnostics-simulator
build/vehicle-diagnostics-tests
```

### Run the Automated Tests

The test executable is registered with CTest:

```bash
ctest --test-dir build --output-on-failure
```

A successful run should report:

```text
100% tests passed
```

### Create a Virtual CAN Interface

Load the Linux `vcan` module:

```bash
sudo modprobe vcan
```

Create `vcan0`:

```bash
sudo ip link add dev vcan0 type vcan
```

Bring the interface up:

```bash
sudo ip link set up vcan0
```

You can verify it with:

```bash
ip link show vcan0
```

If `vcan0` already exists, it does not need to be created again.

### Run the Simulator

With `vcan0` active:

```bash
./build/vehicle-diagnostics-simulator
```

The process then waits for diagnostic CAN traffic on the virtual CAN interface.

The default simulated vehicle contains:

| ECU | Request CAN ID | Response CAN ID |
|---|---:|---:|
| Engine | `0x7E0` | `0x7E8` |
| Brake | `0x7E1` | `0x7E9` |
| Battery | `0x7E2` | `0x7EA` |

### Observe CAN Traffic

In another terminal, start `candump`:

```bash
candump vcan0
```

This lets you observe both requests sent to the simulator and responses transmitted by it.

### Example: Change Diagnostic Session

The Engine ECU begins in Default Session.

Send an ISO-TP Single Frame containing a Diagnostic Session Control request for Extended Session:

```bash
cansend vcan0 7E0#021003
```

The bytes are:

```text
02       ISO-TP Single Frame payload length
10       Diagnostic Session Control
03       Extended Session
```

The simulator responds on the Engine ECU's response CAN ID:

```text
7E8#025003
```

which contains:

```text
02       ISO-TP Single Frame payload length
50       positive response SID for service 0x10
03       Extended Session
```

### Example: Read the Current Diagnostic Session

After switching the Engine ECU to Extended Session, request DID `0xF186`:

```bash
cansend vcan0 7E0#0322F186
```

The request contains:

```text
03       ISO-TP Single Frame payload length
22       Read Data By Identifier
F1 86    DID 0xF186
```

The expected response is:

```text
7E8#0462F18603
```

which contains:

```text
04       ISO-TP Single Frame payload length
62       positive response SID for service 0x22
F1 86    DID 0xF186
03       Extended Session
```

If the ECU is still in Default Session, the final byte is `01` instead.

### Remove the Virtual Interface

When finished, `vcan0` can be removed with:

```bash
sudo ip link delete vcan0
```