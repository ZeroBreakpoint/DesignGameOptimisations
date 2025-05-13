#include "TextureManager.h"

TextureManager::~TextureManager() {
    UnloadAllTextures();
}

// Load a texture from disk if it's not already in the cache.  Uses raylib's LoadTexture internally.  fileName  Relative or absolute path to texture file.  Pointer to managed Texture2D.

Texture2D* TextureManager::LoadTexture(const std::string& fileName) {
    // If not already loaded, load and store it
    if (m_textures.find(fileName) == m_textures.end()) {
        m_textures[fileName] = ::LoadTexture(fileName.c_str());
    }
    // Return address of stored Texture2D
    return &m_textures[fileName];
}

// Unload every texture and clear the cache.  Uses raylib's UnloadTexture.

void TextureManager::UnloadAllTextures() {
    for (auto& pair : m_textures)
        UnloadTexture(pair.second);

    m_textures.clear();
}