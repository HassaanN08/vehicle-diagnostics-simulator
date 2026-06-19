# Vehicle Diagnostics Simulator Scope

## Goal

For the first working version, the simulator should support:

- Create a vehicle with multiple ECUs.
- Each ECU has a name.
- Each ECU can store fault codes.
- User can scan all ECUs.
- User can view faults.
- User can clear faults from an ECU.
- User can display overall vehicle health.

## Core Concepts

- Vehicle Diagnostics
- ECU
- DTC
- Fault
- Scan Vehicle
- Clear Faults

## Classes I Think I Need

1. DTC

Represents one diagnostic trouble code.

Possible data:

code
description
severity

Example:

P0301
Cylinder 1 Misfire Detected
High

2. ECU

Represents one electronic control unit.

Possible data:

name
list of DTCs

Example:

Engine ECU
Faults: P0301, P0171

3. Vehicle

Represents the whole vehicle.

Possible data:

list of ECUs

Example:

BMW 3 Series Simulator
ECUs: Engine, Brake, Battery

## Things I Do Not Understand Yet

Almost everything, to be honest. Automotive programming is very new to me, so let's see how it goes.