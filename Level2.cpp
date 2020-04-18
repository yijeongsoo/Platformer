#include "Level2.h"
#include "Util.h"
#define LEVEL2_WIDTH 36
#define LEVEL2_HEIGHT 8
#define LEVEL2_ENEMYCOUNT 3
unsigned int level2_data[] =
{
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 3,
3, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 3, 3, 0, 3,
3, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 3,
3, 1, 1, 1, 1, 1, 2, 2, 2, 0, 0, 0, 0, 0, 1, 2, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 3,
3, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 3,
3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 0, 0, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 3,
3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3,
};

void Level2::Initialize() {
    state.sceneType = LEVEL2;
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 4, 1);
    // Move over all of the player and enemy code from initialization.
    state.EndOfGame = false;

    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(5, 0, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81, 0); //Setting Acceleration to Gravity
    state.player->speed = 1.75f;
    state.player->textureID = Util::LoadTexture("george_0.png");

    state.player->animRight = new int[4]{ 3, 7, 11, 15 };
    state.player->animLeft = new int[4]{ 1, 5, 9, 13 };
    state.player->animUp = new int[4]{ 2, 6, 10, 14 };
    state.player->animDown = new int[4]{ 0, 4, 8, 12 };

    state.player->height = 0.8f;
    state.player->width = 0.6f;

    state.player->jumpPower = 6.5f;

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;

    state.enemies = new Entity[LEVEL2_ENEMYCOUNT];
    GLuint enemyTextureID = Util::LoadTexture("enemies.png");

    for (int i = 0; i < LEVEL2_ENEMYCOUNT; i++) {
        state.enemies[i].animIndices = new int[3]{ 1, 2, 3 };
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].acceleration = glm::vec3(0, -9.81, 0);
        state.enemies[i].aiType = ENEMYAI;
        state.enemies[i].aiState = WALKING;
        state.enemies[i].speed = 2.0f;
        state.underlings[i].animFrames = 3;
        state.underlings[i].animIndex = 0;
        state.underlings[i].animTime = 0;
        state.underlings[i].animCols = 12;
        state.underlings[i].animRows = 8;
        state.underlings[i].width = 0.8f;
    }

    
    state.enemies[0].position = glm::vec3(12.0, -5.0, 0);
    //state.enemies[0].isActive = false;

    state.enemies[1].position = glm::vec3(24.5, -5.0, 0);
    //state.enemies[1].isActive = false;

    state.enemies[2].position = glm::vec3(27.5, -5.0, 0);
    //state.enemies[2].isActive = false;


    state.text = new Entity();
    GLuint textTextureID = Util::LoadTexture("font.png");
    state.text->textureID = textTextureID;
}
void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMYCOUNT, state.map);
    for (int i = 0; i < LEVEL2_ENEMYCOUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMYCOUNT, state.map);
    }
    if (state.player->position.x >= 32) {
        state.nextScene = 3;
    }
    if (state.player->life == 0) {
        state.EndOfGame = true;
    }

}
void Level2::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.player->Render(program);
    for (int i = 0; i < LEVEL2_ENEMYCOUNT; i++) {
        state.enemies[i].Render(program);
    }
}