#pragma once
#include "GameHost.h"

Graphics *graphics;
Physics *physics;
Sensor *sensor;

int main() {
    auto startTime = std::chrono::steady_clock::now();
    
    game_in_play = true;
    setGameState(IN_PLAY);
    
    sensor = new Sensor(0);
    sensor->detectProjectionSize();
    
    physics = new Physics();
    
    graphics = new Graphics();
    if (!graphics->importResources(ASSET_PATH)) {
        std::cout << "ERROR: Asset(s) Missing, Check Directory" << std::endl;
        return EXIT_FAILURE;
    }
    graphics->spawnWindow(false);    // DEBUG ONLY, CHANGE TO true FOR PROD (UNLESS YOU'RE ON MAAC)
    graphics->drawStartupSplashImage();
    graphics->pushToScreen();
    
    physics->resetPuck(table_center);
    
    thread tPhysics(physicsThread);
    thread tSensor(sensorThread);
    
    setGameState(IN_PLAY);
    graphicsThread();
    
    tSensor.join();
    tPhysics.join();
    
    return EXIT_SUCCESS;
}

void physicsThread() {
    auto lastTime = std::chrono::steady_clock::now();
    auto lastTime_frameCounter = lastTime;
    int frames = 0;
    while (game_in_play) {
        //while (getGameState() != IN_PLAY) // antiquated, trying to phase out
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<long double, std::micro> deltaTime = currentTime - lastTime;
        std::chrono::duration<double, std::micro> frameCountDur = currentTime - lastTime_frameCounter;
        frames++;
        if (frameCountDur.count() >= 5e6) {
            std::cout << "Physics Framerate: " << frames / 5 << std::endl;
            frames = 0;
            lastTime_frameCounter = currentTime;
        }
        
        if(getGameState() == IN_PLAY)
            physics->tick(deltaTime.count());
        
        if (physics->detectGoals() > 0)
            if (physics->detectGoals() == 1)
                if (++score_playerOne >= WINNING_SCORE) {
                    setGameState(WIN_ONE);
                    score_playerOne = 0;
                    score_playerTwo = 0;
                    physics->resetPuck(table_center);
                }
                else {
                    setGameState(GOAL_ONE);
                    physics->resetPuck(table_centerRight);
                }
                else
                    if (++score_playerTwo >= WINNING_SCORE) {
                        setGameState(WIN_TWO);
                        score_playerOne = 0;
                        score_playerTwo = 0;
                        physics->resetPuck(table_center);
                    }
                    else {
                        setGameState(GOAL_TWO);
                        physics->resetPuck(table_centerLeft);
                    }
        lastTime = currentTime;
    }
}

void graphicsThread() {
    auto lastTime = std::chrono::steady_clock::now();
    auto lastTime_frameCounter = lastTime;
    int frames = 0;
    while (game_in_play) {
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::micro> deltaTime = currentTime - lastTime;
        std::chrono::duration<double, std::micro> frameCountDur = currentTime - lastTime_frameCounter;
        frames++;
        if (frameCountDur.count() >= 5e6) {
            std::cout << "Graphics Framerate: " << frames / 5 << std::endl;
            frames = 0;
            lastTime_frameCounter = currentTime;
        }
        
        bool reset_state = false;
        if (getGameState() != IN_PLAY)
            reset_state = true;
        
        if (getGameState() == IN_PLAY)
            graphics->drawGameplayImage();
        else if (getGameState() == GOAL_ONE)
            graphics->drawGoalscoredImage(true);
        else if (getGameState() == GOAL_TWO)
            graphics->drawGoalscoredImage(false);
        else if (getGameState() == WIN_ONE)
            graphics->drawGamewonImage(true);
        else if (getGameState() == WIN_TWO)
            graphics->drawGamewonImage(false);
        
        graphics->pushToScreen();
        
        if(reset_state)
            setGameState(IN_PLAY);
        
        lastTime = currentTime;
    }
}

void sensorThread() {
    auto lastTime = std::chrono::steady_clock::now();
    auto lastTime_frameCounter = lastTime;
    int frames = 0;
    while (game_in_play) {
        //while (gameState != IN_PLAY); // antiquated, trying to phase out
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::micro> deltaTime = currentTime - lastTime;
        std::chrono::duration<double, std::micro> frameCountDur = currentTime - lastTime_frameCounter;
        frames++;
        if (frameCountDur.count() >= 5e6) {
            std::cout << "Sensor Framerate: " << frames / 5 << std::endl;
            frames = 0;
            lastTime_frameCounter = currentTime;
        }
        sensor->collectFrameFromCamera();
        sensor->processFrame();
        sensor->updatePaddles(deltaTime.count());
        lastTime = currentTime;
    }
}
