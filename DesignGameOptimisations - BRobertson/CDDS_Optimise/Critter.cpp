#include "Critter.h"



Critter::Critter()
    : m_position{ 0, 0 }
    , m_velocity{ 0, 0 }
    , m_radius{ 0 }
    , m_texture(nullptr)
    , m_isLoaded(false)
    , m_isDirty(false)
{
}

// Destructor marks the critter as unloaded.

Critter::~Critter()
{
    m_isLoaded = false;
}

 
// Initialise the critter with position, velocity, radius, and texture. position starting position in world-space. velocity initial movement vector. radius collision radius. texture pointer to a loaded Texture2D.

void Critter::Init(Vector2 position, Vector2 velocity, float radius, Texture2D* texture)
{
    m_position = position;
    m_velocity = velocity;
    m_radius = radius;
    m_texture = texture;
    m_isLoaded = true;
    // New critter has no pending collisions
    m_isDirty = false;
}


// Mark the critter as inactive/dead.

void Critter::Destroy()
{
    m_isLoaded = false;
}

// Update the critter's position based on velocity and delta time.  Clears the 'dirty' flag so collisions can be re-checked next frame.  dt Time elapsed since last update (seconds).

void Critter::Update(float dt)
{
    if (!m_isLoaded)
        return;  // Skip inactive critters

    // Move by velocity * delta time
    m_position.x += m_velocity.x * dt;
    m_position.y += m_velocity.y * dt;

    // Ready for next collision check
    m_isDirty = false;
}

// Draw the critter if it's active.  Cast positions to int for pixel-perfect placement.

void Critter::Draw()
{
    if (!m_isLoaded)
        return;  // Skip inactive critters

    // DrawTexture expects an int position
    DrawTexture(*m_texture,
        static_cast<int>(m_position.x),
        static_cast<int>(m_position.y),
        WHITE);  // WHITE is a Raylib colour constant
}
// Reset the critter for respawning: new position, velocity, radius, texture.  Marks it as active and clears the dirty flag.
 
void Critter::Reset(Vector2 position, Vector2 velocity, float radius, Texture2D* texture)
{
    m_position = position;
    m_velocity = velocity;
    m_radius = radius;
    m_texture = texture;
    m_isLoaded = true;
    m_isDirty = false;
}