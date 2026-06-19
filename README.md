# Vehicle Diagnostics Simulator

A C++ console-based vehicle diagnostics simulator that models ECUs, diagnostic trouble codes, vehicle health checks, and basic diagnostic workflows.

This project is part of a long-term automotive C++ portfolio focused on building practical software engineering skills for vehicle platforms, diagnostics, embedded systems, and modern C++ development.

## Current Version

**Version:** v0.1 planning stage
**Status:** Early development

The current stage defines the initial project scope and lays the foundation for a basic diagnostics simulator.

## Project Goal

The goal of this project is to simulate how a diagnostic system might inspect different electronic modules inside a vehicle and report faults.

The first working version will focus on a simplified model:

* A vehicle contains multiple ECUs
* Each ECU can store diagnostic trouble codes
* The user can scan the vehicle
* The user can view active faults
* The user can clear faults
* The system can display overall vehicle health

## Core Concepts

### ECU

An ECU, or Electronic Control Unit, is a small computer inside a vehicle that controls or monitors a specific system.

Examples:

* Engine ECU
* Brake ECU
* Battery ECU
* Transmission ECU
* Airbag ECU

In this simulator, an ECU is represented as a C++ object that can store and report diagnostic trouble codes.

### DTC

A DTC, or Diagnostic Trouble Code, is a code that represents a detected fault.

Example:

```text
P0301 - Cylinder 1 Misfire Detected
```

In this simulator, DTCs will be represented as objects containing information such as:

* Code
* Description
* Severity

### Vehicle Scan

A vehicle scan means checking all ECUs in the vehicle and reporting whether each ECU has active faults.

Example:

```text
Scanning vehicle...

Engine ECU:
P0301 - Cylinder 1 Misfire Detected - High

Brake ECU:
No active faults
```

## Planned v0.1 Features

The first working version of this project will support:

* Creating a vehicle with multiple ECUs
* Adding ECUs to the vehicle
* Adding diagnostic trouble codes to ECUs
* Scanning all ECUs
* Viewing active faults
* Clearing faults from an ECU
* Displaying overall vehicle health

## Planned Classes

The initial design will likely include:

### DTC

Represents one diagnostic trouble code.

Possible data:

* Code
* Description
* Severity

### ECU

Represents one electronic control unit.

Possible data:

* ECU name
* List of active DTCs

### Vehicle

Represents the full vehicle.

Possible data:

* Vehicle name/model
* List of ECUs

## Concepts Practiced

This project is intended to strengthen both C++ fundamentals and automotive software thinking.

C++ concepts:

* Classes and objects
* Encapsulation
* `std::vector`
* Const correctness
* Pointers and references
* Multi-file project structure
* Menu-driven console programs
* Clean class responsibility

Automotive concepts:

* ECUs
* Diagnostic trouble codes
* Vehicle health checks
* Fault storage
* Fault clearing
* Basic diagnostic workflows

## Future Improvements

Future versions may include:

* More realistic diagnostic sessions
* UDS-inspired request/response behavior
* CAN bus message simulation
* Fault severity filtering
* Persistent logging
* File saving and loading
* Unit tests
* CMake support
* Better project structure
* Linux-first development workflow

## Why This Project Exists

This project is part of my path toward becoming a modern C++ automotive software engineer.

The goal is not just to build a generic console application, but to model automotive software concepts in C++ and gradually grow the project toward more realistic diagnostics, vehicle communication, and middleware-style architecture.

## Technologies Used

* C++
* Standard Library
* Console input/output

## How to Run

At the current stage, the project contains a starter `main.cpp`.

Compile with:

```bash
g++ main.cpp -o vehicle-diagnostics-simulator
./vehicle-diagnostics-simulator
```

On Windows:

```bash
g++ main.cpp -o vehicle-diagnostics-simulator.exe
vehicle-diagnostics-simulator.exe
```

## Repository Status

This project is in early development.

The current focus is defining the v0.1 scope and then building the first working diagnostics model step by step.