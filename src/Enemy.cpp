#include "Enemy.hpp"
#include "Utility.hpp"

#include <cmath>

using namespace Utility;

Enemy::Enemy() :
	positionCurrent({ 0.f, 0.f }),
	positionPrevious(positionCurrent),
	shapeSize(40.f),
	velocity({ 0.f, 0.f }),
	direction({ 0.f, 0.f }),
	acceleration(1000.f),
	maxSpeed(500.f),
	dampingFactor(0.85f),
	angleCurrent(sf::degrees(0.f)),
	anglePrevious(angleCurrent),
	rotationSpeed(2.f),
	healthMax(100.f),
	health(healthMax)
{
	shape.setSize({ shapeSize, shapeSize });
	shape.setFillColor(sf::Color::Red);
	shape.setOrigin(shape.getSize() / 2.f);
	shape.setPosition({ 0.f, 0.f });
}

void Enemy::update(float deltaTime, const sf::RenderWindow& window, sf::Vector2f playerPosition)
{
	// Calculate target angle to the player
	direction = playerPosition - positionCurrent; // Vector from enemy to player
	sf::Angle targetAngle = sf::radians(std::atan2(direction.y, direction.x)); // Angle of vector in radians

	// Calculate the difference in degrees between the target angle and the current angle
	sf::Angle angleDifference = sf::degrees(fmodf(targetAngle.asDegrees() - angleCurrent.asDegrees() + 180.f, 360.f) - 180.f);

	// Normalize the angle difference to the range [-180, 180] to avoid issues with wrapping
	if (angleDifference < sf::degrees(-180.f))
		angleDifference += sf::degrees(360.f);
	else if (angleDifference > sf::degrees(180.f))
	angleDifference -= sf::degrees(360.f);

	// Change the current angle towards the target angle whilst respecting the rotation speed
	anglePrevious = angleCurrent;
	angleCurrent += angleDifference * rotationSpeed * deltaTime; 
	shape.setRotation(angleCurrent);

	// Update direction and velocity
	if (direction.x != 0.f || direction.y != 0.f)
	{
		direction = normalize(direction);
		velocity += direction * acceleration * deltaTime;
	}

	// Clamp the velocity to the max speed
	float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
	if (speed > maxSpeed)
	{
		velocity = (velocity / speed) * maxSpeed;
	}

	// Apply damping (simple friction)
	velocity *= std::pow(dampingFactor, deltaTime * 60.f); // Assuming 60 FPS

	// Move shape
	positionPrevious = positionCurrent;
	positionCurrent += velocity * deltaTime; 
	shape.setPosition(positionCurrent);
}

void Enemy::render(float alpha, sf::RenderWindow& window, bool isDebugModeOn)
{
	// Interpolate between the previous and current position for smooth rendering
	shape.setPosition(interpolate(positionPrevious, positionCurrent, alpha));

	// Interpolate between the previous and current angle for smooth rendering
	shape.setRotation(interpolate(anglePrevious, angleCurrent, alpha));

	window.draw(shape);

	if (isDebugModeOn)
	{
		sf::RectangleShape debug({ shape.getGlobalBounds().size.x, shape.getGlobalBounds().size.y });
		debug.setPosition({ shape.getGlobalBounds().position.x, shape.getGlobalBounds().position.y });
		debug.setFillColor(sf::Color::Transparent);
		debug.setOutlineColor(sf::Color::Magenta);
		debug.setOutlineThickness(1.f);
		window.draw(debug);
	}
}

void Enemy::decreaseHealthBy(int amount)
{
	health -= amount;

	// Change color based on health
	float t = 1.f - std::clamp(health / healthMax, 0.f, 1.f); // 0 = full health, 1 = no health
	shape.setFillColor(lerp(sf::Color::Red, sf::Color::White, t));
}