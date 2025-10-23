#pragma once
#include <vector>
#include <cstdint>
#include <cstring>
#include <stdexcept>

class ByteView {
    const uint8_t* data_;
    size_t size_;
public:
    ByteView() : data_(nullptr), size_(0) {}
    ByteView(const void* data, size_t size) : data_((const uint8_t*)data), size_(size) {}

    const uint8_t* data() const { return data_; }
    size_t size() const { return size_; }

    ByteView slice(size_t offset, size_t length) const {
        if (offset + length > size_) throw std::out_of_range("slice out of range");
        return ByteView(data_ + offset, length);
    }
};

class ByteBuffer {
    std::vector<uint8_t> buf;
public:
    ByteBuffer() = default;
    explicit ByteBuffer(size_t size) : buf(size) {}

    uint8_t* data() { return buf.data(); }
    const uint8_t* data() const { return buf.data(); }
    size_t size() const { return buf.size(); }

    void clear() { buf.clear(); }

    void append(const void* src, size_t len) {
        size_t old = buf.size();
        buf.resize(old + len);
        std::memcpy(buf.data() + old, src, len);
    }

    void append(const ByteView& view) { append(view.data(), view.size()); }

    std::vector<uint8_t>& vec() { return buf; }
    const std::vector<uint8_t>& vec() const { return buf; }

    ByteView view() const { return ByteView(buf.data(), buf.size()); }
};