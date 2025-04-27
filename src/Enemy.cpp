#include "Enemy.hpp"
#include "Utility.hpp"

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

using namespace Utility;

Enemy::Enemy() :
	isFirstUpdate(true),
	hasFinishedRendering(false),
	isAboutToDie(false),
	positionCurrent({ 0.f, 0.f }),
	positionPrevious(positionCurrent),
	speedGrowth(0.02f),
	sizeGrowth(0.045f),
	healthGrowth(0.066f),
	shapeSize(60.f),
	collisionRadius(shapeSize / 2.f),
	scaleCurrent(1.f),
	scalePrevious(1.f),
	velocity({ 0.f, 0.f }),
	direction({ 0.f, 0.f }),
	acceleration(1000.f),
	maxSpeed(500.f),
	dampingFactor(0.85f),
	angleCurrent(sf::degrees(0.f)),
	anglePrevious(angleCurrent),
	rotationSpeed(2.f),
	healthMax(5.f),
	healthCurrent(healthMax),
	flashTimer(sf::seconds(0.f)),
	damage((shapeSize / 10.f + maxSpeed / 100.f) * 1.75f),
	scoreValue(0),
	color(sf::Color::Red)
{
	shape.setFillColor(color);
	shape.setPosition({ 0.f, 0.f });
}

void Enemy::initTimeBasedModifiers(sf::Time timeSinceStart)
{
	float speedVariation = getRandomNumber(0.9f, 1.1f); // +/- 10%
	float sizeVariation = getRandomNumber(0.95f, 1.05f); // +/- 5%
	float difficultyFactor = timeSinceStart.asSeconds() / 10.f;

	healthMax *= (1.f + difficultyFactor * healthGrowth) * sizeVariation;
	healthMax = std::floorf(healthMax);
	healthCurrent = healthMax;

	maxSpeed *= (1.f + difficultyFactor * speedGrowth) * speedVariation;

	shapeSize *= (1.f + difficultyFactor * sizeGrowth) * sizeVariation;
	shape.setSize({ shapeSize, shapeSize });
	shape.setOrigin(shape.getSize() / 2.f);
	collisionRadius = shapeSize / 2.f;

	damage = (shapeSize / 10.f + maxSpeed / 100.f) * 1.75f,

	scoreValue = static_cast<int>((maxSpeed * shapeSize) / 1000.f);
}

int Enemy::update(float deltaTime, const sf::RenderWindow& window, sf::Vector2f playerPosition)
{
	if (healthCurrent <= 0.f && !isAboutToDie)
		isAboutToDie = true;

	if (isFirstUpdate)
	{
		setRandomSpawnPosition(window);
		setRotation(playerPosition);
		isFirstUpdate = false;
	}

	updateRotation(deltaTime, playerPosition);

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

	if (flashTimer > sf::Time::Zero)
	{
		flashTimer -= sf::seconds(deltaTime);
		shape.setFillColor(sf::Color::White);
	}
	else
	{
		updateColor();
	}

	if (isAboutToDie && !hasFinishedRendering)
	{
		scalePrevious = scaleCurrent;
		scaleCurrent -= deltaTime * 10.f;
		if (scaleCurrent <= 0.f)
		{
			hasFinishedRendering = true;
			return scoreValue;
		}
	}

	return 0;
}

void Enemy::render(float alpha, sf::RenderWindow& window, bool isDebugModeOn)
{
	// Interpolate between the previous and current position for smooth rendering
	shape.setPosition(interpolate(positionPrevious, positionCurrent, alpha));

	// Interpolate between the previous and current angle for smooth rendering
	shape.setRotation(interpolate(anglePrevious, angleCurrent, alpha));

	if (isAboutToDie && !hasFinishedRendering)
		shape.setScale({ interpolate(scalePrevious, scaleCurrent, alpha), interpolate(scalePrevious, scaleCurrent, alpha) });

	window.draw(shape);

	if (isDebugModeOn)
	{
		sf::CircleShape debug(collisionRadius);
		debug.setFillColor(sf::Color::Transparent);
		debug.setOutlineColor(sf::Color::Magenta);
		debug.setOutlineThickness(2.f);
		debug.setOrigin({ collisionRadius, collisionRadius });
		debug.setPosition({ positionCurrent.x, positionCurrent.y });
		window.draw(debug);
	}
}

void Enemy::decreaseHealthBy(int amount)
{
	healthCurrent -= amount;
	//healthCurrent = std::floorf(healthCurrent);
	flashTimer = sf::seconds(0.025f); // Flash for 0.1 seconds
	updateColor();
}

void Enemy::setRandomSpawnPosition(const sf::RenderWindow& window)
{
	enum
	{
		TOP,
		BOTTOM,
		LEFT,
		RIGHT
	};

	int spawnSide = std::rand() % 4;
	sf::Vector2f spawnPosition;

	switch (spawnSide)
	{
	case TOP:
		spawnPosition.x = std::rand() % window.getSize().x;  // Random X within window width
		spawnPosition.y = -shape.getSize().y * 1.5f;	// Spawn just above the window
		break;

	case BOTTOM:
		spawnPosition.x = std::rand() % window.getSize().x;  // Random X within window width
		spawnPosition.y = window.getSize().y * 1.5f;    // Spawn just below the window
		break;

	case LEFT:
		spawnPosition.x = -shape.getSize().x * 1.5f;	// Spawn just left of the window
		spawnPosition.y = std::rand() % window.getSize().y;  // Random Y within window height
		break;

	case RIGHT:
		spawnPosition.x = window.getSize().x * 1.5f;    // Spawn just right of the window
		spawnPosition.y = std::rand() % window.getSize().y;  // Random Y within window height
		break;
	}

	setPosition(spawnPosition);
}

void Enemy::setRotation(const sf::Vector2f& playerPosition)
{
	// Calculate target angle to the player
	direction = playerPosition - positionCurrent; // Vector from enemy to player
	sf::Angle targetAngle = sf::radians(std::atan2(direction.y, direction.x)); // Angle of vector in radians

	// Instantly set the current angle to the target angle
	angleCurrent = targetAngle;
	anglePrevious = angleCurrent;
	shape.setRotation(angleCurrent);
}

void Enemy::updateRotation(float deltaTime, const sf::Vector2f& playerPosition)
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
}

void Enemy::setPosition(sf::Vector2f position)
{
	positionCurrent = position;
	positionPrevious = positionCurrent;
	shape.setPosition(positionCurrent);
}

void Enemy::updateColor()
{
	float t = 1.f - std::clamp(healthCurrent / healthMax, 0.f, 1.f);

	float startHue = 300.f;  // deeper purple
	float endHue = 360.f;    // red
	float hue = startHue + t * (endHue - startHue);
	if (hue >= 360.f) hue -= 360.f;

	// Higher saturation and still bright, but not washed out
	float saturation = 0.9f - t * 0.2f;  // from 0.9 to 0.7
	float value = 0.7f + t * 0.25f;      // from 0.7 to 0.95

	color = hsvToRgb(hue, saturation, value);
	shape.setFillColor(color);
}