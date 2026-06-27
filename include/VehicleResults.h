#pragma once

enum class ClearFaultResult : int {
    SessionNotAllowed,
    ECUNotFound,
    NoFaultsToClear,
    FaultsCleared,
    ECUOffline
};

enum class ECUStatusResult : int {
    ECUNotFound,
    AlreadyInRequestedState,
    StatusChanged
};

enum class DiagnosticSessionResult : int {
    AlreadyInRequestedState,
    SessionChanged
};