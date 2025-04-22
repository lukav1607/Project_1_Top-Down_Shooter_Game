#pragma once

#include <SFML/Graphics.hpp>

class Bullet
{
public: 
	Bullet(const sf::Vector2f& position, const sf::Angle& angle, sf::Color color = sf::Color::White, float speedMultiplier = 1.f, float sizeMultiplier = 1.f);

	void update(float deltaTime, const sf::RenderWindow& window);
	void render(float alpha, sf::RenderWindow& window, bool isDebugModeOn);
	void markForDeletion() { isMarkedForDeletion = true; }

	inline bool getIsMarkedForDeletion() const { return isMarkedForDeletion; }
	inline sf::FloatRect getGlobalBounds() const { return shape.getGlobalBounds(); }inline unsigned getDamage() const { return shape.getRadius() * 10.f; }

private:
	sf::Vector2f positionCurrent;
	sf::Vector2f positionPrevious;

	sf::CircleShape shape;
	sf::Vector2f velocity;
	bool isMarkedForDeletion;
};