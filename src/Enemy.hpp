#pragma once

#include <SFML/Graphics.hpp>

#include "Bullet.hpp"

class Enemy
{
public:
	Enemy();

	void update(float deltaTime, const sf::RenderWindow& window, sf::Vector2f playerPosition);
	void render(float alpha, sf::RenderWindow& window, bool isDebugModeOn);

	void decreaseHealthBy(int amount);

	inline float getHealth() const { return health; }
	inline float getCollisionRadius() const { return collisionRadius; }
	inline sf::Vector2f getPosition() const { return positionCurrent; }
	inline sf::FloatRect getGlobalBounds() const { return shape.getGlobalBounds(); }

private:
	sf::Vector2f positionCurrent;
	sf::Vector2f positionPrevious;

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
	float health;

	std::vector<Bullet> bullets;
	sf::Color bulletColor = sf::Color::Red;
};