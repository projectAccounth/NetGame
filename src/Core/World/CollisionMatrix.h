#pragma once

#include "Core/Objects/CollisionGroups.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class CollisionMatrix {
private:
    static inline std::unordered_map<CollisionGroup,
        std::unordered_map<CollisionGroup, bool>> matrix;

    static CollisionGroup FromString(const std::string& name) {
        static const std::unordered_map<std::string, CollisionGroup> map = {
            {"DefaultCollidable", CollisionGroup::DefaultCollidable},
            {"DefaultNonCollidable", CollisionGroup::DefaultNonCollidable},
            {"Player", CollisionGroup::Player},
            {"Projectile", CollisionGroup::Projectile}
        };
        auto it = map.find(name);
        if (it != map.end()) return it->second;
        throw std::runtime_error("Unknown CollisionGroup name: " + name);
    }

public:
    static void LoadFromFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open collision config: " + path);
        }

        json data;
        file >> data;

        for (auto& [groupName, collidesWith] : data.items()) {
            CollisionGroup g1 = FromString(groupName);
            for (const auto& otherName : collidesWith) {
                CollisionGroup g2 = FromString(otherName);
                Set(g1, g2, true);
            }
        }

        file.close();
    }

    static void Set(CollisionGroup a, CollisionGroup b, bool value) {
        matrix[a][b] = value;
        matrix[b][a] = value;
    }

    static bool ShouldCollide(CollisionGroup a, CollisionGroup b) {
        auto it1 = matrix.find(a);
        if (it1 == matrix.end()) return false;
        auto it2 = it1->second.find(b);
        if (it2 == it1->second.end()) return false;
        return it2->second;
    }

    static void PrintMatrix() {
        for (auto& [a, pairs] : matrix) {
            std::cout << "Group " << (int)a << " collides with: ";
            for (auto& [b, val] : pairs)
                if (val) std::cout << (int)b << " ";
            std::cout << std::endl;
        }
    }
};
