#include "Utility.hpp"

#include <SFML/System.hpp>
#include <map>
#include <random>

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

float Utility::interpolate(float previous, float current, float alpha)
{
	return previous * (1.f - alpha) + current * alpha;
}

sf::Vector2f Utility::interpolate(const sf::Vector2f& previous, const sf::Vector2f& current, float alpha)
{
	return previous * (1.f - alpha) + current * alpha;
}

sf::Angle Utility::interpolate(const sf::Angle& previous, const sf::Angle& current, float alpha)
{
	return previous * (1.f - alpha) + current * alpha;
}

float Utility::getRandomNumber(float min, float max)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(min, max);
	return dist(gen);
}

int Utility::getRandomNumber(int min, int max)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(min, max);
	return dist(gen);
}

bool Utility::doesCircleIntersectRectangle(sf::Vector2f circleCenter, float circleRadius, sf::FloatRect rectangle)
{
	// Find the closest point to the circle within the rectangle
	float closestX = std::clamp(circleCenter.x, rectangle.position.x, rectangle.position.x + rectangle.size.x);
	float closestY = std::clamp(circleCenter.y, rectangle.position.y, rectangle.position.y + rectangle.size.y);

	// Calculate the distance between the circle's center and this closest point
	float dx = circleCenter.x - closestX;
	float dy = circleCenter.y - closestY;

	// If the distance is less than or equal to the circle's radius, an intersection occurs
	return (dx * dx + dy * dy) <= (circleRadius * circleRadius);
}

bool Utility::doesCircleIntersectCircle(sf::Vector2f circleCenter1, float circleRadius1, sf::Vector2f circleCenter2, float circleRadius2)
{
	// Calculate the distance between the centers of the two circles
	float dx = circleCenter1.x - circleCenter2.x;
	float dy = circleCenter1.y - circleCenter2.y;
	float distanceSquared = dx * dx + dy * dy;

	// Calculate the sum of the radii
	float radiusSum = circleRadius1 + circleRadius2;

	// Check if the distance is less than or equal to the sum of the radii
	return distanceSquared <= (radiusSum * radiusSum);
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

sf::Color Utility::lerp(const sf::Color& start, const sf::Color& end, float t)
{
	return sf::Color
	(
		static_cast<std::uint8_t>(start.r + (end.r - start.r) * t),
		static_cast<std::uint8_t>(start.g + (end.g - start.g) * t),
		static_cast<std::uint8_t>(start.b + (end.b - start.b) * t),
		static_cast<std::uint8_t>(start.a + (end.a - start.a) * t)
	);
}

sf::Vector3f Utility::rgbToHsv(const sf::Color& color)
{
	float r = color.r / 255.f, g = color.g / 255.f, b = color.b / 255.f;
	float max = std::max({ r, g, b });
	float min = std::min({ r, g, b });
	float h, s, v = max;

	float d = max - min;
	s = max == 0 ? 0 : d / max;

	if (max == min)
		h = 0;
	else if (max == r)
		h = fmod((g - b) / d + (g < b ? 6 : 0), 6.f);
	else if (max == g)
		h = (b - r) / d + 2.f;
	else
		h = (r - g) / d + 4.f;

	h *= 60.f;
	return { h, s, v };
}

sf::Color Utility::hsvToRgb(float h, float s, float v)
{
	float c = v * s;
	float x = c * (1 - fabs(fmod(h / 60.f, 2) - 1));
	float m = v - c;

	float r, g, b;
	if		(h < 60)  r = c, g = x, b = 0;
	else if (h < 120) r = x, g = c, b = 0;
	else if (h < 180) r = 0, g = c, b = x;
	else if (h < 240) r = 0, g = x, b = c;
	else if (h < 300) r = x, g = 0, b = c;
	else              r = c, g = 0, b = x;

	return sf::Color(
		static_cast<std::uint8_t>((r + m) * 255),
		static_cast<std::uint8_t>((g + m) * 255),
		static_cast<std::uint8_t>((b + m) * 255)
	);
}

sf::Color Utility::hsvToRgb(const sf::Vector3f& hsv)
{ 
	return hsvToRgb(hsv.x, hsv.y, hsv.z);
}
