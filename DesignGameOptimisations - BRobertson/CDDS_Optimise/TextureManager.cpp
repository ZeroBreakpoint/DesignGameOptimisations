#include "TextureManager.h"

TextureManager::~TextureManager() {
    UnloadAllTextures();
}

Texture2D* TextureManager::LoadTexture(const std::string& fileName) {
    if (m_textures.find(fileName) == m_textures.end()) {
        m_textures[fileName] = ::LoadTexture(fileName.c_str());
    }
    return &m_textures[fileName];
}

void TextureManager::UnloadAllTextures() {
    for (auto& texturePair : m_textures) {
        UnloadTexture(texturePair.second);
    }
    m_textures.clear();
}