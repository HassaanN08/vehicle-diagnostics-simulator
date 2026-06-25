#pragma once

enum class ClearFaultResult : int {
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