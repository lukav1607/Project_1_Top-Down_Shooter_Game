#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

class Player {
public:
	Player(const sf::RenderWindow& const window);

	void handleInput();
	void update(float deltaTime, const sf::RenderWindow& const window);
	void draw(sf::RenderWindow& window);

private:
	sf::RectangleShape shape;
	sf::Vector2f velocity;
	sf::Vector2f direction;
	float acceleration;
	float maxSpeed;
	float dampingFactor;

	sf::Vector2f normalize(const sf::Vector2f& vector);
};