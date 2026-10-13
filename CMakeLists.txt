cmake_minimum_required(VERSION 3.15)

project(vehicleDiagnosticsSimulator LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_library(vds-core
    src/domain/ECU.cpp
    src/domain/Vehicle.cpp
    src/uds/UDSRequestProcessor/ClearDiagnosticInformation.cpp
    src/uds/UDSRequestProcessor/DiagnosticSessionControl.cpp
    src/uds/UDSRequestProcessor/ReadDataByIdentifier.cpp
    src/uds/UDSRequestProcessor/ReadDTCInformation.cpp
    src/isotp/Receiver.cpp
    src/isotp/Sender.cpp
    src/can/CANFrame.cpp
    src/isotp/IsoTp.cpp
    src/can/CanSocket.cpp
)

add_executable(vehicle-diagnostics-simulator
    src/main.cpp
)
add_executable(vehicle-diagnostics-tests
    tests/tests.cpp
    tests/implementation/diagnosticCoordinatorTests.cpp
)

target_link_libraries(vehicle-diagnostics-simulator PRIVATE vds-core)
target_link_libraries(vehicle-diagnostics-tests PRIVATE vds-core)

#PUBLIC keyword automatically adds src as a directory to any targets vds-core is linked with
target_include_directories(vds-core PUBLIC src)
target_include_directories(vehicle-diagnostics-tests PRIVATE tests)

if(MSVC)
    target_compile_options(vds-core PRIVATE /W4)
    target_compile_options(vehicle-diagnostics-simulator PRIVATE /W4)
    target_compile_options(vehicle-diagnostics-tests     PRIVATE /W4)
else()
    target_compile_options(vds-core PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(vehicle-diagnostics-simulator PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(vehicle-diagnostics-tests     PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(
    NAME tests
    COMMAND vehicle-diagnostics-tests
)
