#include "Bullet.hpp"
#include "Utility.hpp"

using Utility::interpolate;

Bullet::Bullet(const sf::Vector2f& position, const sf::Angle& angle, sf::Color color, float speedMultiplier, float sizeMultiplier)
{
	shape.setRadius(5.f * sizeMultiplier);
	shape.setFillColor(color);
	shape.setOrigin({ shape.getRadius() , shape.getRadius() });

	positionCurrent = position;
	positionPrevious = positionCurrent;
	shape.setPosition(positionCurrent);

	float speed = 1000.f * speedMultiplier;
	velocity.x = speed * std::cos(angle.asRadians());
	velocity.y = speed * std::sin(angle.asRadians());

	isMarkedForDeletion = false;
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

void Bullet::render(float alpha, sf::RenderWindow& window)
{
	// Interpolate between the previous and current position for smooth rendering
	shape.setPosition(interpolate(positionPrevious, positionCurrent, alpha));

	window.draw(shape);
}