# Vehicle Diagnostics Simulator

A C++ console-based vehicle diagnostics simulator that models ECUs, diagnostic trouble codes, ECU communication status, vehicle health checks, diagnostic event logging, result-based operation handling, and basic diagnostic workflows.

This project is part of a long-term systems engineering portfolio focused on Modern C++, embedded-style thinking, diagnostics, vehicle platforms, and transferable software architecture skills.

## Current Version

**Version:** v0.4
**Status:** Working release

This version includes a multi-file C++ project structure, CMake build support, diagnostic trouble code modeling, ECU fault storage, vehicle-level scanning, an interactive console menu, ECU registration through the menu, diagnostic event logging, ECU communication status, result enums for clearer operation outcomes, and basic assert-based unit tests.

## Features

* Create a simulated vehicle with multiple ECUs
* Store diagnostic trouble codes inside ECUs
* Add faults to a specific ECU through the menu
* Display all ECUs
* Display ECU communication status
* Scan the full vehicle
* Display active DTCs for each online ECU
* Show offline ECUs as not responding during scans
* Clear faults from a specific ECU
* Display overall vehicle health
* Treat offline ECUs as vehicle health issues
* Add new ECUs through the menu
* Reject duplicate ECU names
* Set ECU communication status to Online or Offline
* Display a diagnostic event log
* Log successful ECU additions
* Log successful fault additions
* Log successful fault clearing
* Log vehicle scan events
* Log successful ECU status changes
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
9. Exit
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
│   └── VehicleResults.h
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

### VehicleResults

Contains scoped enum result types used by vehicle-level operations.

Current result enums include:

```cpp
ClearFaultResult::ECUNotFound
ClearFaultResult::NoFaultsToClear
ClearFaultResult::FaultsCleared
```

and:

```cpp
ECUStatusResult::ECUNotFound
ECUStatusResult::AlreadyInRequestedState
ECUStatusResult::StatusChanged
```

These result enums replace unclear integer return codes and make the code easier to read, maintain, and extend.

### ECU

Represents one Electronic Control Unit.

Responsibilities:

* Store ECU name
* Store active DTCs
* Store communication status
* Default to Online when created
* Add diagnostic trouble codes
* Display active faults
* Clear all faults
* Report whether active faults exist
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
* Add ECUs
* Prevent duplicate ECU names
* Add faults to a specific ECU
* Set ECU communication status
* Scan all ECUs
* Display all ECUs and their status
* Check overall vehicle health
* Treat offline ECUs as health issues
* Clear faults from a specific ECU
* Store diagnostic event logs
* Display diagnostic event logs
* Encapsulate ECU search logic
* Return meaningful operation results using enum classes

## Current Diagnostic Model

The simulator uses this hierarchy:

```text
Vehicle
  └── ECU
        └── DTC
```

A vehicle owns multiple ECUs.
Each ECU owns multiple diagnostic trouble codes.
Each ECU also owns its own communication status.

The menu interacts with the vehicle, and the vehicle coordinates diagnostic actions.

## ECU Communication Status

Each ECU can currently be either:

```text
Online
Offline
```

Online ECUs can report their active DTCs during a scan.

Offline ECUs are shown as not responding, and their DTCs are not displayed during the scan because the diagnostic tool cannot communicate with them.

Example:

```text
Airbag Control Module

Airbag Control Module ECU status is Offline
```

Offline ECUs also affect vehicle health. If any ECU is offline, the vehicle health check reports that issues are detected.

## Diagnostic Log

The simulator keeps a simple session-level diagnostic log.

The log records successful actions such as:

* Adding a new ECU
* Adding a fault to an ECU
* Clearing faults from an ECU
* Scanning the vehicle
* Changing ECU communication status

Example:

```text
Diagnostic Log:

1. Added ECU: Transmission
2. Added fault P0301 to Engine
3. Vehicle scan performed
4. Set Airbag Control Module status to Offline
5. Cleared faults from Engine
```

The log does not currently persist after the program closes.

## Unit Tests

The project includes a separate assert-based test executable.

Current tests cover:

* ECU defaults to Online
* ECU can be set to Offline
* ECU reports no faults when created
* ECU reports faults after adding a DTC
* ECU reports no faults after clearing faults
* Vehicle accepts new ECUs
* Vehicle rejects duplicate ECU names
* Vehicle reports healthy when ECUs are online and no faults exist
* Vehicle reports issues when an ECU is offline
* Clearing faults returns the correct result enum
* Setting ECU status returns the correct result enum

The tests are intentionally simple and use `assert()` instead of a full testing framework. This keeps the focus on learning testing fundamentals before introducing GoogleTest, Catch2, or other test frameworks.

## Example Workflow

A user can:

1. Start the simulator
2. Display all ECUs
3. Add a new ECU such as `Transmission`
4. Add a fault to `Transmission`
5. Add an ECU such as `Airbag Control Module`
6. Set `Airbag Control Module` to Offline
7. Scan the vehicle
8. See offline ECUs shown as not responding
9. Display vehicle health
10. Set the ECU back to Online
11. Clear faults from an ECU
12. Display the diagnostic log
13. Exit the program

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
* Automated behavior verification
* Build tooling

Automotive concepts:

* ECUs
* Diagnostic trouble codes
* Fault severity
* Vehicle scanning
* Fault clearing
* ECU communication status
* Offline / not responding ECU behavior
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
* No diagnostic session handling yet
* ECU state is limited to Online and Offline only
* Tests use basic `assert()` instead of a full testing framework

## Planned Improvements

Future versions may include:

* Cleaner reusable input helpers
* More ECU communication states
* ECU readiness states
* Fault history per ECU
* Diagnostic log timestamps
* File persistence
* GoogleTest or Catch2 integration
* CAN bus message simulation
* UDS-inspired diagnostic requests
* ECU state machine behavior
* Improved Linux-first development workflow

## Why This Project Exists

This project is designed to move beyond generic beginner C++ projects and start modeling real systems-style software.

The goal is to build toward a Modern C++ Systems Engineering profile with relevance to automotive, embedded systems, diagnostics, vehicle platforms, cyber-physical systems, and other engineering-heavy industries.

This simulator is an early step toward larger projects involving CAN bus simulation, ECU state machines, diagnostics tooling, and automotive middleware-style architecture.

## Repository Status

This project is currently at **v0.4**.

The current release contains a working diagnostic simulator with a multi-file C++ structure, CMake build support, an interactive menu, ECU modeling, DTC storage, ECU registration, fault scanning, fault clearing, vehicle health checks, diagnostic event logging, ECU communication status, result enums for clearer operation outcomes, and basic assert-based unit tests.