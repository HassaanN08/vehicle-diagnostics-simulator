#pragma once

enum class ClearFaultResult : int {
    ECUNotFound,
    NoFaultsToClear,
    FaultsCleared
};

enum class ECUStatusResult : int {
    ECUNotFound,
    AlreadyInRequestedState,
    StatusChanged
};