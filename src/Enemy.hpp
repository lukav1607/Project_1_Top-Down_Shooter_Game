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
	bool getNeedsDeleting() const { return hasFinishedRendering; }
	inline float getHealth() const { return healthMax; }
	inline float getDamage() const { return damage; }
	inline float getCollisionRadius() const { return collisionRadius; }
	inline sf::Vector2f getPosition() const { return positionCurrent; }
	inline sf::FloatRect getGlobalBounds() const { return shape.getGlobalBounds(); }
	inline sf::Color getColor() const { return shape.getFillColor(); }

	int scoreValue;

private:
	void setRotation(const sf::Vector2f& playerPosition);
	void updateRotation(float deltaTime, const sf::Vector2f& playerPosition);
	void setPosition(sf::Vector2f position);
	void updateColor();

	bool isFirstUpdate;
	bool isAboutToDie;
	bool hasFinishedRendering;

	sf::Vector2f positionCurrent;
	sf::Vector2f positionPrevious;

	float speedGrowth;
	float sizeGrowth;
	float healthGrowth;

	sf::RectangleShape shape;
	float shapeSize;
	float collisionRadius;

	float scaleCurrent;
	float scalePrevious;

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