#pragma once
#include "ByteBuffer.h"
#include "CodecBuilder.h"
#include "PacketRegistry.h"
#include "VarIntEndian.h"
#include <string>
#include <vector>

class PacketWriter {
    ByteBuffer buf;
public:
    PacketWriter() = default;

    // fixed-size BE write
    template<typename T>
    void writeBE(T value) {
        static_assert(std::is_integral_v<T>, "writeBE numeric only");
        std::vector<uint8_t> tmp;
        ::writeBE<T>(tmp, value);     // writes into tmp vector
        buf.append(tmp.data(), tmp.size());
    }

    // raw write (little or host order) for cheap append:
    template<typename T>
    void writeRaw(T value) {
        static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>);
        buf.append(&value, sizeof(T));
    }

    // varints
    void writeVarUInt32(uint32_t v) {
        auto tmp = encodeVarUInt32(v);
        buf.append(tmp.data(), tmp.size());
    }
    void writeVarUInt64(uint64_t v) {
        auto tmp = encodeVarUInt64(v);
        buf.append(tmp.data(), tmp.size());
    }
    void writeVarInt32(int32_t v) {
        auto tmp = encodeVarInt32(v);
        buf.append(tmp.data(), tmp.size());
    }
    void writeVarInt64(int64_t v) {
        auto tmp = encodeVarUInt64(zigzagEncode64(v));
        buf.append(tmp.data(), tmp.size());
    }

    // strings (length-prefixed with varuint)
    void writeString(const std::string& s) {
        writeVarUInt32(static_cast<uint32_t>(s.size()));
        if (!s.empty()) buf.append(s.data(), s.size());
    }

    ByteBuffer& buffer() { return buf; }
};

class PacketReader {
    ByteView view;
    size_t offset = 0;
public:
    PacketReader(ByteView v) : view(v), offset(0) {}

    template<typename T>
    T readBE() {
        static_assert(std::is_integral_v<T>, "readBE numeric only");
        if (offset + sizeof(T) > view.size()) throw std::out_of_range("readBE overflow");
        T val = ::readBE<T>(view.data() + offset, view.size() - offset);
        offset += sizeof(T);
        return val;
    }

    template<typename T>
    T readRaw() {
        static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>);
        if (offset + sizeof(T) > view.size()) throw std::out_of_range("readRaw overflow");
        T val;
        std::memcpy(&val, view.data() + offset, sizeof(T));
        offset += sizeof(T);
        return val;
    }

    uint32_t readVarUInt32() {
        auto [v, n] = decodeVarUInt32(view.data() + offset, view.size() - offset);
        offset += n;
        return v;
    }
    uint64_t readVarUInt64() {
        auto [v, n] = decodeVarUInt64(view.data() + offset, view.size() - offset);
        offset += n;
        return v;
    }
    int32_t readVarInt32() {
        auto [v, n] = decodeVarInt32(view.data() + offset, view.size() - offset);
        offset += n;
        return v;
    }
    int64_t readVarInt64() {
        auto [uv, n] = decodeVarUInt64(view.data() + offset, view.size() - offset);
        offset += n;
        return zigzagDecode64(uv);
    }

    std::string readString() {
        uint32_t len = readVarUInt32();
        if (offset + len > view.size()) throw std::out_of_range("string overflow");
        std::string s(reinterpret_cast<const char*>(view.data() + offset), len);
        offset += len;
        return s;
    }

    bool eof() const { return offset >= view.size(); }
};

class PacketIO {
private:
    const PacketRegistry& registry;
    CodecBuilder codec_builder;

public:
    explicit PacketIO(const PacketRegistry& reg, CodecBuilder builder = {})
        : registry(reg), codec_builder(std::move(builder)) {}

    // Encode a packet into raw bytes
    std::vector<uint8_t> EncodePacket(const Packet& packet) const {
        PacketCodec codec;
        codec.Write<uint32_t>(packet.GetPacketID());
        packet.Encode(codec);
        return codec_builder.Encode(codec);
    }

    // Decode raw bytes into packet instance
    std::unique_ptr<Packet> DecodePacket(const std::vector<uint8_t>& data) const {
        auto codec = codec_builder.Decode(data);
        uint32_t id = codec.Read<uint32_t>();
        auto pkt = registry.Create(id);
        pkt->Decode(codec);
        return pkt;
    }
};