#include "MainMenu.h"
#include "Util.h"
#define MAINMENU_WIDTH 11
#define MAINMENU_HEIGHT 8
#define MAINMENU_ENEMYCOUNT 1
unsigned int MainMenu_data[] =
{
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,  
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,  
3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,  
3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3,  
};

void MainMenu::Initialize() {
	state.sceneType = MAINMENU;
	state.nextScene = -1;
	GLuint mapTextureID = Util::LoadTexture("tileset.png");
	state.map = new Map(MAINMENU_WIDTH, MAINMENU_HEIGHT, MainMenu_data, mapTextureID, 1.0f, 4, 1);
	// Move over all of the player and enemy code from initialization.

    // Initialize Game Objects
	state.player = new Entity();
	state.player->isActive = false;

	state.text = new Entity();
	GLuint textTextureID = Util::LoadTexture("font.png");
	state.text->textureID = textTextureID;
	state.EndOfGame = false;


}
void MainMenu::Update(float deltaTime) {
	state.player->Update(deltaTime, state.player, state.enemies, MAINMENU_ENEMYCOUNT, state.map);

}
void MainMenu::Render(ShaderProgram* program) {
	state.map->Render(program);
	state.player->Render(program);
}