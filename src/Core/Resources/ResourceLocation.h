#pragma once

#include <algorithm>
#include <string>
#include <filesystem>

// Represents a resource location, e.g. "textures/default/player.png"
class ResourceLocation {
private:
    std::string pNamespace;
    std::string path;

    static inline bool CaseInsensitive = true;
    static inline bool NormalizeSlashes = true;

    static std::string NormalizeString(std::string s) {
        if (NormalizeSlashes) {
            std::replace(s.begin(), s.end(), '\\', '/');
        }

        if (CaseInsensitive) {
            std::transform(s.begin(), s.end(), s.begin(),
                [](unsigned char c) { return std::tolower(c); });
        }

        // Normalize redundant components
        std::filesystem::path p(s);
        p = p.lexically_normal();

        return p.generic_string();
    }

public:
    std::string GetNamespace() const { return pNamespace; }
    std::string GetPath() const { return path; }

    ResourceLocation(std::string ns, std::string p) : pNamespace(std::move(ns)), path(std::move(p)) {
    }

    ResourceLocation(std::string p): pNamespace("default"), path(std::move(p)) {}

    std::string ToString() const {
        return pNamespace + ":" + path;
    }

    static ResourceLocation FromString(const std::string& str) {
        auto pos = str.find(':');
        if (pos == std::string::npos) {
            return ResourceLocation("default", str);
        } else {
            return ResourceLocation(str.substr(0, pos), str.substr(pos + 1));
        }
    }

    static ResourceLocation FromPath(const std::string& path) {
        return ResourceLocation("default", path);
    }

    static ResourceLocation FromNamespaceAndPath(const std::string& ns, const std::string& path) {
        return ResourceLocation(ns, path);
    }

    static ResourceLocation Empty() {
        return ResourceLocation("", "");
    }

    constexpr std::string GetTexturePath() const {
        return "textures/" + pNamespace + "/" + path;
    }

    constexpr std::string GetModelPath() const {
        return "models/" + pNamespace + "/" + path;
    }

    constexpr std::string GetSoundPath() const {
        return "sounds/" + pNamespace + "/" + path;
    }

    constexpr std::string GetScriptPath() const {
        return "scripts/" + pNamespace + "/" + path;
    }

    constexpr std::string GetRawPath() const {
        return pNamespace + "/" + path;
    }

    constexpr std::string GetRawDataPath() const {
        return "data/" + pNamespace + "/" + path;
    }

    static void SetCaseInsensitive(bool v) { CaseInsensitive = v; }
    static void SetNormalizeSlashes(bool v) { NormalizeSlashes = v; }

    bool operator==(const ResourceLocation& other) const {
        return NormalizeString(pNamespace.empty() ? "default" : pNamespace)
                == NormalizeString(other.pNamespace.empty() ? "default" : other.pNamespace)
            && NormalizeString(path) == NormalizeString(other.path);
    }

    bool operator!=(const ResourceLocation& other) const {
        return !(*this == other);
    }

    bool operator<(const ResourceLocation& other) const {
        return (pNamespace < other.pNamespace) || 
               (pNamespace == other.pNamespace && path < other.path);
    }

    // Hash function for use in unordered containers
    struct Hash {
        std::size_t operator()(const ResourceLocation& rl) const {
            std::string ns = NormalizeString(rl.pNamespace.empty() ? "default" : rl.pNamespace);
            std::string path = NormalizeString(rl.path);
            return std::hash<std::string>()(ns) ^ (std::hash<std::string>()(path) << 1);
        }
    };
};