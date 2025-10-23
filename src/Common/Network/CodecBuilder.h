#pragma once
#include "PacketCodec.h"
#include <functional>

//
// CodecBuilder — allows building encode/decode pipelines.
//
class CodecBuilder {
public:
    using Transform = std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)>;
    using InverseTransform = std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)>;

private:
    std::vector<Transform> encoders;
    std::vector<InverseTransform> decoders;

public:
    CodecBuilder& AddLayer(Transform encode, InverseTransform decode) {
        encoders.push_back(std::move(encode));
        decoders.insert(decoders.begin(), std::move(decode));
        return *this;
    }

    std::vector<uint8_t> Encode(const PacketCodec& codec) const {
        std::vector<uint8_t> data = codec.Data();
        for (auto& e : encoders) {
            data = e(data);
        }
        return data;
    }

    PacketCodec Decode(const std::vector<uint8_t>& raw) const {
        std::vector<uint8_t> data = raw;
        for (auto& d : decoders) {
            data = d(data);
        }
        return PacketCodec(data);
    }
};
