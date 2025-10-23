#pragma once

enum class RenderLayer {
    Entities,
    Props,
    Terrain,
    Default
};

struct RenderInfo {
    RenderLayer layer = RenderLayer::Default;
    float yLayer = 0;
};