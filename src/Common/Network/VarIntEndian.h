#pragma once
#include <cstdint>
#include <vector>
#include <stdexcept>
#include <type_traits>
#include <cstring>

//
// Endian helpers (big-endian network order)
//
template<typename T>
inline std::enable_if_t<std::is_integral_v<T>, void>
writeBE(std::vector<uint8_t>& out, T value) {
    static_assert(std::is_integral_v<T>, "writeBE requires integral type");
    size_t n = sizeof(T);
    out.resize(out.size() + n);
    for (size_t i = 0; i < n; ++i) {
        out[out.size() - n + i] = static_cast<uint8_t>((value >> (8 * (n - 1 - i))) & 0xFF);
    }
}

template<typename T>
inline std::enable_if_t<std::is_integral_v<T>, T>
readBE(const uint8_t* ptr, size_t available) {
    static_assert(std::is_integral_v<T>, "readBE requires integral type");
    size_t n = sizeof(T);
    if (available < n) throw std::out_of_range("readBE overflow");
    T value = 0;
    for (size_t i = 0; i < n; ++i) {
        value = static_cast<T>((value << 8) | ptr[i]);
    }
    return value;
}

//
// Varint (unsigned) - LEB128 / base-128
//
inline std::vector<uint8_t> encodeVarUInt64(uint64_t v) {
    std::vector<uint8_t> out;
    while (v >= 0x80) {
        out.push_back(static_cast<uint8_t>((v & 0x7F) | 0x80));
        v >>= 7;
    }
    out.push_back(static_cast<uint8_t>(v));
    return out;
}

// returns pair(value, bytesConsumed)
inline std::pair<uint64_t, size_t> decodeVarUInt64(const uint8_t* ptr, size_t available) {
    uint64_t result = 0;
    unsigned int shift = 0;
    size_t i = 0;
    while (i < available) {
        uint8_t byte = ptr[i++];
        result |= static_cast<uint64_t>(byte & 0x7F) << shift;
        if ((byte & 0x80) == 0) {
            return { result, i };
        }
        shift += 7;
        if (shift >= 64) throw std::out_of_range("varuint64 overflow");
    }
    throw std::out_of_range("varuint64 truncated");
}

//
// ZigZag for signed integers
//
inline uint64_t zigzagEncode64(int64_t v) {
    return (static_cast<uint64_t>(v) << 1) ^ static_cast<uint64_t>(v >> 63);
}
inline int64_t zigzagDecode64(uint64_t v) {
    return static_cast<int64_t>((v >> 1) ^ (~(v & 1) + 1));
}

// Convenience wrappers for 32-bit as well
inline std::vector<uint8_t> encodeVarUInt32(uint32_t v) {
    return encodeVarUInt64(static_cast<uint64_t>(v));
}
inline std::pair<uint32_t, size_t> decodeVarUInt32(const uint8_t* ptr, size_t available) {
    auto [v, n] = decodeVarUInt64(ptr, available);
    if (v > UINT32_MAX) throw std::out_of_range("varuint32 overflow");
    return { static_cast<uint32_t>(v), n };
}
inline std::vector<uint8_t> encodeVarInt32(int32_t v) {
    return encodeVarUInt32(static_cast<uint32_t>(zigzagEncode64(v)));
}
inline std::pair<int32_t, size_t> decodeVarInt32(const uint8_t* ptr, size_t available) {
    auto [uv, n] = decodeVarUInt32(ptr, available);
    return { static_cast<int32_t>(zigzagDecode64(uv)), n };
}
