#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "Bullet.hpp"
#include "Enemy.hpp"

class Player
{
public:
	Player(const sf::RenderWindow& window);

	void handleInput();
	void update(float deltaTime, const sf::RenderWindow& window, std::vector<Enemy>& enemies);
	void render(float alpha, sf::RenderWindow& window, bool isDebugModeOn);

	inline float getHealthCurrent() const { return healthCurrent; }
	inline float getHealthMax() const { return healthMax; }
	inline sf::Vector2f getPosition() const { return positionCurrent; }

	int score;

private:
	void launchBullet();
	void updateMovement(float deltaTime);
	void updateRotation(float deltaTime, const sf::RenderWindow& window);
	void updateShooting(float deltaTime, const sf::RenderWindow& window, std::vector<Enemy>& enemies);
	void updateCollisions(float deltaTime, const sf::RenderWindow& window, std::vector<Enemy>& enemies);

	sf::ConvexShape shape;
	float shapeSize;
	float offset; // Offset from center to the tip of the player shape
	float collisionRadius;

	sf::Vector2f positionCurrent;
	sf::Vector2f positionPrevious;
	sf::Vector2f velocity;
	sf::Vector2f direction;
	float acceleration;
	float maxSpeed;
	float dampingFactor; // A factor to reduce velocity over time (simulates friction)

	sf::Angle angleCurrent;
	sf::Angle anglePrevious;
	float rotationSpeed;

	float healthMax;
	float healthCurrent;

	std::vector<Bullet> bullets;
	sf::Color bulletColor = sf::Color(255, 230, 100);
	bool isShooting;
	sf::Time fireRate;
	sf::Time timeSinceLastShot;
	float bulletSpeedMultiplier;
	float bulletSizeMultiplier;
};