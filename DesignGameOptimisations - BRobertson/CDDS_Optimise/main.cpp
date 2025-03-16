#include "raylib.h"
#include "raymath.h"
#include <random>
#include <time.h>
#include "Critter.h"
#include "ObjectPool.h"
#include "TextureManager.h"
#include <iostream>
#include <chrono>
#include <fstream>

int main(int argc, char* argv[])
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Design Game Optimised BRobertson");

    //SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    srand(static_cast<unsigned int>(time(NULL)));

    const int CRITTER_COUNT = 50;
    const int MAX_VELOCITY = 80;

    TextureManager textureManager;
    Texture2D* critterTexture = textureManager.LoadTexture("res/10.png");
    Texture2D* destroyerTexture = textureManager.LoadTexture("res/9.png");

    ObjectPool<Critter> critterPool(CRITTER_COUNT);
    Critter* critters[CRITTER_COUNT]{};

    // create some critters
    for (int i = 0; i < CRITTER_COUNT; i++)
    {
        critters[i] = critterPool.Get();
        // create a random direction vector for the velocity
        Vector2 velocity = { -100.0f + static_cast<float>(rand() % 200), -100.0f + static_cast<float>(rand() % 200) };
        // normalize and scale by a random speed
        velocity = Vector2Scale(Vector2Normalize(velocity), MAX_VELOCITY);

        // create a critter in a random location
        critters[i]->Init(
            { static_cast<float>(5 + rand() % (screenWidth - 10)), static_cast<float>(5 + rand() % (screenHeight - 10)) },
            velocity,
            12, critterTexture);
    }

    Critter destroyer;
    Vector2 velocity = { -100.0f + static_cast<float>(rand() % 200), -100.0f + static_cast<float>(rand() % 200) };
    velocity = Vector2Scale(Vector2Normalize(velocity), MAX_VELOCITY);
    destroyer.Init(Vector2{ (float)(screenWidth >> 1), (float)(screenHeight >> 1) }, velocity, 20, destroyerTexture);

    float timer = 1;
    Vector2 nextSpawnPos = destroyer.GetPosition();

    auto startTime = std::chrono::high_resolution_clock::now();
    int frameCounter = 0;
    float totalFrameTime = 0;


    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        float delta = GetFrameTime();

        // Log frame time and FPS
        totalFrameTime += delta;
        frameCounter++;
        int fps = GetFPS();
        /*logFile << "Frame Time: " << delta * 1000.0f << " ms, FPS: " << fps << std::endl;*/

        // Update the destroyer
        destroyer.Update(delta);
        // Check each critter against screen bounds
        if (destroyer.GetX() < 0) {
            destroyer.SetX(0);
            destroyer.SetVelocity(Vector2{ -destroyer.GetVelocity().x, destroyer.GetVelocity().y });
        }
        if (destroyer.GetX() > screenWidth) {
            destroyer.SetX(static_cast<float>(screenWidth));
            destroyer.SetVelocity(Vector2{ -destroyer.GetVelocity().x, destroyer.GetVelocity().y });
        }
        if (destroyer.GetY() < 0) {
            destroyer.SetY(0);
            destroyer.SetVelocity(Vector2{ destroyer.GetVelocity().x, -destroyer.GetVelocity().y });
        }
        if (destroyer.GetY() > screenHeight) {
            destroyer.SetY(static_cast<float>(screenHeight));
            destroyer.SetVelocity(Vector2{ destroyer.GetVelocity().x, -destroyer.GetVelocity().y });
        }

        // Update the critters
        // (dirty flags will be cleared during update)
        for (int i = 0; i < CRITTER_COUNT; i++)
        {
            critters[i]->Update(delta);

            // Check each critter against screen bounds
            if (critters[i]->GetX() < 0) {
                critters[i]->SetX(0);
                critters[i]->SetVelocity(Vector2{ -critters[i]->GetVelocity().x, critters[i]->GetVelocity().y });
            }
            if (critters[i]->GetX() > screenWidth) {
                critters[i]->SetX(static_cast<float>(screenWidth));
                critters[i]->SetVelocity(Vector2{ -critters[i]->GetVelocity().x, critters[i]->GetVelocity().y });
            }
            if (critters[i]->GetY() < 0) {
                critters[i]->SetY(0);
                critters[i]->SetVelocity(Vector2{ critters[i]->GetVelocity().x, -critters[i]->GetVelocity().y });
            }
            if (critters[i]->GetY() > screenHeight) {
                critters[i]->SetY(static_cast<float>(screenHeight));
                critters[i]->SetVelocity(Vector2{ critters[i]->GetVelocity().x, -critters[i]->GetVelocity().y });
            }

            // Kill any critter touching the destroyer
            // Simple circle-to-circle collision check
            float dist = Vector2Distance(critters[i]->GetPosition(), destroyer.GetPosition());
            if (dist < critters[i]->GetRadius() + destroyer.GetRadius())
            {
                critters[i]->Destroy();
                // This would be the perfect time to put the critter into an object pool
                critterPool.Return(critters[i]);
            }
        }

        // Check for critter-on-critter collisions
        for (int i = 0; i < CRITTER_COUNT; i++)
        {
            for (int j = 0; j < CRITTER_COUNT; j++) {
                if (i == j || critters[i]->IsDirty()) // Note: the other critter (j) could be dirty - that's OK
                    continue;
                // Check every critter against every other critter
                float dist = Vector2Distance(critters[i]->GetPosition(), critters[j]->GetPosition());
                if (dist < critters[i]->GetRadius() + critters[j]->GetRadius())
                {
                    // Collision!
                    // Do math to get critters bouncing
                    Vector2 normal = Vector2Normalize(Vector2Subtract(critters[j]->GetPosition(), critters[i]->GetPosition()));

                    // Not even close to real physics, but fine for our needs
                    critters[i]->SetVelocity(Vector2Scale(normal, -MAX_VELOCITY));
                    // Set the critter to *dirty* so we know not to process any more collisions on it
                    critters[i]->SetDirty();

                    // We still want to check for collisions in the case where 1 critter is dirty - so we need a check
                    // to make sure the other critter is clean before we do the collision response
                    if (!critters[j]->IsDirty()) {
                        critters[j]->SetVelocity(Vector2Scale(normal, MAX_VELOCITY));
                        critters[j]->SetDirty();
                    }
                    break;
                }
            }
        }

        timer -= delta;
        if (timer <= 0)
        {
            timer = 1;

            // Find any dead critters and spit them out (respawn)
            for (int i = 0; i < CRITTER_COUNT; i++)
            {
                if (critters[i]->IsDead())
                {
                    Vector2 normal = Vector2Normalize(destroyer.GetVelocity());

                    // Get a position behind the destroyer, and far enough away that the critter won't bump into it again
                    Vector2 pos = destroyer.GetPosition();
                    pos = Vector2Add(pos, Vector2Scale(normal, -50));
                    // It's pretty inefficient to keep reloading textures. ...if only there was something else we could do
                    critters[i]->Reset(pos, Vector2Scale(normal, -MAX_VELOCITY), 12, critterTexture);
                    break;
                }
            }
            nextSpawnPos = destroyer.GetPosition();
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        // Draw the critters
        for (int i = 0; i < CRITTER_COUNT; i++)
        {
            critters[i]->Draw();
        }
        // Draw the destroyer
        destroyer.Draw();

        // Display frame time and FPS
        /*DrawText(FormatText("Frame Time: %.3f ms", delta * 1000.0f), 10, 30, 20, DARKGRAY);
        DrawText(FormatText("FPS: %d", fps), 10, 50, 20, DARKGRAY);*/
        DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    for (int i = 0; i < CRITTER_COUNT; i++)
    {
        critters[i]->Destroy();
    }

    textureManager.UnloadAllTextures();

    // Close log file
    /*logFile.close();*/

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}