cmake_minimum_required(VERSION 3.15)

project(vehicleDiagnosticsSimulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(vehicle-diagnostics-simulator
    src/main.cpp
    src/domain/ECU.cpp
    src/domain/Vehicle.cpp
    src/uds/UDSRequestProcessor/ClearDiagnosticInformation.cpp
    src/uds/UDSRequestProcessor/DiagnosticSessionControl.cpp
    src/uds/UDSRequestProcessor/ReadDataByIdentifier.cpp
    src/isotp/Receiver.cpp
    src/isotp/Sender.cpp
    src/can/CANFrame.cpp
    src/isotp/IsoTp.cpp
)

add_executable(vehicle-diagnostics-tests
    tests/tests.cpp
    src/domain/ECU.cpp
    src/domain/Vehicle.cpp
    src/uds/UDSRequestProcessor/ClearDiagnosticInformation.cpp
    src/uds/UDSRequestProcessor/DiagnosticSessionControl.cpp
    src/uds/UDSRequestProcessor/ReadDataByIdentifier.cpp
    src/isotp/Receiver.cpp
    src/isotp/Sender.cpp
    src/can/CANFrame.cpp
    src/isotp/IsoTp.cpp
)

target_include_directories(vehicle-diagnostics-simulator 
    PRIVATE 
        src
)

target_include_directories(vehicle-diagnostics-tests
    PRIVATE 
        src
        tests
)
