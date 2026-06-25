cmake_minimum_required(VERSION 3.15)

project(vehicleDiagnosticsSimulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(vehicle-diagnostics-simulator
    src/DTC.cpp
    src/ECU.cpp
    src/main.cpp
    src/Vehicle.cpp
)

add_executable(vehicle-diagnostics-tests
    tests/test_vehicle.cpp
    src/DTC.cpp
    src/ECU.cpp
    src/Vehicle.cpp
)

target_include_directories(vehicle-diagnostics-simulator PRIVATE include)

target_include_directories(vehicle-diagnostics-tests PRIVATE include)