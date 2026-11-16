#pragma once
#include <array>
#include <string>
#include <vector>

struct BoundingVolume {
    float centerX, centerY, centerZ;
    float boundingRadius;
};
// Global bounds tracked on-the-fly during file streaming
struct ParsingState {
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float minZ = std::numeric_limits<float>::max();
    float maxX = -std::numeric_limits<float>::max();
    float maxY = -std::numeric_limits<float>::max();
    float maxZ = -std::numeric_limits<float>::max();
    float maxAtomRadius = 0.0f;
};
struct GPUAtom {
    float x, y, z;
    float radius;
    std::array<float, 4> color;
};

std::vector<GPUAtom> parsePDBAndComputeBounds(const std::string& filepath, BoundingVolume& outBounds);
void getAtomProperties(const std::string& element, GPUAtom& atom);