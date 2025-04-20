#pragma once

#include <SFML/Graphics.hpp>

class Bullet
{
public: 
	Bullet(const sf::Vector2f& position, const sf::Angle& angle);

	void update(float deltaTime, const sf::RenderWindow& window);
	void draw(float alpha, sf::RenderWindow& window);

	bool getIsOffScreen() const { return isOffScreen; }

private:
	sf::Vector2f positionCurrent;
	sf::Vector2f positionPrevious;

	sf::CircleShape shape;
	sf::Vector2f velocity;
	bool isOffScreen;
};