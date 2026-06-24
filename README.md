# Vehicle Diagnostics Simulator

A C++ console-based vehicle diagnostics simulator that models ECUs, diagnostic trouble codes, vehicle health checks, diagnostic event logging, and basic diagnostic workflows.

This project is part of a long-term systems engineering portfolio focused on Modern C++, embedded-style thinking, diagnostics, vehicle platforms, and transferable software architecture skills.

## Current Version

**Version:** v0.2
**Status:** Working release

This version includes a multi-file C++ project structure, CMake build support, diagnostic trouble code modeling, ECU fault storage, vehicle-level scanning, an interactive console menu, ECU registration through the menu, and diagnostic event logging.

## Features

* Create a simulated vehicle with multiple ECUs
* Store diagnostic trouble codes inside ECUs
* Add faults to a specific ECU through the menu
* Display all ECUs
* Scan the full vehicle
* Display active DTCs for each ECU
* Clear faults from a specific ECU
* Display overall vehicle health
* Add new ECUs through the menu
* Reject duplicate ECU names
* Display a diagnostic event log
* Log successful ECU additions
* Log successful fault additions
* Log successful fault clearing
* Log vehicle scan events
* Handle invalid ECU names
* Convert user severity input into a strongly typed `Severity` enum
* Use value ownership for ECUs and DTCs
* Keep ECU search logic encapsulated inside the `Vehicle` class
* Build the project using CMake

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
8. Exit
```

## Project Structure

```text
vehicle-diagnostics-simulator/
├── include/
│   ├── DTC.h
│   ├── ECU.h
│   ├── Severity.h
│   └── Vehicle.h
├── src/
│   ├── DTC.cpp
│   ├── ECU.cpp
│   ├── main.cpp
│   └── Vehicle.cpp
├── notes/
│   └── project-scope.md
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

### ECU

Represents one Electronic Control Unit.

Responsibilities:

* Store ECU name
* Store active DTCs
* Add diagnostic trouble codes
* Display active faults
* Clear all faults
* Report whether active faults exist

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
* Scan all ECUs
* Display all ECUs
* Check overall vehicle health
* Clear faults from a specific ECU
* Store diagnostic event logs
* Display diagnostic event logs
* Encapsulate ECU search logic

## Current Diagnostic Model

The simulator uses this hierarchy:

```text
Vehicle
  └── ECU
        └── DTC
```

A vehicle owns multiple ECUs.
Each ECU owns multiple diagnostic trouble codes.
The menu interacts with the vehicle, and the vehicle coordinates diagnostic actions.

## Diagnostic Log

The simulator now keeps a simple session-level diagnostic log.

The log records successful actions such as:

* Adding a new ECU
* Adding a fault to an ECU
* Clearing faults from an ECU
* Scanning the vehicle

Example:

```text
Diagnostic Log:

1. Added ECU: Transmission
2. Added fault P0301 to Engine
3. Vehicle scan performed
4. Cleared faults from Engine
```

The log does not currently persist after the program closes.

## Example Workflow

A user can:

1. Start the simulator
2. Display all ECUs
3. Add a new ECU such as `Transmission`
4. Add a fault to `Transmission`
5. Scan the vehicle
6. Display vehicle health
7. Clear faults from `Transmission`
8. Display the diagnostic log
9. Exit the program

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
* Console input handling
* Menu-driven program flow
* CMake build configuration

Systems concepts:

* Component hierarchy
* Fault modeling
* Diagnostic scanning
* Health checks
* Module-level responsibility
* Simple system state inspection
* Safe object ownership
* Event logging
* Build tooling

Automotive concepts:

* ECUs
* Diagnostic trouble codes
* Fault severity
* Vehicle scanning
* Fault clearing
* Basic diagnostic workflow
* Diagnostic event history

## How to Run

This project uses CMake.

### Linux / macOS

```bash
cmake -S . -B build
cmake --build build
./build/vehicle-diagnostics-simulator
```

### Windows

```bash
cmake -S . -B build
cmake --build build
build\Debug\vehicle-diagnostics-simulator.exe
```

## Manual Build Alternative

You can also compile manually with `g++`.

### Linux / macOS

```bash
g++ -std=c++17 src/main.cpp src/Vehicle.cpp src/ECU.cpp src/DTC.cpp -Iinclude -o vehicle-diagnostics-simulator
./vehicle-diagnostics-simulator
```

### Windows

```bash
g++ -std=c++17 src/main.cpp src/Vehicle.cpp src/ECU.cpp src/DTC.cpp -Iinclude -o vehicle-diagnostics-simulator.exe
vehicle-diagnostics-simulator.exe
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
* No unit tests yet
* No persistent diagnostic history
* No real vehicle communication
* Limited input validation
* No diagnostic session handling yet
* No ECU state model yet

## Planned Improvements

Future versions may include:

* Cleaner reusable input helpers
* Return stronger result types instead of integer status codes
* ECU state modeling
* Fault history per ECU
* Diagnostic log timestamps
* File persistence
* Unit tests
* CAN bus message simulation
* UDS-inspired diagnostic requests
* ECU state machine behavior
* Improved Linux-first development workflow

## Why This Project Exists

This project is designed to move beyond generic beginner C++ projects and start modeling real systems-style software.

The goal is to build toward a Modern C++ Systems Engineering profile with relevance to automotive, embedded systems, diagnostics, vehicle platforms, cyber-physical systems, and other engineering-heavy industries.

This simulator is an early step toward larger projects involving CAN bus simulation, ECU state machines, diagnostics tooling, and automotive middleware-style architecture.

## Repository Status

This project is currently at **v0.2**.

The current release contains a working diagnostic simulator with a multi-file C++ structure, CMake build support, interactive menu, ECU modeling, DTC storage, ECU registration, fault scanning, fault clearing, vehicle health checks, and diagnostic event logging.