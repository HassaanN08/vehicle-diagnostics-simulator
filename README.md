# Vehicle Diagnostics Simulator

A C++ console-based vehicle diagnostics simulator that models ECUs, diagnostic trouble codes, vehicle health checks, and basic diagnostic workflows.

This project is part of a long-term systems engineering portfolio focused on Modern C++, embedded-style thinking, diagnostics, vehicle platforms, and transferable software architecture skills.

## Current Version

**Version:** v0.1
**Status:** First working release

This version includes a multi-file C++ project structure, diagnostic trouble code modeling, ECU fault storage, vehicle-level scanning, and an interactive console menu.

## Features

* Create a simulated vehicle with multiple ECUs
* Store diagnostic trouble codes inside ECUs
* Add faults to a specific ECU through the menu
* Display all ECUs
* Scan the full vehicle
* Display active DTCs for each ECU
* Clear faults from a specific ECU
* Display overall vehicle health
* Handle invalid ECU names
* Convert user severity input into a strongly typed `Severity` enum
* Use value ownership for ECUs and DTCs
* Keep search logic encapsulated inside the `Vehicle` class

## Menu Options

The current console menu supports:

```text
Vehicle Diagnostics Simulator

1. Display all ECUs
2. Scan vehicle
3. Display vehicle health
4. Add fault to ECU
5. Clear faults from ECU
6. Exit
```

## Project Structure

```text
vehicle-diagnostics-simulator/
├── DTC.h
├── DTC.cpp
├── ECU.h
├── ECU.cpp
├── Vehicle.h
├── Vehicle.cpp
├── Severity.h
├── main.cpp
├── README.md
└── .gitignore
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

### Vehicle

Represents the full simulated vehicle.

Responsibilities:

* Store multiple ECUs
* Add ECUs
* Add faults to a specific ECU
* Scan all ECUs
* Display all ECUs
* Check overall vehicle health
* Clear faults from a specific ECU
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

## Example Workflow

A user can:

1. Start the simulator
2. Display all ECUs
3. Scan the vehicle
4. Add a DTC to the Engine ECU
5. Add a DTC to the Brake ECU
6. Scan the vehicle again
7. See active faults
8. Display vehicle health
9. Clear faults from one ECU
10. Scan again
11. Exit the program

## Concepts Practiced

This project strengthens both C++ fundamentals and systems-style thinking.

C++ concepts:

* Classes and objects
* Encapsulation
* Multi-file project structure
* Header and implementation separation
* `std::vector`
* `enum class`
* Const member functions
* Passing by reference and const reference
* Private helper methods
* Value ownership
* Console input handling
* Menu-driven program flow

Systems concepts:

* Component hierarchy
* Fault modeling
* Diagnostic scanning
* Health checks
* Module-level responsibility
* Simple system state inspection
* Safe object ownership

Automotive concepts:

* ECUs
* Diagnostic trouble codes
* Fault severity
* Vehicle scanning
* Fault clearing
* Basic diagnostic workflow

## How to Run

Compile the project using a C++ compiler such as `g++`.

### Linux / macOS

```bash
g++ -std=c++17 main.cpp Vehicle.cpp ECU.cpp DTC.cpp -o vehicle-diagnostics-simulator
./vehicle-diagnostics-simulator
```

### Windows

```bash
g++ -std=c++17 main.cpp Vehicle.cpp ECU.cpp DTC.cpp -o vehicle-diagnostics-simulator.exe
vehicle-diagnostics-simulator.exe
```

## Current Limitations

This is an early version and intentionally keeps the model simple.

Current limitations:

* ECUs are hardcoded at startup
* No file saving or loading
* No CAN bus simulation yet
* No UDS request/response behavior yet
* No CMake build system yet
* No unit tests yet
* No persistent diagnostic history
* No real vehicle communication

## Planned Improvements

Future versions may include:

* Add ECU through the menu
* Cleaner reusable input helpers
* Return `bool` from vehicle-level actions for better error handling
* CMake support
* Unit tests
* Fault history/logging
* File persistence
* CAN bus message simulation
* UDS-inspired diagnostic requests
* ECU state machine behavior
* Improved Linux-first development workflow

## Why This Project Exists

This project is designed to move beyond generic beginner C++ projects and start modeling real systems-style software.

The goal is to build toward a Modern C++ Systems Engineering profile with relevance to automotive, embedded systems, diagnostics, vehicle platforms, cyber-physical systems, and other engineering-heavy industries.

This simulator is the first step toward larger projects involving CAN bus simulation, ECU state machines, diagnostics tooling, and automotive middleware-style architecture.

## Repository Status

This project is currently at **v0.1**.

The current release contains the first working diagnostic simulator with a multi-file C++ structure, interactive menu, ECU modeling, DTC storage, fault scanning, and fault clearing.