#pragma once

#include <SFML/Graphics.hpp>

namespace Utility
{
	// Normalize a vector to have a magnitude of 1 (used for direction calculations)
	sf::Vector2f normalize(const sf::Vector2f& vector);

	// Calculate dot product of two vectors
	float dot(const sf::Vector2f& a, const sf::Vector2f& b);

	// Interpolate between two floats
	float interpolate(float previous, float current, float alpha);
	// Interpolate between two points
	sf::Vector2f interpolate(const sf::Vector2f& previous, const sf::Vector2f& current, float alpha);
	// Interpolate between two angles
	sf::Angle interpolate(const sf::Angle& previous, const sf::Angle& current, float alpha);

	float getRandomNumber(float min, float max);
	int getRandomNumber(int min, int max);

	bool doesCircleIntersectRectangle(sf::Vector2f circleCenter, float circleRadius, sf::FloatRect rectangle);
	bool doesCircleIntersectCircle(sf::Vector2f circleCenter1, float circleRadius1, sf::Vector2f circleCenter2, float circleRadius2);

	// Check if a key was released this frame
	bool isKeyReleased(const sf::Keyboard::Key& key);

	// Lerp between two colors
	sf::Color lerp(const sf::Color& start, const sf::Color& end, float t);
	sf::Vector3f rgbToHsv(const sf::Color& color);
	sf::Color hsvToRgb(float h, float s, float v);
	sf::Color hsvToRgb(const sf::Vector3f& hsv);
}