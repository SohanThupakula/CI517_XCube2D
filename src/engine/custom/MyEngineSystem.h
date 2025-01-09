#ifndef __MY_ENGINE_H__
#define __MY_ENGINE_H__

#include "../EngineCommon.h"
#include <vector>
#include <cmath>
#include <iostream> 

class MyEngineSystem {
	friend class XCube2Engine;
private:

public:
	struct Ball {
		double x;
		double y;
		double radius;
		double vx; // velocity in x direction
		double vy; // velocity in y direction
	};

	struct Peg {
		double x;
		double y;
		double radius;
	};

	// Function to calculate the distance between two points
	double CalculateDistance(double x1, double y1, double x2, double y2);

	// Function to check the collision between the ball and the pegs
	bool CheckCollision(const Ball& ball, const Peg& peg);

	// Function to handle the ball's response after colliding with a peg
	void HandleCollisionResponse(Ball& ball, const Peg& peg);
	int HandleCollisionResponse(int param1, int param2);

	// Function to apply gravity to the ball
	void ApplyGravity(Ball& ball, double gravity);

	// Function to display a message when a peg is hit by the ball
	void EasterEgg(const Peg& peg);
};

#endif