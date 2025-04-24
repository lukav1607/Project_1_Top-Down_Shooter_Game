#pragma once

#include <SFML/Graphics.hpp>

#include "Bullet.hpp"

class Enemy
{
public:
	Enemy();
	void setRandomSpawnPosition(const sf::RenderWindow& window);

	void initTimeBasedModifiers(sf::Time timeSinceStart);
	int update(float deltaTime, const sf::RenderWindow& window, sf::Vector2f playerPosition);
	void render(float alpha, sf::RenderWindow& window, bool isDebugModeOn);

	void decreaseHealthBy(int amount);

	bool getNeedsDeleting() const { return needsDeleting; }
	inline float getHealth() const { return healthCurrent; }
	inline float getDamage() const { return damage; }
	inline float getCollisionRadius() const { return collisionRadius; }
	inline sf::Vector2f getPosition() const { return positionCurrent; }
	inline sf::FloatRect getGlobalBounds() const { return shape.getGlobalBounds(); }

	int scoreValue;

private:
	void setRotation(const sf::Vector2f& playerPosition);
	void updateRotation(float deltaTime, const sf::Vector2f& playerPosition);
	void setPosition(sf::Vector2f position);
	void updateColor();

	bool isFirstUpdate;
	bool needsDeleting;

	sf::Vector2f positionCurrent;
	sf::Vector2f positionPrevious;

	float speedGrowth;
	float sizeGrowth;
	float healthGrowth;

	sf::RectangleShape shape;
	float shapeSize;
	float collisionRadius;

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
	sf::Time flashTimer;
	float damage;

	std::vector<Bullet> bullets;
	sf::Color bulletColor = sf::Color::Red;
};