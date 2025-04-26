#pragma once

#include <SFML/Graphics.hpp>

struct Particle
{
	sf::Vector2f positionCurrent;
	sf::Vector2f positionPrevious;
	sf::Vector2f velocity;
	sf::Time lifetime;
	sf::RectangleShape shape;
};