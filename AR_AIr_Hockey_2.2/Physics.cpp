#include "Physics.h"

using namespace std;

Physics::Physics() {
	puck_position[0] = table_width / 2;
	puck_position[1] = table_height / 2;
	puck_velocity[0] = 0.0;
	puck_velocity[1] = 0.0;
	paddleOne_position[0] = table_width / 4;
	paddleOne_position[1] = table_height/2;
	paddleTwo_position[0] = table_width * 3 / 4;
	paddleTwo_position[1] = table_height / 2;
}

void Physics::tick(const long double deltaTime_micros) {
	puck_position[0] += puck_velocity[0] * deltaTime_micros * 1e-6;
	puck_position[1] += puck_velocity[1] * deltaTime_micros * 1e-6;

	double magnitude = sqrt(pow(puck_velocity[0], 2) + pow(puck_velocity[1], 2));
	double angle = atan2(puck_velocity[1], puck_velocity[0]);

	if (magnitude > PUCK_FRICTION)
		magnitude -= (PUCK_FRICTION * deltaTime_micros * 1e-6);
	else
		magnitude = 0;

	puck_velocity[0] = magnitude * cos(angle);
	puck_velocity[1] = magnitude * sin(angle);

	if (abs(puck_velocity[0]) < (PUCK_FRICTION * deltaTime_micros * 1e-6))
		puck_velocity[0] = 0;
	if (abs(puck_velocity[1]) < (PUCK_FRICTION * deltaTime_micros * 1e-6))
		puck_velocity[1] = 0;

	handleCollisions();
}

void Physics::handleCollisions() {
	if (puck_position[1] > (table_height + GOAL_WIDTH) / 2 || puck_position[1] < (table_height - GOAL_WIDTH) / 2) {
		if (puck_position[0] <= PUCK_RADIUS + WALL_PADDING_THICKNESS || puck_position[0] >= (table_width - PUCK_RADIUS - WALL_PADDING_THICKNESS)) {
			puck_velocity[0] *= (-1.0 * WALL_ELASTICITY);
			if (puck_position[0] <= PUCK_RADIUS + WALL_PADDING_THICKNESS)
				puck_position[0] = PUCK_RADIUS + 1 + WALL_PADDING_THICKNESS;
			else
				puck_position[0] = table_width - PUCK_RADIUS - 1 - WALL_PADDING_THICKNESS;
		}
		if (puck_position[1] <= PUCK_RADIUS + WALL_PADDING_THICKNESS || puck_position[1] >= (table_height - PUCK_RADIUS - WALL_PADDING_THICKNESS)) {
			puck_velocity[1] *= (-1.0 * WALL_ELASTICITY);
			if (puck_position[1] <= PUCK_RADIUS + WALL_PADDING_THICKNESS)
				puck_position[1] = PUCK_RADIUS + 1 + WALL_PADDING_THICKNESS;
			else
				puck_position[1] = table_height - PUCK_RADIUS - 1 - WALL_PADDING_THICKNESS;
		}
	}
	double* paddle_position;
	double* paddle_velocity;
	bool isPaddleOne;
	if (hasCollision(true)) {
		paddle_position = paddleOne_position;
		paddle_velocity = paddleOne_velocity;
		isPaddleOne = true;
	}
	else if (hasCollision(false)) {
		paddle_position = paddleTwo_position;
		paddle_velocity = paddleTwo_velocity;
		isPaddleOne = false;
	}
	else
		return;
	double collisionNormal = atan2(puck_position[1] - paddle_position[1], puck_position[0] - paddle_position[0]);
	double collisionHeading = atan2(paddle_velocity[1] - puck_velocity[1], paddle_velocity[0] - puck_velocity[0]);
	double magnitudeOfInteraction = sqrt(pow(puck_velocity[0] - paddle_velocity[0], 2) + pow(puck_velocity[1] - paddle_velocity[1], 2));

	double bounceHeading = collisionNormal + (collisionNormal - collisionHeading);

	puck_velocity[0] = magnitudeOfInteraction * cos(bounceHeading) + paddle_velocity[0];
	puck_velocity[1] = magnitudeOfInteraction * sin(bounceHeading) + paddle_velocity[1];

	while (hasCollision(isPaddleOne)) {
		puck_position[0] += cos(collisionNormal);
		puck_position[1] += sin(collisionNormal);
	}
}

bool Physics::hasCollision(const bool isPaddleOne) {
	if(isPaddleOne)
		return (sqrt(pow(puck_position[0] - paddleOne_position[0], 2) + pow(puck_position[1] - paddleOne_position[1], 2)) <= (PUCK_RADIUS + PADDLE_RADIUS));
	else
		return (sqrt(pow(puck_position[0] - paddleTwo_position[0], 2) + pow(puck_position[1] - paddleTwo_position[1], 2)) <= (PUCK_RADIUS + PADDLE_RADIUS));
}

int Physics::detectGoals() {
	if (puck_position[1] > (table_height + GOAL_WIDTH) / 2 || puck_position[1] < (table_height - GOAL_WIDTH) / 2)
		return 0;
	if (puck_position[0] <= PUCK_RADIUS + WALL_PADDING_THICKNESS)
		return 2;
	if (puck_position[0] >= table_width - PUCK_RADIUS - WALL_PADDING_THICKNESS)
		return 1;
	return 0;
}

void Physics::resetPuck(const double* new_position) {
	puck_position[0] = new_position[0];
	puck_position[1] = new_position[1];
	puck_velocity[0] = 0.0;
	puck_velocity[1] = 0.0;
}