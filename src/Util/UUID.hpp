#pragma once

#include <array>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <exception>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace Util {

/// @brief A simple UUID (Universally Unique Identifier) class.
class UUID {
public:
    using bytes_t = std::array<std::uint8_t, 16>;

    constexpr UUID() noexcept : data_({0}) {}
    explicit UUID(const bytes_t &b) noexcept : data_(b) {}
    explicit UUID(bytes_t &&b) noexcept : data_(std::move(b)) {}
    explicit UUID(const std::uint8_t *raw) noexcept { std::memcpy(data_.data(), raw, 16); }

    static UUID random() {
        static thread_local std::random_device rd;
        static thread_local std::mt19937_64 gen(rd());
        static thread_local std::uniform_int_distribution<std::uint64_t> dist(0ULL, ~0ULL);

        bytes_t b;
        std::uint64_t a = dist(gen);
        std::uint64_t c = dist(gen);
        for (int i = 0; i < 8; ++i) b[i] = static_cast<std::uint8_t>((a >> (8 * (7 - i))) & 0xFF);
        for (int i = 0; i < 8; ++i) b[8 + i] = static_cast<std::uint8_t>((c >> (8 * (7 - i))) & 0xFF);

        b[6] = static_cast<std::uint8_t>((b[6] & 0x0F) | 0x40);
        b[8] = static_cast<std::uint8_t>((b[8] & 0x3F) | 0x80);
        return UUID(std::move(b));
    }

    static UUID from_string(const std::string &s) {
        UUID out;
        if (!try_parse(s, out)) {
            throw std::invalid_argument("UUID::from_string: invalid UUID string");
        }
        return out;
    }

    static UUID null() noexcept { return UUID(); }

    static bool try_parse(std::string s, UUID &out) noexcept {
        // Trim URN prefix if present
        const std::string urn_prefix = "urn:UUID:";
        if (s.size() >= urn_prefix.size()) {
            std::string prefix = s.substr(0, urn_prefix.size());
            for (auto &c : prefix) c = static_cast<char>(std::tolower(c));
            if (prefix == urn_prefix) s = s.substr(urn_prefix.size());
        }

        // Strip braces if present
        if (!s.empty() && s.front() == '{' && s.back() == '}') {
            s = s.substr(1, s.size() - 2);
        }

        // Remove hyphens
        std::string hex;
        hex.reserve(32);
        for (char c : s) {
            if (c == '-') continue;
            hex.push_back(c);
        }

        if (hex.size() != 32) return false;

        auto hexval = [](char c)->int {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
            if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
            return -1;
        };

        bytes_t b{};
        for (int i = 0; i < 16; ++i) {
            int hi = hexval(hex[i * 2]);
            int lo = hexval(hex[i * 2 + 1]);
            if (hi == -1 || lo == -1) return false;
            b[i] = static_cast<std::uint8_t>((hi << 4) | lo);
        }

        out = UUID(std::move(b));
        return true;
    }

    std::string to_string() const {
        std::ostringstream os;
        os << std::hex << std::setfill('0');
        auto put = [&os](std::uint8_t v) { os << std::setw(2) << static_cast<int>(v); };

        for (int i = 0; i < 4; ++i) put(data_[i]);
        os << '-';
        for (int i = 4; i < 6; ++i) put(data_[i]);
        os << '-';
        for (int i = 6; i < 8; ++i) put(data_[i]);
        os << '-';
        for (int i = 8; i < 10; ++i) put(data_[i]);
        os << '-';
        for (int i = 10; i < 16; ++i) put(data_[i]);
        return os.str();
    }

    const bytes_t &bytes() const noexcept { return data_; }
    bytes_t &bytes() noexcept { return data_; }

    friend bool operator==(const UUID &a, const UUID &b) noexcept { return a.data_ == b.data_; }
    friend bool operator!=(const UUID &a, const UUID &b) noexcept { return !(a == b); }
    friend bool operator<(const UUID &a, const UUID &b) noexcept { return a.data_ < b.data_; }

    friend std::ostream &operator<<(std::ostream &os, const UUID &u) { os << u.to_string(); return os; }

    int version() const noexcept { return (data_[6] >> 4) & 0x0F; }
    int variant() const noexcept {
        std::uint8_t c = data_[8];
        if ((c & 0x80) == 0x00) return 0;
        if ((c & 0xC0) == 0x80) return 2;
        return 6;
    }

private:
    bytes_t data_;
};

} // namespace util

namespace std {
    template<>
    struct hash<Util::UUID> {
        std::size_t operator()(Util::UUID const &u) const noexcept {
            std::uint64_t a = 0, b = 0;
            std::memcpy(&a, u.bytes().data(), 8);
            std::memcpy(&b, u.bytes().data() + 8, 8);
            std::size_t h = static_cast<std::size_t>(a ^ (b << 1));
            return h ^ (h >> 32);
        }
    };
}
