#pragma once
#include "raylib.h"

class Critter
{
protected:
    Vector2 m_position;     // Current position in world-space
    Vector2 m_velocity;     // Movement vector 
    float   m_radius;       // Collision radius

    Texture2D* m_texture;   // Pointer to texture resource

    bool m_isLoaded;        // Is this critter currently active/loaded
    bool m_isDirty;         // Has this critter already been processed this frame

public:
    // Constructor / Destructor
    Critter();
    ~Critter();

    // Initialise the critter with starting values
    void Init(Vector2 position, Vector2 velocity, float radius, Texture2D* texture);

    // Mark the critter as inactive
    void Destroy();

    // Update position & internal state; dt = delta time since last frame
    void Update(float dt);

    // Draw the critter to the screen if loaded
    void Draw();

    // Getters and setters for position, velocity, radius
    float   GetX() const { return m_position.x; }
    float   GetY() const { return m_position.y; }
    void    SetX(float x) { m_position.x = x; }
    void    SetY(float y) { m_position.y = y; }

    Vector2 GetPosition() const { return m_position; }
    void    SetPosition(Vector2 position) { m_position = position; }

    Vector2 GetVelocity() const { return m_velocity; }
    void    SetVelocity(Vector2 velocity) { m_velocity = velocity; }

    float   GetRadius() const { return m_radius; }

    // Dirty flag indicates we've already handled a collision this frame
    bool    IsDirty() const { return m_isDirty; }
    void    SetDirty() { m_isDirty = true; }

    // Is this critter inactive/dead?
    bool    IsDead() const { return !m_isLoaded; }

    // Reset to a new position/velocity/radius/texture and mark active
    void    Reset(Vector2 position, Vector2 velocity, float radius, Texture2D* texture);
};

