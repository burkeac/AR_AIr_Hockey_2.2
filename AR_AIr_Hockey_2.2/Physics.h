#pragma once
#include "GameData.h"

using namespace std;

class Physics {
public:
	Physics();
	void tick(const long double);
	void handleCollisions();
	bool hasCollision(const bool);
	int detectGoals();
	void resetPuck(const double*);
};