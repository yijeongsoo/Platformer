#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Entity.h"
#include "Map.h"
#include "Util.h"
#include "Scene.h"
#include "MainMenu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene* currentScene;
Scene* sceneList[4];

Mix_Music* music;
Mix_Chunk* bounce;

void SwitchToScene(Scene* scene, int remainLife) {
    currentScene = scene;
    currentScene->Initialize();
    if (currentScene->state.sceneType != MAINMENU) {
        currentScene->state.player->life = remainLife;
    }
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4996);
    music = Mix_LoadMUS("dooblydoo.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 3);
    bounce = Mix_LoadWAV("bounce.wav");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
    sceneList[0] = new MainMenu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    SwitchToScene(sceneList[2], 3);


}

void ProcessInput() {
   
    currentScene->state.player->movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player right
                        break;
                        
                    case SDLK_SPACE:
                        if ((currentScene->state.sceneType) != MAINMENU && currentScene->state.EndOfGame != true) {
                            if (currentScene->state.player->collidedBottom) {
                                currentScene->state.player->jump = true;
                                Mix_PlayChannel(-1, bounce, 0);
                            }
                            break;
                        }

                    case SDLK_RETURN:
                        if (currentScene->state.sceneType == MAINMENU) {
                            currentScene->state.nextScene = 1;
                        }
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (currentScene->state.EndOfGame != true) {
        if (keys[SDL_SCANCODE_LEFT]) {
            currentScene->state.player->movement.x = -1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animLeft;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            currentScene->state.player->movement.x = 1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animRight;
        }
    }

    if (glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
    }


}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime

        currentScene->Update(FIXED_TIMESTEP);       
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
    if (currentScene->state.EndOfGame != true) {
        viewMatrix = glm::mat4(1.0f);
        if (currentScene->state.player->position.x > 5) {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
        }
        else {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
        } // Don't bother with the y,z for now
    }
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    program.SetViewMatrix(viewMatrix);
    currentScene->Render(&program);

    if (currentScene->state.sceneType == MAINMENU) {
        Util::DrawText(&program, currentScene->state.text->textureID, "The Strange Journey", 0.7f, -0.25f, glm::vec3(1.0, -2.0, 0));
        Util::DrawText(&program, currentScene->state.text->textureID, "Press Enter to Start", 0.5f, -0.25f, glm::vec3(2.0, -3.0, 0));
    }
    if (currentScene->state.sceneType == LEVEL1 && currentScene->state.player->position.x <= 20.0f) {
        Util::DrawText(&program, currentScene->state.text->textureID, "Level 1", 0.7f, -0.25f, glm::vec3(4.0, -2.0, 0));
        Util::DrawText(&program, currentScene->state.text->textureID, "Press Space Bar to Jump", 0.5f, -0.25f, glm::vec3(5.0, -4.0, 0));
    }
    if (currentScene->state.sceneType == LEVEL2 && currentScene->state.player->position.x <= 20.0f) {
        Util::DrawText(&program, currentScene->state.text->textureID, "Level 2", 0.7f, -0.25f, glm::vec3(4.0, -2.0, 0));
        Util::DrawText(&program, currentScene->state.text->textureID, "Avoid Touching the Ghosts!", 0.5f, -0.25f, glm::vec3(5.0, -4.0, 0));
    }
    if (currentScene->state.sceneType == LEVEL3 && currentScene->state.player->position.x <= 20.0f) {
        Util::DrawText(&program, currentScene->state.text->textureID, "Level 3", 0.7f, -0.25f, glm::vec3(4.0, -2.0, 0));
        Util::DrawText(&program, currentScene->state.text->textureID, "Get to the End before the Boss gets You!", 0.5f, -0.25f, glm::vec3(4.0, -4.0, 0));
    }
    if (currentScene->state.sceneType == LEVEL3 && currentScene->state.EndOfGame == true && currentScene->state.player->life != 0) {
        Util::DrawText(&program, currentScene->state.text->textureID, "Game Completed!", 0.7f, -0.25f, glm::vec3(31.0, -2.0, 0));
        Util::DrawText(&program, currentScene->state.text->textureID, "You Win!", 0.7f, -0.25f, glm::vec3(32.0, -3.0, 0));
    }

    if (currentScene->state.sceneType != MAINMENU) {
        if (currentScene->state.player->life == 3 && currentScene->state.EndOfGame != true) {
            Util::DrawText(&program, currentScene->state.text->textureID, "Remaining Lives : 2", 0.4f, -0.2f, glm::vec3((currentScene->state.player->position.x) + 1.0, -1.0, 0));
        }
        else if (currentScene->state.player->life == 2 && currentScene->state.EndOfGame != true) {
            Util::DrawText(&program, currentScene->state.text->textureID, "Remaining Lives : 1", 0.4f, -0.2f, glm::vec3((currentScene->state.player->position.x) + 1.0, -1.0, 0));
        }
        else if (currentScene->state.player->life == 1 && currentScene->state.EndOfGame != true) {
            Util::DrawText(&program, currentScene->state.text->textureID, "Remaining Lives : 0", 0.4f, -0.2f, glm::vec3((currentScene->state.player->position.x) + 1.0, -1.0, 0));
        }
        else if (currentScene->state.player->life == 0) {
            Util::DrawText(&program, currentScene->state.text->textureID, "Game Over", 0.7f, -0.25f, glm::vec3(currentScene->state.player->position.x, -3.0, 0));
        }
        
    }
    
    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        if (currentScene->state.nextScene >= 0) SwitchToScene(sceneList[currentScene->state.nextScene], currentScene->state.player->life);
        Render();
    }
    
    Shutdown();
    return 0;
}
