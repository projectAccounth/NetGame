#pragma once

#include "World.h"

class ClientWorld : public World {
private:
public:
    ClientWorld(): World(false) {};
    ~ClientWorld() override = default;
};