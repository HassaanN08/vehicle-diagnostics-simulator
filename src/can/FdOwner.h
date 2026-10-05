#pragma once

#include <unistd.h>

class FdOwner {
    int m_fd { -1 };

    public:
        explicit FdOwner(int fd) : m_fd { fd } {}

        ~FdOwner() {
            if (m_fd >= 0)
                close(m_fd);
        }

        FdOwner(const FdOwner&) = delete;
        FdOwner& operator=(const FdOwner&) = delete;

        FdOwner(FdOwner&& other) noexcept {
            this->m_fd = other.m_fd;
            other.m_fd = -1;
        }

        FdOwner& operator=(FdOwner&& other) noexcept {
            if (this == &other)
                return *this;

            if (this->m_fd >= 0)
                close(this->m_fd);

            this->m_fd = other.m_fd;
            other.m_fd = -1;

            return *this;
        }

        int getFd() const { return m_fd; }
};