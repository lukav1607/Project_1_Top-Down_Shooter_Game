#include "Enemy.hpp"
#include "Utility.hpp"

using Utility::normalize;
using Utility::interpolate;

Enemy::Enemy() :
	positionCurrent({ 0.f, 0.f }),
	positionPrevious(positionCurrent),
	shapeSize(35.f),
	velocity({ 0.f, 0.f }),
	direction({ 0.f, 0.f }),
	acceleration(750.f),
	maxSpeed(250.f),
	dampingFactor(0.9f),
	angleCurrent(sf::degrees(0.f)),
	anglePrevious(angleCurrent),
	rotationSpeed(4.5f),
	healthMax(100),
	health(healthMax)
{
	shape.setSize({ shapeSize, shapeSize });
	shape.setFillColor(sf::Color::Red);
	shape.setOrigin(shape.getSize() / 2.f);
	shape.setPosition(positionCurrent);
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
}

void Enemy::render(float alpha, sf::RenderWindow& window)
{
	// Interpolate between the previous and current position for smooth rendering
	shape.setPosition(interpolate(positionPrevious, positionCurrent, alpha));

	// Interpolate between the previous and current angle for smooth rendering
	shape.setRotation(interpolate(anglePrevious, angleCurrent, alpha));

	window.draw(shape);
}

void Enemy::decreaseHealthBy(int amount)
{
	health -= amount;
}