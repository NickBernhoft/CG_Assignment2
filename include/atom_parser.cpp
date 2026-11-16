#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cctype>

#include "atom_parser.hpp"

void getAtomProperties(const std::string& element, GPUAtom& atom) {
    if (element == "C") {
        atom.radius = 1.7f;
        atom.color = { 0.2f, 0.2f, 0.2f, 1.0f };
    }
    else if (element == "O") {
        atom.radius = 1.52f;
        atom.color = { 1.0f, 0.2f, 0.2f, 1.0f };
    }
    else if (element == "P") {
        atom.radius = 1.8f;
        atom.color = { 1.0f, 0.6f, 0.0f, 1.0f };
    }
    else if (element == "N") {
        atom.radius = 1.55f;
        atom.color = { 0.2f, 0.4f, 1.0f, 1.0f };
    }
    else if (element == "S") {
        atom.radius = 1.8f;
        atom.color = { 1.0f, 0.8f, 0.0f, 1.0f };
    }
    else {
        atom.radius = 1.2f;
        atom.color = { 1.0f, 1.0f, 1.0f, 1.0f };
    }
}

std::vector<GPUAtom> parsePDBAndComputeBounds(const std::string& filepath,
    BoundingVolume& outBounds) {
    std::vector<GPUAtom> atoms;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[Error] Failed to open PDB file: " << filepath << std::endl;
        return atoms;
    }

    ParsingState state;
    std::string line;

    while (std::getline(file, line)) {
        if (line.length() < 54) continue;

        std::string recordType = line.substr(0, 6);
        if (recordType == "ATOM  " || recordType == "HETATM") {
            try {
                GPUAtom atom = {};
                atom.x = std::stof(line.substr(30, 8));
                atom.y = std::stof(line.substr(38, 8));
                atom.z = std::stof(line.substr(46, 8));

                // 1. Try reading standard PDB element column (columns 77-78)
                std::string element = "";
                if (line.length() >= 78) {
                    element = line.substr(76, 2);
                    element.erase(std::remove_if(element.begin(), element.end(),
                        [](unsigned char c) { return std::isspace(c); }), element.end());
                }

                // 2. Fall back to atom name column (columns 13-16) if element column was blank
                if (element.empty() && line.length() >= 16) {
                    element = line.substr(12, 4);
                    element.erase(std::remove_if(element.begin(), element.end(),
                        [](unsigned char c) { return std::isspace(c); }), element.end());
                }

                getAtomProperties(element, atom);
                atoms.push_back(atom);

                // --- STREAMING BOUNDING CALCULATION ---
                if (atom.x < state.minX) state.minX = atom.x;
                if (atom.y < state.minY) state.minY = atom.y;
                if (atom.z < state.minZ) state.minZ = atom.z;
                if (atom.x > state.maxX) state.maxX = atom.x;
                if (atom.y > state.maxY) state.maxY = atom.y;
                if (atom.z > state.maxZ) state.maxZ = atom.z;
                if (atom.radius > state.maxAtomRadius) state.maxAtomRadius = atom.radius;
            }
            catch (const std::exception& e) {
                // Skip malformed lines gracefully
                continue;
            }
        }
    }

    if (!atoms.empty()) {
        outBounds.centerX = (state.minX + state.maxX) * 0.5f;
        outBounds.centerY = (state.minY + state.maxY) * 0.5f;
        outBounds.centerZ = (state.minZ + state.maxZ) * 0.5f;

        float halfW = (state.maxX - state.minX) * 0.5f;
        float halfH = (state.maxY - state.minY) * 0.5f;
        float halfD = (state.maxZ - state.minZ) * 0.5f;
        float maxBoxRadius = std::sqrt((halfW * halfW) + (halfH * halfH) + (halfD * halfD));

        outBounds.boundingRadius = maxBoxRadius + state.maxAtomRadius;
    }

    return atoms;
}
