#pragma once
#include "raylib.h"
#include <unordered_map>
#include <string>

class TextureManager {
private:
    // Map from filename to loaded Texture2D
    std::unordered_map<std::string, Texture2D> m_textures;

public:
    TextureManager() = default;
    ~TextureManager();

    // Load a texture if not already loaded; return pointer to it
    Texture2D* LoadTexture(const std::string& fileName);

    // Unload all managed textures (called by destructor)
    void UnloadAllTextures();
};