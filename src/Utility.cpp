#include "Utility.hpp"

#include <map>

sf::Vector2f Utility::normalize(const sf::Vector2f& vector)
{
	// Calculate the length of the vector
	float length = std::sqrt(vector.x * vector.x + vector.y * vector.y);

	// Normalize the vector to a unit vector, avoid division by zero
	if (length != 0.f)
		return vector / length;
	else
		return { 0.f, 0.f };
}

sf::Vector2f Utility::interpolate(const sf::Vector2f& previous, const sf::Vector2f& current, float alpha)
{
	return previous * (1.f - alpha) + current * alpha;
}

sf::Angle Utility::interpolate(const sf::Angle& previous, const sf::Angle& current, float alpha)
{
	return previous * (1.f - alpha) + current * alpha;
}

bool Utility::isKeyReleased(const sf::Keyboard::Key& key)
{
	static std::map<sf::Keyboard::Key, bool> keyStates;
	if (keyStates.find(key) == keyStates.end())
		keyStates[key] = false;
	bool isPressedNow = sf::Keyboard::isKeyPressed(key);
	bool wasPressedLastFrame = keyStates[key];
	keyStates[key] = isPressedNow;
	return !isPressedNow && wasPressedLastFrame;
}