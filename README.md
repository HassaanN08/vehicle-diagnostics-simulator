# Vehicle Diagnostics Simulator

A C++ console-based vehicle diagnostics simulator that models ECUs, diagnostic trouble codes, ECU communication status, active faults, cleared fault history, diagnostic sessions, vehicle health checks, diagnostic event logging, result-based operation handling, and basic diagnostic workflows.

This project is part of a long-term systems engineering portfolio focused on Modern C++, embedded-style thinking, diagnostics, vehicle platforms, and transferable software architecture skills.

## Current Version

**Version:** v0.6
**Status:** Working release

This version includes a multi-file C++ project structure, CMake build support, diagnostic trouble code modeling, ECU fault storage, vehicle-level scanning, an interactive console menu, ECU registration through the menu, diagnostic event logging, ECU communication status, result enums for clearer operation outcomes, basic assert-based unit tests, cleared fault history per ECU, and diagnostic session handling.

## Features

* Create a simulated vehicle with multiple ECUs
* Store diagnostic trouble codes inside ECUs
* Add faults to a specific ECU through the menu
* Display all ECUs
* Display ECU communication status
* Scan the full vehicle
* Display active DTCs for each online ECU
* Show offline ECUs as not responding during scans
* Clear active faults from a specific ECU
* Move cleared faults into ECU fault history
* Display cleared fault history for a specific ECU
* Prevent clearing faults from offline ECUs
* Display overall vehicle health
* Treat offline ECUs as vehicle health issues
* Add new ECUs through the menu
* Reject duplicate ECU names
* Set ECU communication status to Online or Offline
* Display the current diagnostic session
* Switch between Default Session and Extended Session
* Require Extended Session before clearing faults
* Display a diagnostic event log
* Log successful ECU additions
* Log successful fault additions
* Log successful fault clearing
* Log vehicle scan events
* Log successful ECU status changes
* Log diagnostic session changes
* Handle invalid ECU names
* Convert user severity input into a strongly typed `Severity` enum
* Use `enum class` result types instead of magic return codes
* Use value ownership for ECUs and DTCs
* Keep ECU search logic encapsulated inside the `Vehicle` class
* Build the project using CMake
* Build and run a separate assert-based test executable

## Menu Options

The current console menu supports:

```text
Vehicle Diagnostics Simulator

1. Display all ECUs
2. Scan vehicle
3. Display vehicle health
4. Add fault to ECU
5. Clear faults from ECU
6. Add ECU
7. Display diagnostic log
8. Set ECU communication status
9. Display ECU fault history
10. Display Diagnostic Session
11. Set Diagnostic Session
12. Exit
```

## Project Structure

```text
vehicle-diagnostics-simulator/
├── include/
│   ├── DTC.h
│   ├── ECU.h
│   ├── ECUStatus.h
│   ├── Severity.h
│   ├── Vehicle.h
│   ├── VehicleResults.h
│   └── VehicleSession.h
├── src/
│   ├── DTC.cpp
│   ├── ECU.cpp
│   ├── main.cpp
│   └── Vehicle.cpp
├── tests/
│   └── test_vehicle.cpp
├── notes/
│   ├── project-scope.md
│   └── v0.4-plan.md
├── CMakeLists.txt
├── .gitignore
└── README.md
```

## Core Components

### DTC

Represents one diagnostic trouble code.

Responsibilities:

* Store fault code
* Store fault description
* Store severity
* Display fault details
* Convert severity enum values into readable text

Example:

```text
P0301 - Cylinder 1 Misfire Detected - High
```

### Severity

A scoped enum used to represent fault severity.

Current values:

```cpp
Severity::Low
Severity::Medium
Severity::High
```

Using an enum keeps severity as a controlled internal type instead of relying on random strings.

### ECUStatus

A scoped enum used to represent whether an ECU is currently communicating.

Current values:

```cpp
ECUStatus::Online
ECUStatus::Offline
```

This allows the simulator to model ECUs that are present in the vehicle but not currently responding during a diagnostic scan.

### VehicleSession

A scoped enum used to represent the current diagnostic session for the vehicle.

Current values:

```cpp
VehicleSession::DefaultSession
VehicleSession::ExtendedSession
```

The vehicle starts in Default Session. Certain actions, such as clearing faults, require Extended Session.

### VehicleResults

Contains scoped enum result types used by vehicle-level operations.

Current clear-fault results include:

```cpp
ClearFaultResult::SessionNotAllowed
ClearFaultResult::ECUNotFound
ClearFaultResult::NoFaultsToClear
ClearFaultResult::FaultsCleared
ClearFaultResult::ECUOffline
```

Current ECU status results include:

```cpp
ECUStatusResult::ECUNotFound
ECUStatusResult::AlreadyInRequestedState
ECUStatusResult::StatusChanged
```

Current diagnostic session results include:

```cpp
DiagnosticSessionResult::AlreadyInRequestedState
DiagnosticSessionResult::SessionChanged
```

These result enums replace unclear integer return codes and make the code easier to read, maintain, and extend.

### ECU

Represents one Electronic Control Unit.

Responsibilities:

* Store ECU name
* Store active DTCs
* Store cleared fault history
* Store communication status
* Default to Online when created
* Add diagnostic trouble codes
* Display active faults
* Display cleared fault history
* Move active faults into fault history when faults are cleared
* Report whether active faults exist
* Report whether cleared fault history exists
* Set communication status

Examples of ECUs:

* Engine
* Brake
* Battery
* Transmission
* Airbag Control Module

### Vehicle

Represents the full simulated vehicle.

Responsibilities:

* Store multiple ECUs
* Store the current diagnostic session
* Add ECUs
* Prevent duplicate ECU names
* Add faults to a specific ECU
* Set ECU communication status
* Set diagnostic session
* Scan all ECUs
* Display all ECUs and their status
* Check overall vehicle health
* Treat offline ECUs as health issues
* Clear faults from a specific ECU
* Prevent fault clearing when an ECU is offline
* Prevent fault clearing when the vehicle is in Default Session
* Display fault history for a specific ECU
* Store diagnostic event logs
* Display diagnostic event logs
* Encapsulate ECU search logic
* Return meaningful operation results using enum classes

## Current Diagnostic Model

The simulator uses this hierarchy:

```text
Vehicle
  └── ECU
        ├── Active DTCs
        └── Cleared DTC History
```

A vehicle owns multiple ECUs.
Each ECU owns its active diagnostic trouble codes.
Each ECU also owns cleared fault history.
Each ECU owns its own communication status.
The vehicle owns the current diagnostic session.

The menu interacts with the vehicle, and the vehicle coordinates diagnostic actions.

## Diagnostic Sessions

The simulator now supports basic diagnostic sessions.

Current sessions:

```text
Default Session
Extended Session
```

The vehicle starts in Default Session.

Default Session represents a basic diagnostic mode where the user can view information, scan the vehicle, add simulator faults, and inspect state.

Extended Session represents a deeper diagnostic mode where stronger diagnostic actions are allowed.

Clearing faults requires Extended Session. If the user attempts to clear faults while the vehicle is still in Default Session, the simulator rejects the operation and returns a session-related result.

This makes the simulator more realistic because diagnostic tools do not allow every operation in every mode.

## Active Faults vs Fault History

The simulator separates active faults from cleared fault history.

### Active Faults

Active faults represent issues that currently exist in an ECU.

These appear during a vehicle scan if the ECU is online.

### Cleared Fault History

Cleared fault history stores faults that previously existed but were cleared.

When faults are cleared from an online ECU while the vehicle is in Extended Session:

```text
Active faults -> Cleared fault history
Active faults become empty
```

This allows the simulator to preserve diagnostic history instead of completely forgetting cleared faults.

## Offline ECU Behavior

If an ECU is offline, the simulator treats it as not responding.

During a vehicle scan:

```text
Airbag Control Module ECU status is Offline
```

The simulator does not display that ECU’s active DTCs during the scan because the diagnostic tool cannot communicate with it.

Fault clearing is also blocked when an ECU is offline. If the user tries to clear faults from an offline ECU, the simulator returns a clear result showing that the ECU is offline and does not move active faults into history.

## Diagnostic Log

The simulator keeps a simple session-level diagnostic log.

The log records successful actions such as:

* Adding a new ECU
* Adding a fault to an ECU
* Clearing faults from an ECU
* Scanning the vehicle
* Changing ECU communication status
* Changing diagnostic session

Example:

```text
Diagnostic Log:

1. Added ECU: Transmission
2. Added fault P0301 to Engine
3. Vehicle scan performed
4. Set Airbag Control Module status to Offline
5. Diagnostic Session changed to Extended
6. Cleared faults from Engine
```

The log does not currently persist after the program closes.

## Unit Tests

The project includes a separate assert-based test executable.

Current tests cover:

* ECU defaults to Online
* ECU can be set to Offline
* ECU reports no faults when created
* ECU reports fault history as empty when created
* ECU reports faults after adding a DTC
* ECU reports no active faults after clearing faults
* Clearing faults moves active faults into cleared fault history
* Vehicle accepts new ECUs
* Vehicle rejects duplicate ECU names
* Vehicle reports healthy when ECUs are online and no faults exist
* Vehicle reports issues when an ECU is offline
* Clearing faults returns the correct result enum
* Clearing faults from an offline ECU returns `ClearFaultResult::ECUOffline`
* Vehicle starts in Default Session
* Changing to Extended Session returns `DiagnosticSessionResult::SessionChanged`
* Changing to the same session returns `DiagnosticSessionResult::AlreadyInRequestedState`
* Clearing faults in Default Session returns `ClearFaultResult::SessionNotAllowed`
* Clearing faults in Extended Session returns `ClearFaultResult::FaultsCleared`
* Setting ECU status returns the correct result enum

The tests are intentionally simple and use `assert()` instead of a full testing framework. This keeps the focus on testing fundamentals before introducing GoogleTest, Catch2, or other test frameworks.

## Example Workflow

A user can:

1. Start the simulator
2. Display the current diagnostic session
3. Display all ECUs
4. Add a new ECU such as `Transmission`
5. Add a fault to `Transmission`
6. Scan the vehicle and see active faults
7. Try to clear faults in Default Session and receive a clean failure message
8. Switch to Extended Session
9. Clear faults from `Transmission`
10. Display `Transmission` fault history
11. Add an ECU such as `Airbag Control Module`
12. Set `Airbag Control Module` to Offline
13. Scan the vehicle and see it marked as offline
14. Try to clear faults from an offline ECU and receive a clean failure message
15. Display vehicle health
16. Display the diagnostic log
17. Exit the program

## Concepts Practiced

This project strengthens both C++ fundamentals and systems-style thinking.

C++ concepts:

* Classes and objects
* Encapsulation
* Multi-file project structure
* Header and implementation separation
* `std::vector`
* `std::string`
* `enum class`
* Const member functions
* Passing by reference and const reference
* Private helper methods
* Value ownership
* Return values for operation success/failure
* Result enums instead of magic numbers
* Moving data between vectors
* Console input handling
* Menu-driven program flow
* `assert()`-based unit testing
* CMake build configuration
* Multiple CMake executables

Systems concepts:

* Component hierarchy
* Fault modeling
* Diagnostic scanning
* Health checks
* Module-level responsibility
* Simple system state inspection
* Safe object ownership
* Event logging
* Communication status modeling
* State-dependent behavior
* Session-dependent behavior
* Permission-gated operations
* Fault history tracking
* Automated behavior verification
* Build tooling

Automotive concepts:

* ECUs
* Diagnostic trouble codes
* Active faults
* Cleared fault history
* Fault severity
* Vehicle scanning
* Fault clearing
* ECU communication status
* Offline / not responding ECU behavior
* Diagnostic sessions
* Basic diagnostic workflow
* Diagnostic event history

## How to Run

This project uses CMake.

### Build

```bash
cmake -S . -B build
cmake --build build
```

### Run the Simulator

#### Linux / macOS

```bash
./build/vehicle-diagnostics-simulator
```

#### Windows

```bash
build\Debug\vehicle-diagnostics-simulator.exe
```

### Run the Tests

#### Linux / macOS

```bash
./build/vehicle-diagnostics-tests
```

#### Windows

```bash
build\Debug\vehicle-diagnostics-tests.exe
```

## Manual Build Alternative

You can also compile manually with `g++`.

### Simulator

```bash
g++ -std=c++17 src/main.cpp src/Vehicle.cpp src/ECU.cpp src/DTC.cpp -Iinclude -o vehicle-diagnostics-simulator
./vehicle-diagnostics-simulator
```

### Tests

```bash
g++ -std=c++17 tests/test_vehicle.cpp src/Vehicle.cpp src/ECU.cpp src/DTC.cpp -Iinclude -o vehicle-diagnostics-tests
./vehicle-diagnostics-tests
```

## Current Limitations

This is still an early simulator and intentionally keeps the model simple.

Current limitations:

* Default startup ECUs are still hardcoded
* No file saving or loading
* Diagnostic log is session-only
* No timestamps in diagnostic log
* No CAN bus simulation yet
* No UDS request/response behavior yet
* No persistent diagnostic history
* No real vehicle communication
* Limited input validation
* ECU communication state is limited to Online and Offline only
* Diagnostic sessions are limited to Default and Extended only
* Cleared fault history has no timestamps
* Tests use basic `assert()` instead of a full testing framework

## Planned Improvements

Future versions may include:

* Cleaner reusable input helpers
* More ECU communication states
* ECU readiness states
* Diagnostic log timestamps
* Fault history timestamps
* File persistence
* GoogleTest or Catch2 integration
* CAN bus message simulation
* UDS-inspired diagnostic requests
* Security access simulation
* ECU state machine behavior
* Improved Linux-first development workflow

## Why This Project Exists

This project is designed to move beyond generic beginner C++ projects and start modeling real systems-style software.

The goal is to build toward a Modern C++ Systems Engineering profile with relevance to automotive, embedded systems, diagnostics, vehicle platforms, cyber-physical systems, and other engineering-heavy industries.

This simulator is an early step toward larger projects involving CAN bus simulation, ECU state machines, diagnostics tooling, and automotive middleware-style architecture.

## Repository Status

This project is currently at **v0.6**.

The current release contains a working diagnostic simulator with a multi-file C++ structure, CMake build support, an interactive menu, ECU modeling, DTC storage, ECU registration, fault scanning, fault clearing, active fault tracking, cleared fault history, vehicle health checks, diagnostic event logging, ECU communication status, diagnostic sessions, result enums for clearer operation outcomes, and basic assert-based unit tests.