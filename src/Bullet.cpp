#include "Bullet.hpp"

Bullet::Bullet(const sf::Vector2f& position, const sf::Angle& angle)
{
	shape.setRadius(5.f);
	shape.setFillColor(sf::Color::Red);
	shape.setOrigin({ shape.getRadius() , shape.getRadius() });

	positionCurrent = position;
	positionPrevious = positionCurrent;
	shape.setPosition(positionCurrent);

	float speed = 1000.f;
	velocity.x = speed * std::cos(angle.asRadians());
	velocity.y = speed * std::sin(angle.asRadians());

	isOffScreen = false;
}

void Bullet::update(float deltaTime, const sf::RenderWindow& window)
{
	positionPrevious = positionCurrent;
	positionCurrent += velocity * deltaTime;
	//shape.move(velocity * deltaTime);

	// Check if the bullet is out of bounds
	if (shape.getPosition().x < 0 || shape.getPosition().x > window.getSize().x ||
		shape.getPosition().y < 0 || shape.getPosition().y > window.getSize().y)
	{
		isOffScreen = true;
	}
}

void Bullet::draw(float alpha, sf::RenderWindow& window)
{
	// Interpolate between the previous and current position for smooth rendering
	sf::Vector2f positionInterpolated = positionCurrent * alpha + positionPrevious * (1.f - alpha);
	shape.setPosition(positionInterpolated);

	window.draw(shape);
}