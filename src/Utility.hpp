#pragma once

#include <SFML/Graphics.hpp>

namespace Utility
{
	// Normalize a vector to have a magnitude of 1 (used for direction calculations)
	sf::Vector2f normalize(const sf::Vector2f& vector);

	// Interpolate between two points
	sf::Vector2f interpolate(const sf::Vector2f& previous, const sf::Vector2f& current, float alpha);

	// Interpolate between two angles
	sf::Angle interpolate(const sf::Angle& previous, const sf::Angle& current, float alpha);

	// Check if a key was released this frame
	bool isKeyReleased(const sf::Keyboard::Key& key);

	// Lerp between two colors
	sf::Color lerp(const sf::Color& start, const sf::Color& end, float t);
}