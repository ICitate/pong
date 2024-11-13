#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif


int screenWidth = 800;
int screenHeight = 450;

int MAX_VELOCITY = 10;

int PLAYER_RECT_W = 10;
int PLAYER_RECT_H = 80;

typedef struct {
    Vector2 pos;
    Vector2 vel;
    float rad;
} Ball;

typedef struct {
    Rectangle rec;
    int score;
} Player;

typedef struct {
    Ball *ball;
    Player *p1;
    Player *p2;
    int ended;
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
        .x = 6,
        .y = -7.5
    };
    float rad = 8;
    Ball ball = { pos, vel, rad };
    return ball;
}

Player NewPlayer(float x, float y) {
    Rectangle rec = { x, y, PLAYER_RECT_W, PLAYER_RECT_H };
    Player p = { rec };
    return p;
}

void ResetBall(Ball *ball, int direction) {
    // reset ball
    ball->pos.x = screenWidth * 0.5;
    ball->pos.y = screenHeight * 0.5;
    ball->vel.x = 6 * direction;
    ball->vel.y = 7.5 * direction;
}

void UpdateBall(Ball *ball, Player *p1, Player *p2) {

    { // out of bounds in x
        if (ball->pos.x < 0) {
            p2->score += 1;
            ResetBall(ball, 1);
            return;
        } 
        if (ball->pos.x > screenWidth) {
            p1->score += 1;
            ResetBall(ball, -1);
            return;
        }
    }

    { // update velocity
        // check collision in Y
        if (ball->pos.y < 0 || ball->pos.y > screenHeight) {
            ball->vel.y *= -1;
        }

        if (CheckCollisionCircleRec(ball->pos, ball->rad, p1->rec) || 
            CheckCollisionCircleRec(ball->pos, ball->rad, p2->rec)) {
            if (ball->pos.x < p1->rec.x + p1->rec.width || ball->pos.x > p2->rec.x) {
                ball->vel.y *= -1;
            }

            ball->vel.x *= -1;
        }

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
    Player p1 = NewPlayer(PLAYER_RECT_W * 2, screenHeight * 0.5 - PLAYER_RECT_H * 0.5);
    Player p2 = NewPlayer(screenWidth - PLAYER_RECT_W * 3, screenHeight * 0.5 - PLAYER_RECT_H * 0.5);
    Game game = {
        &ball,
        &p1,
        &p2,
        0,
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
    if (game->ended == 0) {
        if (IsKeyDown(KEY_S)) {
            if (game->p1->rec.y + 5 + game->p1->rec.height < screenHeight) {
                game->p1->rec.y += 5;
            }
        }
        if (IsKeyDown(KEY_W)) {
            if (game->p1->rec.y - 5 > 0) {
                game->p1->rec.y -= 5;
            }
        }

        if (IsKeyDown(KEY_DOWN)) {
            if (game->p2->rec.y + 5 + game->p2->rec.height < screenHeight) {
                game->p2->rec.y += 5;
            }
        }
        if (IsKeyDown(KEY_UP)) {
            if (game->p2->rec.y - 5 > 0) {
                game->p2->rec.y -= 5;
            }
        }

        UpdateBall(game->ball, game->p1, game->p2);
    }

    if (IsKeyDown(KEY_ENTER)) {
        game->ended = 0;
        game->ball->pos.x = screenWidth * 0.5;
        game->ball->pos.y = screenHeight * 0.5;
        game->ball->vel.x = 6;
        game->ball->vel.y = -7.5;
        game->p1->rec.x = PLAYER_RECT_W * 2;
        game->p1->rec.y = screenHeight * 0.5 - PLAYER_RECT_H * 0.5;
        game->p1->score = 0;
        game->p2->rec.x = screenWidth - PLAYER_RECT_W * 3;
        game->p2->rec.y = screenHeight * 0.5 - PLAYER_RECT_H * 0.5;
        game->p2->score = 0;
    }

    // draw
    BeginDrawing();
        ClearBackground(BLACK);

        DrawText(TextFormat("%d | %d", game->p1->score, game->p2->score), screenWidth/2 - 42, 10, 40, RAYWHITE);
        if (game->ended == 0) {
            DrawCircle(game->ball->pos.x, game->ball->pos.y, game->ball->rad, WHITE);
        }
        DrawRectangle(game->p1->rec.x, game->p1->rec.y, game->p1->rec.width, game->p1->rec.height, LIGHTGRAY);
        DrawRectangle(game->p2->rec.x, game->p2->rec.y, game->p2->rec.width, game->p2->rec.height, LIGHTGRAY);

        if (game->ended == 1) {
            DrawText("Press enter to play again", 0.5*(screenWidth-MeasureText("Player 1 Wins!", 20)), screenHeight-30, 20, RAYWHITE);
        }

        if (game->p1->score >= 10) {
            DrawText("Player 1 Wins!", 0.5*(screenWidth-MeasureText("Player 1 Wins!", 50)), 0.5*(screenHeight-50), 50, BLUE);
            game->ended = 1;
        }
        if (game->p2->score >= 10) {
            DrawText("Player 2 Wins!", 0.5*(screenWidth-MeasureText("Player 2 Wins!", 50)), 0.5*(screenHeight-50), 50, BLUE);
            game->ended = 1;
        }

    EndDrawing();
}
