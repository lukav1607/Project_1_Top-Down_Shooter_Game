#include "Bullet.hpp"
#include "Utility.hpp"

#include <iostream>

using Utility::interpolate;

Bullet::Bullet(const sf::Vector2f& position, const sf::Angle& angle, sf::Color color, float speedMultiplier, float sizeMultiplier) :
	positionCurrent(position),
	positionPrevious(positionCurrent),
	isMarkedForDeletion(false),
	damage(10)
{
	shape.setRadius(5.f * sizeMultiplier);
	shape.setFillColor(color);
	shape.setOrigin({ shape.getRadius() , shape.getRadius() });

	shape.setPosition(positionCurrent);

	float speed = 750.f * speedMultiplier;
	velocity.x = speed * std::cos(angle.asRadians());
	velocity.y = speed * std::sin(angle.asRadians());
}

void Bullet::update(float deltaTime, const sf::RenderWindow& window)
{
	positionPrevious = positionCurrent;
	positionCurrent += velocity * deltaTime;

	// Check if the bullet is out of bounds
	if (shape.getPosition().x < 0 || shape.getPosition().x > window.getSize().x ||
		shape.getPosition().y < 0 || shape.getPosition().y > window.getSize().y)
	{
		isMarkedForDeletion = true;
	}
}

void Bullet::render(float alpha, sf::RenderWindow& window, bool isDebugModeOn)
{
	// Interpolate between the previous and current position for smooth rendering
	shape.setPosition(interpolate(positionPrevious, positionCurrent, alpha));

	window.draw(shape);

	if (isDebugModeOn)
	{
		sf::RectangleShape debug({ shape.getGlobalBounds().size.x, shape.getGlobalBounds().size.y });
		debug.setPosition({ shape.getGlobalBounds().position.x, shape.getGlobalBounds().position.y });
		debug.setFillColor(sf::Color::Transparent);
		debug.setOutlineColor(sf::Color::Magenta);
		debug.setOutlineThickness(1.f);
		window.draw(debug);
	}
}