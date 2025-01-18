#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "raylib.h"

typedef struct {
    // Synced
    float player1Y;
    float player2Y;
    float ballX;
    float ballY;
    float ballVelocityX;
    float ballVelocityY;
    int player1Score;
    int player2Score;

    // Unsynced
    int turn;  // 0: none, 1: player1, 2: player2
    bool hostMode;

} GameState;

#define PADDLE_HEIGHT 100
#define PADDLE_WIDTH 20
#define BALL_RADIUS 10
#define BALL_INITIAL_VELOCITY 300

#define PADDLE_SPEED 400

#define BALL_VELOCITY_GAIN 1.1

void Update(GameState* gs) {
    // player 1
    if (IsKeyDown(KEY_W)) {
        gs->player1Y -= PADDLE_SPEED * GetFrameTime();
    } else if (IsKeyDown(KEY_S)) {
        gs->player1Y += PADDLE_SPEED * GetFrameTime();
    }

    // bounds
    if (gs->player1Y < 0) {
        gs->player1Y = 0;
    } else if (gs->player1Y > 450 - PADDLE_HEIGHT) {
        gs->player1Y = 450 - PADDLE_HEIGHT;
    }

    // player 2
    if (IsKeyDown(KEY_I)) {
        gs->player2Y -= PADDLE_SPEED * GetFrameTime();
    } else if (IsKeyDown(KEY_J)) {
        gs->player2Y += PADDLE_SPEED * GetFrameTime();
    }

    // bounds
    if (gs->player2Y < 0) {
        gs->player2Y = 0;
    } else if (gs->player2Y > 450 - PADDLE_HEIGHT) {
        gs->player2Y = 450 - PADDLE_HEIGHT;
    }

    // ball movement
    gs->ballX += gs->ballVelocityX * GetFrameTime();
    gs->ballY += gs->ballVelocityY * GetFrameTime();

    // ball bounds
    if (gs->ballY < 0 + BALL_RADIUS || gs->ballY > 450 - BALL_RADIUS) {
        gs->ballVelocityY = -gs->ballVelocityY;
    }

    // ball paddle collision
    if (gs->turn != 2 && CheckCollisionCircleRec(
                             (Vector2){gs->ballX, gs->ballY}, BALL_RADIUS,
                             (Rectangle){50, gs->player1Y, PADDLE_WIDTH, PADDLE_HEIGHT})) {
        gs->ballVelocityX = -gs->ballVelocityX;
        gs->ballVelocityX *= BALL_VELOCITY_GAIN;
        gs->turn = 2;
    }
    if (gs->turn != 1 && CheckCollisionCircleRec(
                             (Vector2){gs->ballX, gs->ballY}, BALL_RADIUS,
                             (Rectangle){800 - 50 - PADDLE_WIDTH, gs->player2Y, PADDLE_WIDTH, PADDLE_HEIGHT})) {
        gs->ballVelocityX = -gs->ballVelocityX;
        gs->ballVelocityX *= BALL_VELOCITY_GAIN;
        gs->turn = 1;
    }

    // scoring
    if (gs->ballX < 0) {
        gs->ballX = 400;
        gs->ballY = 200;
        gs->player2Score++;
        gs->turn = 0;
        gs->ballVelocityX = BALL_INITIAL_VELOCITY;
        gs->ballVelocityY = BALL_INITIAL_VELOCITY;
    } else if (gs->ballX > 800) {
        gs->ballX = 400;
        gs->ballY = 200;
        gs->player1Score++;
        gs->turn = 0;
        gs->ballVelocityX = -BALL_INITIAL_VELOCITY;
        gs->ballVelocityY = -BALL_INITIAL_VELOCITY;
    }
}

void UpdateDumb(GameState* gs) {
    // TODO:
    // 1. Get game state from the server
}

void Draw(GameState* gs) {
    DrawRectangle(50, gs->player1Y, PADDLE_WIDTH, PADDLE_HEIGHT, WHITE);
    DrawRectangle(800 - 50 - PADDLE_WIDTH, gs->player2Y, PADDLE_WIDTH, PADDLE_HEIGHT, WHITE);
    DrawCircle(gs->ballX, gs->ballY, BALL_RADIUS, WHITE);

    char* str;
    asprintf(&str, "Player 1: %d", gs->player1Score);
    DrawText(str, 100, 10, 30, WHITE);
    free(str);
    asprintf(&str, "Player 2: %d", gs->player2Score);
    DrawText(str, 500, 10, 30, WHITE);
    free(str);
}

int main(int argc, char* argv[]) {
    bool hostMode = false;
    if (argc <= 1) {
        printf("Pass either --client or --host\n");
        return 1;
    } else if (strcmp(argv[1], "--client") == 0) {
        printf("Running as Client\n");
    } else if (strcmp(argv[1], "--host") == 0) {
        printf("Running as Host\n");
        hostMode = true;
    } else {
        printf("Pass either --client or --host\n");
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }
    const char* ip = "0.0.0.0";  // TODO: replace me
    const int port = 12345;
    struct sockaddr_in con_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
    };
    // TODO: connect and send/recv data and stuff

    InitWindow(800, 450, "raylib [core] example - basic window");

    GameState gs = {
        .player1Y = 200,
        .player2Y = 200,
        .ballX = 400,
        .ballY = 200,
        .ballVelocityX = BALL_INITIAL_VELOCITY,
        .ballVelocityY = BALL_INITIAL_VELOCITY,
        .turn = 0,
        .player1Score = 0,
        .player2Score = 0,
        .hostMode = hostMode,
    };

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        if (gs.hostMode) {
            Update(&gs);
        } else {
            UpdateDumb(&gs);
        }
        Draw(&gs);

        EndDrawing();
    }

    CloseWindow();
}
