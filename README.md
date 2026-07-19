# Vehicle Diagnostics Simulator

A Modern C++ vehicle diagnostics simulator that models ECUs, diagnostic trouble codes, diagnostic sessions, CAN traffic, CAN signal decoding, simplified UDS request and response handling, diagnostic data retrieval, and ISO-TP response segmentation.

The project began as a console-based vehicle and fault simulator. It is now being developed into a testable automotive diagnostics and middleware portfolio project with clear separation between domain logic, diagnostic protocol behavior, transport behavior, CAN modeling, and console interaction.

## Current Version

**Version:** v0.10.0  
**Status:** Working development release

The current milestone adds an end-to-end UDS-inspired VIN retrieval flow:

```text
Diagnostic CAN request
→ UDS request parsing
→ response construction
→ DID-backed VIN retrieval
→ logical 20-byte UDS response
→ ISO-TP segmentation
→ multiple response CAN frames
```

The project is intentionally simplified and is not presented as full ISO 14229 or ISO 15765 compliance.

---

## Current Capabilities

### Vehicle and ECU Simulation

- Create a simulated vehicle containing multiple ECUs
- Add ECUs at runtime
- Reject duplicate ECU names
- Store active diagnostic trouble codes inside ECUs
- Store cleared fault history separately from active faults
- Scan the full vehicle
- Display ECU communication status
- Set ECUs to Online or Offline
- Treat offline ECUs as vehicle health issues
- Prevent communication-dependent operations on offline ECUs
- Display overall vehicle health
- Clear faults from a selected ECU
- Require Extended Session before clearing faults
- Return scoped result enums instead of magic integer codes

### Diagnostic Sessions

- Default Session
- Extended Session
- Session-dependent fault clearing
- Structured session-change results

### Diagnostic Event Logging

- Vehicle-owned `DiagnosticLog`
- Capacity limited to the most recent 50 entries
- Oldest entry discarded when capacity is exceeded
- Logs important actions such as:
  - vehicle scans
  - ECU additions
  - fault additions
  - fault clearing
  - ECU status changes
  - diagnostic session changes

### CAN Modeling

- `CANFrame` with:
  - CAN identifier
  - sender name
  - raw byte payload
- Classical CAN payload limited to 8 bytes
- Read-only payload snapshots
- `CANBus` traffic history
- CAN traffic capacity limited to 100 frames
- Oldest frame discarded when capacity is exceeded
- Vehicle-owned CAN bus
- Sample and custom CAN traffic support
- CAN traffic reporting and analysis

### CAN Signal Decoding

The simulator currently recognizes these signal-oriented CAN identifiers:

| CAN ID | Source | Decoded Signal |
|---|---|---|
| `256` | Engine | RPM |
| `512` | Brake | Pressed, Released, or Unknown |
| `768` | Battery | Voltage |

Unknown identifiers remain distinguishable from known but malformed frames.

### CAN Traffic Analysis

`CANTrafficAnalyzer` produces a structured `CANTrafficReport` containing:

- total frame count
- known frame count
- valid frame count
- malformed frame count
- unknown frame count
- individual decoded results

### UDS-Inspired Diagnostics

The simulator currently recognizes these service identifiers:

| Service ID | Service |
|---|---|
| `0x10` | Diagnostic Session Control |
| `0x14` | Clear Diagnostic Information |
| `0x19` | Read DTC Information |
| `0x22` | Read Data By Identifier |

Current behavior includes:

- structured request parsing
- recognized, valid, and error states
- positive response service IDs
- negative responses
- service-specific request validation
- one-DID-per-request simplification for service `0x22`

### Supported Negative Response Codes

| NRC | Meaning |
|---|---|
| `0x11` | Service Not Supported |
| `0x13` | Incorrect Message Length or Invalid Format |
| `0x31` | Request Out Of Range |

Examples:

```text
Unsupported service:
99
→ 7F 99 11
```

```text
Malformed Read Data By Identifier request:
22 F1
→ 7F 22 13
```

```text
Unsupported but correctly formed DID:
22 12 34
→ 7F 22 31
```

### Diagnostic Data Store

The current `DiagnosticDataStore` supports:

```text
DID F190 = Vehicle Identification Number
```

The simulated VIN is:

```text
1FTFW1EF0HE123456
```

It contains 17 characters and is returned as 17 byte values.

### End-to-End VIN Retrieval

Request:

```text
22 F1 90
```

Logical positive response:

```text
62 F1 90 [17 VIN bytes]
```

The complete response is 20 logical bytes:

```text
1 response service byte
+ 2 DID bytes
+ 17 VIN bytes
= 20 bytes
```

Because the response exceeds the Classical CAN 8-byte limit, it is passed to the ISO-TP transport layer.

### Simplified ISO-TP Response Segmentation

The transport layer currently supports outbound segmentation only.

Supported frame types:

- Single Frame
- First Frame
- Consecutive Frame

Current behavior:

- empty payload returns no frames
- payloads from 1 to 7 bytes produce one Single Frame
- payloads from 8 to 4095 bytes produce one First Frame followed by Consecutive Frames
- payloads larger than 4095 bytes are rejected
- no frame exceeds 8 bytes
- payload order is preserved
- no padding is added
- Consecutive Frame sequence numbers begin at 1
- sequence numbers wrap after 15

Example Single Frame:

```text
Logical payload:
50

CAN payload:
01 50
```

Example negative response:

```text
Logical payload:
7F 99 11

CAN payload:
03 7F 99 11
```

Example 20-byte VIN response structure:

```text
First Frame:
10 14 62 F1 90 31 46 54

Consecutive Frame 1:
21 46 57 31 45 46 30 48

Consecutive Frame 2:
22 45 31 32 33 34 35 36
```

This implementation pre-generates the complete ordered frame collection. It does not yet wait for a Flow Control frame.

---

## Architecture Overview

The project is divided into distinct responsibilities.

### Signal-Oriented CAN Flow

```text
CANFrame
→ CANDecoder
→ DecodedCANFrame
→ CANTrafficAnalyzer
→ CANTrafficReport
```

### Diagnostic Request and Response Flow

```text
CANFrame request
→ DiagnosticRequestParser
→ DiagnosticRequest
→ DiagnosticResponseBuilder
→ DiagnosticDataStore
→ DiagnosticResponse logical payload
→ IsoTpSegmenter
→ vector<CANFrame>
```

### Responsibility Boundaries

```text
CANFrame
Stores one raw CAN message.

CANBus
Stores and transmits CAN frames inside the simulator.

CANDecoder
Interprets signal-oriented CAN frames such as RPM, brake status, and voltage.

DiagnosticRequestParser
Validates and interprets diagnostic request bytes.

DiagnosticResponseBuilder
Chooses positive or negative diagnostic behavior and builds a logical response.

DiagnosticDataStore
Provides data associated with supported diagnostic identifiers.

IsoTpSegmenter
Splits logical payloads into Classical CAN-sized ISO-TP frames.

DiagnosticMessageProcessor
Coordinates the request-to-response pipeline.

Vehicle
Owns vehicle state, ECUs, diagnostic sessions, logs, and CAN traffic.

MenuActions
Handles console workflows only.
```

No single class owns parsing, response policy, diagnostic data, transport segmentation, CAN storage, and console output.

---

## Project Structure

```text
vehicle-diagnostics-simulator/
├── include/
│   ├── CANBus.h
│   ├── CANDecoder.h
│   ├── CANDecoderHelperFunctions.h
│   ├── CANFrame.h
│   ├── CANTrafficAnalyzer.h
│   ├── CANTrafficReport.h
│   ├── ConsoleInput.h
│   ├── DecodedCANFrame.h
│   ├── DiagnosticLog.h
│   ├── DTC.h
│   ├── ECU.h
│   ├── ECUStatus.h
│   ├── MenuActions.h
│   ├── Severity.h
│   ├── Vehicle.h
│   ├── VehicleResults.h
│   ├── VehicleSession.h
│   ├── diagnostics/
│   │   ├── DiagnosticDataStore.h
│   │   ├── DiagnosticMessageProcessor.h
│   │   ├── DiagnosticRequest.h
│   │   ├── DiagnosticRequestParser.h
│   │   ├── DiagnosticResponse.h
│   │   └── DiagnosticResponseBuilder.h
│   └── transport/
│       └── IsoTpSegmenter.h
├── src/
│   ├── CANBus.cpp
│   ├── CANDecoder.cpp
│   ├── CANDecoderHelperFunctions.cpp
│   ├── CANFrame.cpp
│   ├── CANTrafficAnalyzer.cpp
│   ├── CANTrafficReport.cpp
│   ├── ConsoleInput.cpp
│   ├── DecodedCANFrame.cpp
│   ├── DiagnosticLog.cpp
│   ├── DTC.cpp
│   ├── ECU.cpp
│   ├── main.cpp
│   ├── MenuActions.cpp
│   ├── Vehicle.cpp
│   ├── diagnostics/
│   │   ├── DiagnosticDataStore.cpp
│   │   ├── DiagnosticMessageProcessor.cpp
│   │   ├── DiagnosticRequestParser.cpp
│   │   └── DiagnosticResponseBuilder.cpp
│   └── transport/
│       └── IsoTpSegmenter.cpp
├── tests/
│   └── test_vehicle.cpp
├── notes/
├── CMakeLists.txt
├── .gitignore
└── README.md
```

---

## Module Map

### Core Vehicle Domain

Files related to:

- `Vehicle`
- `ECU`
- `DTC`
- `Severity`
- `ECUStatus`
- `VehicleSession`
- `VehicleResults`
- `DiagnosticLog`

These components model vehicle state, fault state, communication state, sessions, and vehicle-level operations.

### CAN Model and Signal Decoding

Files related to:

- `CANFrame`
- `CANBus`
- `CANDecoder`
- `CANDecoderHelperFunctions`
- `DecodedCANFrame`
- `CANTrafficAnalyzer`
- `CANTrafficReport`

These components model raw CAN traffic and decode selected signal-oriented frames.

### Diagnostics

Files inside:

```text
include/diagnostics/
src/diagnostics/
```

These components parse diagnostic requests, build logical responses, retrieve DID-backed data, and coordinate diagnostics processing.

### Transport

Files inside:

```text
include/transport/
src/transport/
```

The current transport component handles simplified ISO-TP response segmentation.

### Console Interface

Files related to:

- `ConsoleInput`
- `MenuActions`
- `main`

These components handle interactive input and display. Protocol and domain behavior should remain outside this layer.

### Tests

The current assert-based tests are stored in:

```text
tests/test_vehicle.cpp
```

The test file currently covers the full project. It may be split into separate vehicle, CAN, diagnostics, and transport test files as the codebase continues to grow.

---

## Core Components

### Vehicle

Represents the complete simulated vehicle.

Responsibilities include:

- owning ECUs
- owning the diagnostic session
- owning the diagnostic log
- owning the CAN bus
- adding ECUs
- preventing duplicate ECU names
- adding faults
- scanning ECUs
- checking vehicle health
- changing ECU communication status
- clearing faults
- enforcing diagnostic-session restrictions
- exposing CAN traffic information
- producing traffic-analysis reports

### ECU

Represents one Electronic Control Unit.

Responsibilities include:

- storing ECU name
- storing active DTCs
- storing cleared fault history
- storing communication status
- adding faults
- clearing faults
- moving cleared faults into history
- reporting active faults
- reporting fault history

### DTC

Represents one diagnostic trouble code.

Stores:

- fault code
- fault description
- severity

Example:

```text
P0301 - Cylinder 1 Misfire Detected - High
```

### DiagnosticLog

Stores the most recent 50 simulator events.

When full:

```text
oldest entry removed
newest entry added
```

The current log is session-only and does not persist after the program exits.

### CANFrame

Represents one raw CAN-style message.

Stores:

- CAN identifier
- sender name
- raw byte payload

The payload is capped at 8 bytes to preserve Classical CAN behavior.

The frame does not decode itself.

### CANBus

Stores recently transmitted CAN frames.

Current maximum:

```text
100 frames
```

When capacity is exceeded, the oldest frame is discarded.

### CANDecoder

Interprets selected signal-oriented CAN frames.

It does not process UDS requests.

### DiagnosticRequestParser

Converts a diagnostic request `CANFrame` into a structured `DiagnosticRequest`.

It determines:

- source CAN ID
- service ID
- service name
- parameters
- whether the service is recognized
- whether the request shape is valid
- parser error information

It does not retrieve data or execute vehicle behavior.

### DiagnosticResponseBuilder

Converts a `DiagnosticRequest` into a logical `DiagnosticResponse`.

It determines:

- positive or negative response
- response service ID
- negative response code
- response description
- complete logical payload

It does not build CAN frames.

### DiagnosticDataStore

Stores data that may be retrieved through diagnostic identifiers.

Currently supported:

```text
F190 → VIN
```

The data store does not build UDS responses or ISO-TP frames.

### IsoTpSegmenter

Accepts:

- response CAN ID
- sender
- complete logical payload

Returns:

```text
std::vector<CANFrame>
```

It does not know whether the payload contains a VIN, DTC information, session data, or another diagnostic response.

### DiagnosticMessageProcessor

Coordinates:

```text
parse request
→ build response
→ segment response
→ return CAN frames
```

Current response rules:

```text
Response CAN ID = Request CAN ID + 8
Sender = ECU
```

---

## Console Interface

The console application currently supports vehicle, ECU, diagnostic session, fault, logging, and CAN traffic workflows.

Current console capabilities include:

- displaying ECUs
- scanning the vehicle
- displaying vehicle health
- adding faults
- clearing faults
- adding ECUs
- displaying diagnostic logs
- changing ECU communication status
- displaying ECU fault history
- displaying and changing diagnostic sessions
- simulating CAN traffic
- displaying CAN traffic
- decoding and analyzing CAN traffic
- entering selected custom CAN data

The UDS and ISO-TP diagnostic pipeline is currently exercised through the core API and automated tests. It is not yet exposed as a complete interactive menu workflow.

---

## Testing

The project currently uses a separate assert-based test executable.

Tests cover:

### Vehicle and ECU Behavior

- default ECU status
- online and offline state changes
- fault storage
- active-fault reporting
- cleared fault history
- duplicate ECU rejection
- vehicle health
- diagnostic sessions
- session-dependent fault clearing
- offline ECU restrictions
- result enums

### Diagnostic Logging

- adding log entries
- 50-entry capacity
- oldest-entry removal

### CAN Modeling

- frame ID
- sender
- payload length
- empty payload behavior
- 8-byte payload cap
- CAN bus traffic storage
- traffic count
- 100-frame capacity
- oldest-frame removal
- vehicle-owned CAN traffic
- payload snapshot behavior

### CAN Decoding and Analysis

- Engine RPM decoding
- Brake status decoding
- Battery voltage decoding
- malformed known frames
- unknown frames
- structured decoded results
- traffic report counts
- vehicle-level report display

### Diagnostic Request Parsing

- empty requests
- supported services
- unsupported services
- valid request lengths
- malformed request lengths
- service parameters
- one-DID-per-request validation

### Diagnostic Response Building

- positive response service IDs
- NRC `0x11`
- NRC `0x13`
- NRC `0x31`
- positive VIN response
- 20-byte logical VIN payload
- malformed requests
- unsupported DIDs

### Diagnostic Data Store

- supported DID recognition
- VIN byte retrieval
- 17-byte VIN length
- byte order
- unsupported DID behavior

### ISO-TP

- one-byte Single Frame
- seven-byte Single Frame boundary
- eight-byte multi-frame boundary
- First Frame length information
- Consecutive Frame sequence numbers
- 20-byte payload segmentation
- frame counts
- byte ordering

### End-to-End Diagnostics

- `22 F1 90` VIN request
- positive `62 F1 90` logical response
- three ISO-TP response frames
- response CAN ID
- ECU sender
- VIN bytes distributed across frames
- unsupported DID response
- malformed DID request response

The project intentionally uses `assert()` while the focus remains on testing fundamentals. A professional testing framework may be introduced later.

---

## Build and Run

The project uses CMake and requires a C++17-compatible compiler.

### Configure and Build

```bash
cmake -S . -B build
cmake --build build
```

### Run the Simulator

Linux or macOS:

```bash
./build/vehicle-diagnostics-simulator
```

Windows with a multi-configuration generator:

```text
build\Debug\vehicle-diagnostics-simulator.exe
```

### Run the Tests

Linux or macOS:

```bash
./build/vehicle-diagnostics-tests
```

Windows with a multi-configuration generator:

```text
build\Debug\vehicle-diagnostics-tests.exe
```

Expected result:

```text
All Tests Passed!
```

---

## Version History

### v0.10.0 - UDS VIN Retrieval and ISO-TP Response Segmentation

- Added `DiagnosticRequest`
- Added `DiagnosticRequestParser`
- Added recognized, valid, parameter, and error request states
- Added support for services `0x10`, `0x14`, `0x19`, and `0x22`
- Added `DiagnosticResponse`
- Added `DiagnosticResponseBuilder`
- Added positive UDS-style response service IDs
- Added negative responses using NRC `0x11`, `0x13`, and `0x31`
- Added `DiagnosticMessageProcessor`
- Added diagnostics module folders
- Added `DiagnosticDataStore`
- Added DID `0xF190`
- Added simulated 17-character VIN retrieval
- Added logical 20-byte VIN response
- Added transport module
- Added simplified `IsoTpSegmenter`
- Added Single Frame generation
- Added First Frame generation
- Added Consecutive Frame generation
- Added sequence numbering
- Updated diagnostic processing to return multiple CAN frames
- Added end-to-end VIN request and response tests
- Documented current UDS and ISO-TP simplifications

### v0.9 - Structured CAN Decoding and Traffic Analysis

- Added structured `DecodedCANFrame`
- Added `CANDecoder`
- Added Engine RPM decoding
- Added Brake status decoding
- Added Battery voltage decoding
- Added malformed and unknown frame handling
- Added `CANTrafficAnalyzer`
- Added `CANTrafficReport`
- Added decoded traffic counts
- Added vehicle-level CAN traffic analysis
- Improved CAN payload snapshot behavior
- Expanded CAN and analysis tests

### v0.8 - Basic CAN Bus Simulation

- Added `CANFrame`
- Added `CANBus`
- Integrated CAN traffic into `Vehicle`
- Limited Classical CAN payloads to 8 bytes
- Added capacity-limited CAN traffic history
- Added sample CAN traffic
- Added menu support for CAN traffic
- Added CAN frame and bus tests

### v0.7 - Capacity-Limited Diagnostic Logging

- Extracted `DiagnosticLog`
- Limited logs to 50 entries
- Added oldest-entry removal
- Expanded diagnostic log tests

### v0.6 - Diagnostic Sessions

- Added Default Session
- Added Extended Session
- Required Extended Session before clearing faults
- Added diagnostic-session result enums
- Added session tests

### v0.5 - Fault History and Offline Protection

- Added cleared fault history
- Moved cleared faults into history
- Prevented fault clearing from offline ECUs
- Added fault-history tests

### v0.4 - Basic Automated Tests

- Added assert-based tests
- Added separate CMake test executable

### v0.3 - ECU Communication Status and Result Enums

- Added Online and Offline ECU states
- Added scoped result enums
- Treated offline ECUs as vehicle health issues

### v0.2 - Menu, ECU Registration, and Logging

- Added runtime ECU registration
- Added duplicate ECU rejection
- Added diagnostic event logging
- Improved menu workflows

### v0.1 - Initial Simulator

- Added Vehicle
- Added ECU
- Added DTC
- Added active fault storage
- Added vehicle scanning
- Added basic console interface

---

## Current Limitations

The simulator intentionally remains incomplete.

Current limitations include:

- no persistent storage
- no file loading or saving
- no log timestamps
- no structured log event objects
- no cleared-fault timestamps
- no real vehicle connection
- no SocketCAN
- no `vcan0`
- no real CAN arbitration
- no CAN FD
- no extended CAN addressing model
- no receive-side ISO-TP reassembly
- no ISO-TP Flow Control state machine
- no block-size handling
- no STmin timing
- no ISO-TP timeouts
- no ISO-TP padding
- no extended ISO-TP addressing
- no multi-DID `0x22` requests
- no real diagnostic execution against multiple ECU types
- no real session-control behavior through UDS
- no DTC retrieval through UDS
- no security access
- no service dispatcher
- no queues or threaded runtime
- no fixed-size diagnostic buffers
- no `std::array` or `std::span` migration yet
- no professional unit-test framework
- no CI pipeline
- no sanitizers or static-analysis integration
- UDS and ISO-TP behavior is simplified and not standards-complete

---

## Planned Direction

The long-term project direction is:

```text
Vehicle Diagnostics Simulator
→ Architecture Isolation
→ UDS
→ ISO-TP
→ Fixed and Bounded Buffers
→ SocketCAN and Linux
→ Middleware Runtime
→ Portfolio and Interview Readiness
```

Likely future milestones include:

- splitting the large test file into focused test modules
- stronger architecture documentation
- additional diagnostic identifiers
- service-specific diagnostic execution
- receive-side ISO-TP reassembly
- Flow Control modeling
- fixed-size and bounded buffer design
- `std::array`
- `std::span`
- ring buffers
- SocketCAN
- `vcan0`
- Linux adapters
- request and response queues
- diagnostic dispatching
- telemetry services
- middleware runtime behavior
- sanitizers
- `clang-tidy`
- continuous integration
- improved portfolio documentation
- architecture diagrams

---

## Concepts Practiced

### Modern C++

- classes and objects
- encapsulation
- header and implementation separation
- composition
- value ownership
- scoped enums
- const member functions
- const references
- return-by-value result objects
- `std::vector`
- `std::deque`
- `std::string`
- `uint8_t`
- snapshots instead of mutable internal exposure
- multi-file projects
- modular source folders
- CMake
- multiple executables
- assert-based testing

### Data and Protocol Handling

- raw byte payloads
- ASCII character-to-byte conversion
- high-byte and low-byte reconstruction
- bit shifting
- masks
- nibbles
- service identifiers
- Data Identifiers
- positive responses
- negative response codes
- logical message length
- frame segmentation
- sequence numbers
- payload-order preservation

### Architecture

- parser versus executor
- syntax validation versus semantic validation
- logical response versus transport framing
- domain ownership
- coordinator classes
- stateless service objects
- UI boundaries
- application protocol versus transport protocol
- module-level responsibility
- capacity-limited storage

### Automotive and Systems Concepts

- ECUs
- DTCs
- diagnostic sessions
- vehicle health
- offline ECU behavior
- CAN frames
- CAN traffic
- CAN signal decoding
- UDS-inspired services
- Data Identifiers
- VIN retrieval
- ISO-TP segmentation
- Classical CAN payload limitations
- bounded traffic history
- middleware-style processing pipelines

---

## Why This Project Exists

This project is intended to move beyond generic beginner C++ applications.

The goal is to build practical experience relevant to:

- Modern C++ systems engineering
- automotive middleware
- vehicle diagnostics
- embedded-style design
- Linux vehicle platforms
- protocol processing
- bounded-memory systems
- testable software architecture

The project is being developed as a learning system and portfolio project. Each stage introduces a real architectural or protocol problem while keeping the implementation small enough to understand, test, and improve.

---

## Repository Status

The repository is currently at **v0.10.0**.

The current release includes:

- vehicle and ECU modeling
- active and cleared faults
- diagnostic sessions
- capacity-limited logging
- CAN frame and bus simulation
- structured CAN signal decoding
- CAN traffic analysis
- UDS-inspired request parsing
- positive and negative diagnostic responses
- DID-backed VIN retrieval
- simplified ISO-TP response segmentation
- end-to-end multi-frame VIN response generation
- CMake build support
- assert-based automated tests

The next development stages will continue moving the simulator toward Linux-based automotive middleware and diagnostics architecture.