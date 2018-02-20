#pragma once
#include "GameData.h"
#include "Graphics.h"
#include "Physics.h"
#include "Sensor.h"

bool game_in_play = true;
int gameState = IN_PLAY;
double puck_position[2] = { 0,0 }, puck_velocity[2] = { 0,0 };
double paddleOne_position[2] = { 0,0 }, paddleOne_velocity[2] = { 0,0 };
double paddleTwo_position[2] = { 0,0 }, paddleTwo_velocity[2] = { 0,0 };
double table_width = 0, table_height = 0, table_center[2] = { 0,0 }, table_centerLeft[2] = { 0,0 }, table_centerRight[2] = { 0,0 };
int score_playerOne = 0, score_playerTwo = 0;

int main();
void physicsThread();
void graphicsThread();
void sensorThread();

std::mutex mu;

void setGameState(int new_state) {
	mu.lock();
	gameState = new_state;
	mu.unlock();
}

int getGameState() {
	return gameState;
}