#include "raylib.h"
#include <stdio.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif


int screenWidth = 800;
int screenHeight = 450;

int MAX_VELOCITY = 10;

int PLAYER_RECT_W = 8;
int PLAYER_RECT_H = 80;

typedef struct {
    Vector2 pos;
    Vector2 vel;
    Vector2 acc;
} Ball;

typedef struct {
    Vector2 pos;
} Player;

typedef struct {
    Ball *ball;
    Player *p1;
    Player *p2;
} Game;

int sign(float n) {
    if (n < 0) {
        return -1;
    }
    return 1;
}

float absolute(float n) {
    if (n < 0) {
        return -n;
    }
    return n;
}

void UpdateDrawFrame(Game *game);

Ball NewBall() {
    Vector2 pos = {
        .x = screenWidth * 0.5,
        .y = screenHeight * 0.5 
    };
    Vector2 vel = {
        .x = 5,
        .y = 0 
    };
    Vector2 acc = {
        .x = 6,
        .y = 0 
    };
    Ball ball = { pos, vel, acc };
    return ball;
}

Player NewPlayer(float x, float y) {
    Vector2 pos = { x, y };
    Player p = { pos };
    return p;
}

void UpdateBall(Ball *ball) {
    if (ball->pos.x < 0 || ball->pos.x > screenWidth) {
        printf("OUT OF BOUNDS IN X\n");
        ball->acc.x *= -sign(ball->pos.x * ball->acc.x);
        printf("acc x: %.3f\n", ball->acc.x);
    }

    // out of bounds in y
    if (ball->pos.y < 0 || ball->pos.y > screenHeight) {
        printf("OUT OF BOUNDS IN Y\n");
        ball->acc.y *= -sign(ball->pos.y * ball->acc.y);
        ball->vel.y *= -sign(ball->pos.y * ball->acc.y);
        printf("acc y: %.3f\n", ball->acc.y);
    }

    // update velocity
    ball->vel.x += ball->acc.x;
    ball->vel.y += ball->acc.y;
    if (absolute(ball->vel.x) > MAX_VELOCITY) {
        ball->vel.x = MAX_VELOCITY*sign(ball->vel.x);
    }
    if (absolute(ball->vel.y) > MAX_VELOCITY) {
        ball->vel.y = MAX_VELOCITY*sign(ball->vel.y);
    }

    // update position
    ball->pos.x += ball->vel.x;
    ball->pos.y += ball->vel.y;
}

int main() {
    InitWindow(screenWidth, screenHeight, "P o n g");

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);

    Ball ball = NewBall();
    Player p1 = NewPlayer(10, screenHeight * 0.5);
    Player p2 = NewPlayer(screenWidth - 10, screenHeight * 0.5);
    Game game = {
        &ball,
        &p1,
        &p2,
    };

    // Main game loop
    while (!WindowShouldClose()) {
        UpdateDrawFrame(&game);
    }
#endif

    CloseWindow();       

    return 0;
}

void UpdateDrawFrame(Game *game) {
    // update
    UpdateBall(game->ball);
    printf("ball (x = %.3f, y = %.3f)\n", game->ball->pos.x, game->ball->pos.y);

    // draw
    BeginDrawing();

        ClearBackground(DARKGRAY);
        DrawText("First game", GetFontDefault().baseSize, screenHeight/32, 20, LIGHTGRAY);
        DrawLine(screenWidth/2, screenHeight, screenWidth/2, 0, DARKGREEN);
        DrawCircle(game->ball->pos.x, game->ball->pos.y, 8, LIGHTGRAY);
        DrawRectangle(game->p1->pos.x, game->p1->pos.y - PLAYER_RECT_H*0.5, PLAYER_RECT_W, PLAYER_RECT_H, LIGHTGRAY);
        DrawRectangle(game->p2->pos.x, game->p2->pos.y - PLAYER_RECT_H*0.5, PLAYER_RECT_W, PLAYER_RECT_H, LIGHTGRAY);

    EndDrawing();
}
