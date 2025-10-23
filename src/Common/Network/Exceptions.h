#pragma once

#include <stdexcept>

class ConnectionError : public std::runtime_error {
public:
    explicit ConnectionError(const std::string& message)
        : std::runtime_error("ConnectionError: " + message) {}
};
