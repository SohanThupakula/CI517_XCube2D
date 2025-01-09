#include "MyEngineSystem.h"

// Function to calculate the distance between two points
double MyEngineSystem::CalculateDistance(double x1, double y1, double x2, double y2) {
	double dx = x2 - x1;
	double dy = y2 - y1;
	return std::sqrt(dx * dx + dy * dy);
}

// Function to check the collision between the ball and the pegs
bool MyEngineSystem::CheckCollision(const Ball& ball, const Peg& peg) {
	double distance = CalculateDistance(ball.x, ball.y, peg.x, peg.y);
	return distance <= (ball.radius + peg.radius);
}

// Function to handle the ball's response after colliding with a peg
void MyEngineSystem::HandleCollisionResponse(Ball& ball, const Peg& peg) {
	// Calculate the normal vector
	double nx = ball.x - peg.x;
	double ny = ball.y - peg.y;
	double length = std::sqrt(nx * nx + ny * ny);
	nx /= length;
	ny /= length;

	// Calculate the relative velocity in the normal direction
	double relativeVelocity = ball.vx * nx + ball.vy * ny;

	// Reflect the ball's velocity
	ball.vx -= 2 * relativeVelocity * nx;
	ball.vy -= 2 * relativeVelocity * ny;
}

void MyEngineSystem::ApplyGravity(Ball& ball, double gravity) {
	ball.vy += gravity; // Apply gravity to the vertical velocity
}

// Function to display a message when a peg is hit by the ball
void MyEngineSystem::EasterEgg(const Peg& peg) {
	std::cout << "SOHAN-CI517-2024" << std::endl;
}
