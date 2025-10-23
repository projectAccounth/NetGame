#pragma once
#include <vector>
#include <string>
#include <cstring>
#include <type_traits>
#include <stdexcept>
#include <cstdint>
#include "../../Util/UUID.hpp"
#include "../../Util/GMath.h"

//
// PacketCodec — handles encoding/decoding primitives and custom types.
//
class PacketCodec {
private:
    std::vector<uint8_t> buffer;
    size_t read_pos = 0;

public:
    PacketCodec() = default;
    explicit PacketCodec(std::vector<uint8_t> data) : buffer(std::move(data)) {}

    const std::vector<uint8_t>& Data() const { return buffer; }
    size_t Size() const { return buffer.size(); }
    void Reset() { buffer.clear(); read_pos = 0; }
    void ResetRead() { read_pos = 0; }

    const std::vector<uint8_t>& GetBuffer() const { return buffer; }
    void SetBuffer(const std::vector<uint8_t>& buf) { buffer = buf; }

    //
    // Encode
    //
    template<typename T>
    void Write(const T& value) {
        static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable.");
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
        buffer.insert(buffer.end(), bytes, bytes + sizeof(T));
    }

    // String (length-prefixed)
    void WriteString(const std::string& s) {
        uint32_t len = static_cast<uint32_t>(s.size());
        Write(len);
        buffer.insert(buffer.end(), s.begin(), s.end());
    }

    // Byte array
    void WriteBytes(const std::vector<uint8_t>& data) {
        uint32_t len = static_cast<uint32_t>(data.size());
        Write(len);
        buffer.insert(buffer.end(), data.begin(), data.end());
    }

    void WriteUUID(const Util::UUID& uuid) {
        WriteBytes(std::vector<uint8_t>(uuid.bytes().begin(), uuid.bytes().end()));
    }

    void WriteFloat(float value) {
        Write(value);
    }

    void WriteDouble(double value) {
        Write(value);
    }

    void WriteVector2(const Vector2d& vec) {
        Write(vec.x);
        Write(vec.y);
    }

    void WriteVector3(const Vector3d& vec) {
        Write(vec.x);
        Write(vec.y);
        Write(vec.z);
    }

    void WriteStringArray(const std::vector<std::string>& vec) {
        Write<size_t>(vec.size());
        for (auto& str : vec) 
            WriteString(str);
    }

    void WriteRect2(const Rect2d& rect) {
        Write<double>(rect.x);
        Write<double>(rect.y);
        Write<double>(rect.width);
        Write<double>(rect.height);
    }

    //
    // Decode
    //
    template<typename T>
    T Read() {
        static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable.");
        if (read_pos + sizeof(T) > buffer.size()) {
            throw std::runtime_error("PacketCodec: insufficient data for read.");
        }
        T value;
        std::memcpy(&value, buffer.data() + read_pos, sizeof(T));
        read_pos += sizeof(T);
        return value;
    }

    std::string ReadString() {
        uint32_t len = Read<uint32_t>();
        if (read_pos + len > buffer.size()) {
            throw std::runtime_error("PacketCodec: string length overflow.");
        }
        std::string s(reinterpret_cast<const char*>(buffer.data() + read_pos), len);
        read_pos += len;
        return s;
    }

    std::vector<uint8_t> ReadBytes() {
        uint32_t len = Read<uint32_t>();
        if (read_pos + len > buffer.size()) {
            throw std::runtime_error("PacketCodec: byte array length overflow.");
        }
        std::vector<uint8_t> data(buffer.begin() + read_pos, buffer.begin() + read_pos + len);
        read_pos += len;
        return data;
    }

    Util::UUID ReadUUID() {
        auto bytes = ReadBytes();
        if (bytes.size() != 16) {
            throw std::runtime_error("PacketCodec: invalid UUID byte length.");
        }
        return Util::UUID(bytes.data());
    }

    Vector2d ReadVector2() {
        double x = Read<double>();
        double y = Read<double>();
        return Vector2d(x, y);
    }

    Vector3d ReadVector3() {
        double x = Read<double>();
        double y = Read<double>();
        double z = Read<double>();
        return Vector3d(x, y, z);
    }

    std::vector<std::string> ReadStringArray() {
        auto size = Read<size_t>();
        std::vector<std::string> out(size);

        for (std::string& str : out) 
            str = ReadString();

        return out;
    }

    Rect2d ReadRect2() {
        double x = Read<double>(),
               y = Read<double>(),
               w = Read<double>(),
               h = Read<double>();
        return Rect2d {
            x, y, w, h
        };
    }
};
