/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2013-2024 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include <raymath.h>
#include <stdint.h>
#include <stdbool.h>

#define NUM_BIRDS 200
#define BIRD_SIZE 10
#define VICINITY_DIST (BIRD_SIZE*20)
#define TOO_CLOSE_DIST (BIRD_SIZE*3/2)
#define REPULSION_FACTOR 0.1f
#define MASS_ATTRACTION_FACTOR 0.001f
#define ALIGNMENT_FACTOR 0.05f
#define TURN_SPEED 0.1f

#define SCREEN_MARGIN 100

typedef struct {
    Vector2 p1;
    Vector2 p2;
    Vector2 p3;
} Triangle;

typedef struct {
    Vector2 pos[NUM_BIRDS];
    Vector2 dir[NUM_BIRDS];
    Triangle triangle[NUM_BIRDS];
} Birds;

void GetTriangle(Triangle *t, Vector2 pos, Vector2 dir) {
    static int height = 5*BIRD_SIZE/2;
    static int half_width = BIRD_SIZE;

    Vector2 unit_dir = Vector2Scale(dir, 1.0f/Vector2Length(dir));
    Vector2 dir_scaled = Vector2Scale(unit_dir, height);
    Vector2 perp_vector = { unit_dir.y, -unit_dir.x };
    Vector2 perp_vector_scaled = Vector2Scale(perp_vector, half_width);

    t->p1 = Vector2Add(pos, dir_scaled);
    t->p2 = Vector2Add(pos, perp_vector_scaled);
    t->p3 = Vector2Subtract(pos, perp_vector_scaled);
}

void StepPosition(Birds *birds) {
    static float step_amount = 2.0f;
    for (int i = 0; i < NUM_BIRDS; i++) {
        Vector2 step = Vector2Scale(birds->dir[i], step_amount);
        birds->pos[i] = Vector2Add(birds->pos[i], step);
        GetTriangle(birds->triangle+i, birds->pos[i], birds->dir[i]);
    }
}


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1440;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    
    Birds birds;
    for (int i = 0; i < NUM_BIRDS; i++) {
        birds.pos[i] = (Vector2) {
            (float) GetRandomValue(50, screenWidth-50),
            (float) GetRandomValue(50, screenHeight-50),
        };
        int dir_angle = GetRandomValue(0, 359);
        birds.dir[i] = Vector2Rotate( (Vector2){ 1, 0} , dir_angle);
        GetTriangle(birds.triangle+i, birds.pos[i], birds.dir[i]);
    }

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // for each bird find the birds that are within 100 units of it. create a matrix for it.

        // records the number of birds that are close to the bird in each corresponding index.
        int num_vicinity[NUM_BIRDS] = {0};
        int num_too_close[NUM_BIRDS] = {0};

        Vector2 mass_centre[NUM_BIRDS] = {0};
        Vector2 repulsion[NUM_BIRDS] = {0};
        Vector2 dir_mean[NUM_BIRDS] = {0};
        
        for (int i = 0; i < NUM_BIRDS; i++) {
            Vector2 cur_bird_pos = birds.pos[i];
            for (int j = 0; (j < NUM_BIRDS) && i != j; j++) {
                Vector2 other_bird_pos = birds.pos[j];
                Vector2 other_bird_dir = birds.dir[j];
                bool is_vicinity = CheckCollisionPointCircle(other_bird_pos, cur_bird_pos, VICINITY_DIST);
                if (is_vicinity) {
                    num_vicinity[i] += 1;
                    mass_centre[i] = Vector2Add(mass_centre[i], other_bird_pos);
                    dir_mean[i] = Vector2Add(dir_mean[i], other_bird_dir);
                }
                bool is_too_close = CheckCollisionPointCircle(other_bird_pos, cur_bird_pos, TOO_CLOSE_DIST);
                if (is_too_close) {
                    num_too_close[i] += 1;
                    Vector2 repulse_dir = Vector2Subtract(cur_bird_pos, other_bird_pos);
                    repulsion[i] = Vector2Add(repulsion[i], repulse_dir);
                }
            }

            Vector2 mass_centre_dir = {0};
            if (num_vicinity[i] > 0) {
                mass_centre[i] = Vector2Scale(mass_centre[i], 1.0f/num_vicinity[i]);
                mass_centre_dir = Vector2Subtract(mass_centre[i], cur_bird_pos);
                mass_centre_dir = Vector2Scale(mass_centre_dir, MASS_ATTRACTION_FACTOR);

                dir_mean[i] = Vector2Scale(dir_mean[i], ALIGNMENT_FACTOR/num_vicinity[i]);
            }
            if (num_too_close[i] > 0) {
                repulsion[i] = Vector2Scale(repulsion[i], REPULSION_FACTOR/num_too_close[i]);
            }

            birds.dir[i] = Vector2Add(birds.dir[i], Vector2Add(mass_centre_dir, Vector2Add(dir_mean[i], repulsion[i])));
            if (birds.pos[i].x < SCREEN_MARGIN) {
                birds.dir[i].x += TURN_SPEED;
            }
            if (birds.pos[i].x > screenWidth - SCREEN_MARGIN) {
                birds.dir[i].x -= TURN_SPEED;
            }
            if (birds.pos[i].y < SCREEN_MARGIN) {
                birds.dir[i].y += TURN_SPEED;
            }
            if (birds.pos[i].y > screenHeight - SCREEN_MARGIN) {
                birds.dir[i].y -= TURN_SPEED;
            }
            birds.dir[i] = Vector2ClampValue(birds.dir[i], 1, 3);
        }
        
        StepPosition(&birds);
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            for (int i = 0; i < NUM_BIRDS; i++) {
                DrawTriangle(birds.triangle[i].p1, birds.triangle[i].p2, birds.triangle[i].p3, LIGHTGRAY);
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
