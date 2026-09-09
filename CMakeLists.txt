cmake_minimum_required(VERSION 3.15)

project(vehicleDiagnosticsSimulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(vehicle-diagnostics-simulator
    src/main.cpp
    src/domain/ECU.cpp
    src/uds/UDSResponse.cpp
)

add_executable(vehicle-diagnostics-tests
    tests/tests.cpp
    src/domain/ECU.cpp
    src/uds/UDSResponse.cpp
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
