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

	inline int getHealth() const { return health; }
	inline sf::FloatRect getGlobalBounds() const { return shape.getGlobalBounds(); }

private:
	sf::Vector2f positionCurrent;
	sf::Vector2f positionPrevious;

	sf::RectangleShape shape;
	float shapeSize;

	sf::Vector2f velocity;
	sf::Vector2f direction;
	float acceleration;
	float maxSpeed;
	float dampingFactor; // A factor to reduce velocity over time (simulates friction)

	sf::Angle angleCurrent;
	sf::Angle anglePrevious;
	float rotationSpeed;

	unsigned healthMax;
	int health;

	std::vector<Bullet> bullets;
	sf::Color bulletColor = sf::Color::Red;
};