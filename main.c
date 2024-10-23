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
    float rad;
    int isColliding;
} Ball;

typedef struct {
    Rectangle rec;
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
    float rad = 8;
    int isColliding = 0;
    Ball ball = { pos, vel, acc, rad, isColliding };
    return ball;
}

Player NewPlayer(float x, float y) {
    Rectangle rec = { x, y, PLAYER_RECT_W, PLAYER_RECT_H };
    Player p = { rec };
    return p;
}

void UpdateBall(Ball *ball, Player *p1, Player *p2) {
    ball->isColliding = 0;

    { // update acceleration
        // check collision in Y
        if (ball->pos.y < 0 || ball->pos.y > screenHeight) {
            //printf("OUT OF BOUNDS IN Y\n");
            ball->acc.y *= -sign(ball->pos.y * ball->acc.y);
            printf("acc y: %.3f\n", ball->acc.y);
        }

        if (CheckCollisionCircleRec(ball->pos, ball->rad, p1->rec)) {
            printf("COLLIDED\n");
            ball->acc.x = 8;
            printf("acc x: %.3f\n", ball->acc.x);
            ball->isColliding = 1;
        }

        if (CheckCollisionCircleRec(ball->pos, ball->rad, p2->rec)) {
            printf("COLLIDED\n");
            ball->acc.x = -8;
            printf("acc x: %.3f\n", ball->acc.x);
            ball->isColliding = 1;
        }
    }

    { // update velocity
        ball->vel.x += ball->acc.x;
        ball->vel.y += ball->acc.y;
        if (absolute(ball->vel.x) > MAX_VELOCITY) {
            ball->vel.x = MAX_VELOCITY*sign(ball->vel.x);
        }
        if (absolute(ball->vel.y) > MAX_VELOCITY) {
            ball->vel.y = MAX_VELOCITY*sign(ball->vel.y);
        }
    }

    { // update position
        ball->pos.x += ball->vel.x;
        ball->pos.y += ball->vel.y;
    }
}

int main() {
    InitWindow(screenWidth, screenHeight, "P o n g");

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);

    Ball ball = NewBall();
    Player p1 = NewPlayer(PLAYER_RECT_W * 2, screenHeight * 0.5);
    Player p2 = NewPlayer(screenWidth - PLAYER_RECT_W * 3, screenHeight * 0.5);
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
    UpdateBall(game->ball, game->p1, game->p2);
    printf("ball (x = %.3f, y = %.3f)\n", game->ball->pos.x, game->ball->pos.y);

    // draw
    BeginDrawing();

        ClearBackground(DARKGRAY);
        DrawText("First game", GetFontDefault().baseSize, screenHeight/32, 20, LIGHTGRAY);
        DrawLine(screenWidth/2, screenHeight, screenWidth/2, 0, DARKGREEN);
        DrawLine(screenWidth, screenHeight, screenWidth, 0, DARKGREEN);
        DrawLine(1, screenHeight, 1, 1, DARKGREEN);
        DrawCircle(game->ball->pos.x, game->ball->pos.y, game->ball->rad, game->ball->isColliding ? RED : LIGHTGRAY);
        DrawLine(game->p1->rec.x, 0, game->p1->rec.x, screenHeight, DARKBLUE);
        DrawLine(game->p2->rec.x + game->p2->rec.width, 0, game->p2->rec.x + game->p2->rec.width, screenHeight, DARKBLUE);
        DrawRectangle(game->p1->rec.x, game->p1->rec.y - game->p1->rec.height*0.5, game->p1->rec.width, game->p1->rec.height, LIGHTGRAY);
        DrawRectangle(game->p2->rec.x, game->p2->rec.y - game->p2->rec.height*0.5, game->p2->rec.width, game->p2->rec.height, LIGHTGRAY);

    EndDrawing();
}
