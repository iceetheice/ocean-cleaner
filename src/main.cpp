#include "raylib.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <string>

std::string gameState = "play";
bool isPlayerDead = false;
int hearts = 3;
float enemysSpeed = 60;
float bulletsTimer = 1;

typedef struct Entity
{
    float x;
    float y;
    float speed;
    float width;
    float height;
    Rectangle rect;
} Entity;

float spawnEnemysTimer = 0;

void updatePlayer(Entity &entity);
void drawPlayer(Entity player, Texture2D playerSprite);
void spawnEnemys(std::vector<Entity> &enemys);
void updateEnemys(std::vector<Entity> &enemys, Entity player);
void drawEnemys(std::vector<Entity> &enemys);
void spawnBullet(std::vector<Entity> &bullets, Entity player);
void updateBullets(std::vector<Entity> &bullets, std::vector<Entity> &enemys);
void drawBullets(std::vector<Entity> &bullets);

void updateMenu(Entity button, Entity player);
void drawMenu(Entity button, Texture2D ButtonSprites[]);
void lose(std::vector<Entity> &enemys, std::vector<Entity> &bullets);

int main()
{
    InitWindow(600, 600, "Ocean");

    // load
    Entity player;
    player.x = 280;
    player.y = 500;
    player.width = 50;
    player.height = 70;
    player.speed = 1500;
    Texture2D playerSprite = LoadTexture("../assets/player/ship1.png");

    std::vector<Entity> enemys;
    std::vector<Entity> bullets;

    Entity button;
    button.x = 250;
    button.y = 400;
    button.width = 120;
    button.height = 60;
    button.rect = {button.x, button.y, button.width, button.height};
    Texture2D buttonSprites[2] = {LoadTexture("../assets/ui/button.png")};

    while (!WindowShouldClose())
    {
        // update
        if (gameState == "play")
        {
            updatePlayer(player);
            if (spawnEnemysTimer < 0)
            {
                spawnEnemys(enemys);
                spawnEnemysTimer = 2.5;
            }
            else
            {
                spawnEnemysTimer -= 1 * GetFrameTime();
            }
            updateEnemys(enemys, player);

            // bullets logic
             updateBullets(bullets, enemys);
            
            if (bulletsTimer < 0)
            {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    spawnBullet(bullets, player);
                    bulletsTimer = 1;
                }
            }
            else
            {
                bulletsTimer -= 1 * GetFrameTime();
                std::cout << bulletsTimer << '\n';
            }

            //lose logic
            if (isPlayerDead)
            {
                lose(enemys, bullets);
            }
            enemysSpeed += 3 * GetFrameTime();
        }
        else if (gameState == "start")
        {
            updateMenu(button, player);
        }

        // draw
        BeginDrawing();
        ClearBackground(SKYBLUE);
        if (gameState == "play")
        {
            drawPlayer(player, playerSprite);
            drawEnemys(enemys);
            drawBullets(bullets);
            DrawText(std::to_string(hearts).c_str(), 10, 20, 30, RED);
        }
        else if (gameState == "start")
        {
            drawMenu(button, buttonSprites);
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void updatePlayer(Entity &player)
{
    player.rect = {player.x, player.y, player.width, player.height};
    Vector2 mousePos = GetMousePosition();
    
    float distance;

    if (CheckCollisionPointRec(mousePos, player.rect))
    {
        isPlayerDead = true;
    }
    if (hearts < 1)
    {
        isPlayerDead = true;
    }

    if (mousePos.y < player.y + player.height && mousePos.y > player.y)
    {
        if (mousePos.x > player.x + player.width/2 && player.x > 0 + player.width + 20)
        {
            distance = mousePos.x - player.x;
            player.x -= player.speed / (distance/6) * GetFrameTime();
        }
        else if (player.x < 600 - player.width - 20)
        {
            distance = abs(mousePos.x - player.x);
            player.x += player.speed / (distance) * GetFrameTime();
        }
    }
}

void drawPlayer(Entity player, Texture2D playerSprite)
{
    DrawRectangle(player.x, player.y, player.width, player.height,  RED);
    DrawTexture(playerSprite, player.x, player.y, WHITE);
}

void spawnEnemys(std::vector<Entity> &enemys)
{
    Entity enemy;
    enemy.x = rand() % ((600 - 40-20) - (0 + 40+20) + 1) + (0 + 40+20);
    enemy.y = -40;
    enemy.speed = enemysSpeed;
    enemy.width = 40;
    enemy.height = 40;
    enemy.rect = {enemy.x, enemy.y, enemy.width, enemy.height};
    enemys.push_back(enemy);
}

void updateEnemys(std::vector<Entity> &enemys, Entity player)
{
    for (int i = 0; i < enemys.size(); i++)
    {
        enemys[i].y += enemys[i].speed * GetFrameTime();
        enemys[i].rect = {enemys[i].x, enemys[i].y, enemys[i].width, enemys[i].height};
        if (enemys[i].y > 600+enemys[i].height)
        {
            enemys.erase(enemys.begin() + i);
            hearts--;
        }

        if (CheckCollisionRecs(enemys[i].rect, player.rect))
        {
            isPlayerDead = true;
        }
    } 
}

void drawEnemys(std::vector<Entity> &enemys)
{
    for (auto &enemy : enemys)
    {
        DrawRectangle(enemy.x, enemy.y, enemy.width, enemy.height, BLUE);
    } 
}

void spawnBullet(std::vector<Entity> &bullets, Entity player)
{
    Entity bullet;
    bullet.x = player.x + 10;
    bullet.y = player.y - 10;
    bullet.speed = 150;
    bullet.width = 20;
    bullet.height = 20;
    bullet.rect = {bullet.x, bullet.y, bullet.width, bullet.height};
    bullets.push_back(bullet);
}

void updateBullets(std::vector<Entity> &bullets, std::vector<Entity> &enemys)
{
    for (int i = 0; i < bullets.size(); i++)
    {
        bullets[i].y -= bullets[i].speed * GetFrameTime();
        bullets[i].rect = {bullets[i].x, bullets[i].y, bullets[i].width, bullets[i].height};
        if (bullets[i].y < -50)
        {
            bullets.erase(bullets.begin() + i);
            continue;
        }

        for (int j = 0; j < enemys.size(); j++)
        {
            if (CheckCollisionRecs(bullets[i].rect, enemys[j].rect))
            {
                enemys.erase(enemys.begin() + j);
                bullets.erase(bullets.begin() + i);
                break;
            }
        }
    }
}

void drawBullets(std::vector<Entity> &bullets)
{
    for (auto &bullet : bullets)
    {
        DrawRectangle(bullet.x, bullet.y, bullet.width, bullet.height, BLACK);
    }
}

void updateMenu(Entity button, Entity player)
{
    if (CheckCollisionPointRec(GetMousePosition(), button.rect) && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        gameState = "play";
        isPlayerDead = false;
        spawnEnemysTimer = 0;
        hearts = 3;
        player.x = 280;
        enemysSpeed = 60;
    }
}

void drawMenu(Entity button, Texture2D ButtonSprites[])
{
    DrawTexture(ButtonSprites[0], button.x, button.y, WHITE);
}

void lose(std::vector<Entity> &enemys, std::vector<Entity> &bullets)
{
    gameState = "start";

    enemys.clear();
    bullets.clear();
}