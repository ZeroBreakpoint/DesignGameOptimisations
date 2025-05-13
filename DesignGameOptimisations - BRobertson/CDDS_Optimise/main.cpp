#include "raylib.h"
#include "raymath.h"
#include <random>
#include <time.h>
#include "Critter.h"
#include "ObjectPool.h"
#include "TextureManager.h"
#include "QuadTree.h"
#include <iostream>
#include <chrono>
#include <fstream>

int main(int argc, char* argv[])
{

    // Initialise window & timing

    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Design Game Optimised BRobertson");
    
    // Seed random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Constants & managers
 
    const int  CRITTER_COUNT = 50;
    const float MAX_VELOCITY = 80.0f;
    const float RESPAWN_INTERVAL = 1.0f;

    TextureManager textureManager;
    Texture2D* critterTexture = textureManager.LoadTexture("res/10.png");
    Texture2D* destroyerTexture = textureManager.LoadTexture("res/9.png");

    // Pool to recycle Critter objects
    ObjectPool<Critter> critterPool(CRITTER_COUNT);
    Critter* critters[CRITTER_COUNT] = {};

   
    // Spawn initial critters

    for (int i = 0; i < CRITTER_COUNT; ++i)
    {
        critters[i] = critterPool.Get();

        Vector2 velocity = {
            -100.0f + static_cast<float>(rand() % 200),
            -100.0f + static_cast<float>(rand() % 200)
        };
        velocity = Vector2Scale(Vector2Normalize(velocity), MAX_VELOCITY);

        Vector2 position = {
            static_cast<float>(5 + rand() % (screenWidth - 10)),
            static_cast<float>(5 + rand() % (screenHeight - 10))
        };

        critters[i]->Init(position, velocity, 12.0f, critterTexture);
    }


    // Create destroyer critter

    Critter destroyer;
    Vector2 dVel = {
        -100.0f + static_cast<float>(rand() % 200),
        -100.0f + static_cast<float>(rand() % 200)
    };
    dVel = Vector2Scale(Vector2Normalize(dVel), MAX_VELOCITY);
    destroyer.Init({ screenWidth / 2.0f, screenHeight / 2.0f },
        dVel,
        20.0f,
        destroyerTexture);

    // Respawn timer accumulator
    float respawnTimerAcc = RESPAWN_INTERVAL;

    // Predefine quadtree world region (screen bounds)
    static const AABB worldRegion{ {0.0f, 0.0f,
                                   static_cast<float>(screenWidth),
                                   static_cast<float>(screenHeight)} };


    // Create quadtree once and reuse each frame

    QuadTree quadTree(worldRegion);

    // Main game loop

    while (!WindowShouldClose())
    {
        // Clear the quadtree for this frame to avoid reallocating nodes
        quadTree.Clear();

        // Calculate delta time
        float dt = GetFrameTime();

        // --- Update destroyer movement & bouncing ---
        destroyer.Update(dt);
        Vector2 dPos = destroyer.GetPosition();
        Vector2 dVelCurr = destroyer.GetVelocity();
        float   dr = destroyer.GetRadius();

        // Bounce left/right accounting for radius
        if (dPos.x - dr < 0.0f)
        {
            dPos.x = dr;
            dVelCurr.x *= -1.0f;
        }
        else if (dPos.x + dr > screenWidth)
        {
            dPos.x = screenWidth - dr;
            dVelCurr.x *= -1.0f;
        }
        // Bounce top/bottom accounting for radius
        if (dPos.y - dr < 0.0f)
        {
            dPos.y = dr;
            dVelCurr.y *= -1.0f;
        }
        else if (dPos.y + dr > screenHeight)
        {
            dPos.y = screenHeight - dr;
            dVelCurr.y *= -1.0f;
        }
        destroyer.SetPosition(dPos);
        destroyer.SetVelocity(dVelCurr);

        // --- Update critters & handle destroyer collisions ---
        for (int i = 0; i < CRITTER_COUNT; ++i)
        {
            Critter* c = critters[i];
            c->Update(dt);

            Vector2 pos = c->GetPosition();
            Vector2 vel = c->GetVelocity();
            float   r = c->GetRadius();

            // Bounce left/right
            if (pos.x - r < 0.0f)
            {
                pos.x = r;
                vel.x *= -1.0f;
            }
            else if (pos.x + r > screenWidth)
            {
                pos.x = screenWidth - r;
                vel.x *= -1.0f;
            }
            // Bounce top/bottom
            if (pos.y - r < 0.0f)
            {
                pos.y = r;
                vel.y *= -1.0f;
            }
            else if (pos.y + r > screenHeight)
            {
                pos.y = screenHeight - r;
                vel.y *= -1.0f;
            }

            c->SetPosition(pos);
            c->SetVelocity(vel);

            // Collision with destroyer?
            float distToD = Vector2Distance(pos, destroyer.GetPosition());
            if (distToD < r + destroyer.GetRadius())
            {
                c->Destroy();
                critterPool.Return(c);
            }
        }

        // --- Insert critters into re-used quadtree ---
        for (int i = 0; i < CRITTER_COUNT; ++i)
        {
            Critter* c = critters[i];
            if (!c->IsDead())
                quadTree.Insert(c, c->GetPosition());
        }

        // --- Quadtree-accelerated critter–critter collisions ---
        for (int i = 0; i < CRITTER_COUNT; ++i)
        {
            Critter* a = critters[i];
            if (a->IsDead() || a->IsDirty()) continue;

            float range = a->GetRadius() * 2.0f;
            AABB  queryBox{ { a->GetX() - range / 2.0f,
                             a->GetY() - range / 2.0f,
                             range, range} };
            std::vector<Critter*> neighbours;
            quadTree.Query(queryBox, neighbours);

            for (Critter* b : neighbours)
            {
                if (b == a || b->IsDirty()) continue;
                float dist = Vector2Distance(a->GetPosition(), b->GetPosition());
                if (dist < a->GetRadius() + b->GetRadius())
                {
                    Vector2 normal = Vector2Normalize(
                        Vector2Subtract(b->GetPosition(), a->GetPosition())
                    );
                    a->SetVelocity(Vector2Scale(normal, -MAX_VELOCITY));
                    b->SetVelocity(Vector2Scale(normal, MAX_VELOCITY));
                    a->SetDirty();
                    b->SetDirty();
                }
            }
        }

        // --- Respawn logic ---
        respawnTimerAcc -= dt;
        if (respawnTimerAcc <= 0.0f)
        {
            respawnTimerAcc = RESPAWN_INTERVAL;
            for (int i = 0; i < CRITTER_COUNT; ++i)
            {
                Critter* c = critters[i];
                if (c->IsDead())
                {
                    Vector2 dir = Vector2Normalize(destroyer.GetVelocity());
                    Vector2 spawnPos = Vector2Subtract(
                        destroyer.GetPosition(),
                        Vector2Scale(dir, 50.0f)
                    );
                    c->Reset(spawnPos,
                        Vector2Scale(dir, -MAX_VELOCITY),
                        12.0f,
                        critterTexture);
                    break;
                }
            }
        }

        // --- Draw ---
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (int i = 0; i < CRITTER_COUNT; ++i)
            critters[i]->Draw();
        destroyer.Draw();
        DrawFPS(10, 10);
        EndDrawing();
    }

    // Cleanup
    for (int i = 0; i < CRITTER_COUNT; ++i)
        critters[i]->Destroy();
    textureManager.UnloadAllTextures();
    CloseWindow();

    return 0;
}