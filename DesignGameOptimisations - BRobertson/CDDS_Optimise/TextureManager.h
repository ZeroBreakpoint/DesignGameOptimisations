#pragma once
#include "raylib.h"
#include <unordered_map>
#include <string>

class TextureManager {
private:
    std::unordered_map<std::string, Texture2D> m_textures;

public:
    TextureManager() = default;
    ~TextureManager();

    Texture2D* LoadTexture(const std::string& fileName);
    void UnloadAllTextures();
};