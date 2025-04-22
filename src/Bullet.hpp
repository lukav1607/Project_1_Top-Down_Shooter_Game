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
	inline float getCollisionRadius() const { return collisionRadius; }
	inline sf::Vector2f getPosition() const { return positionCurrent; }
	inline sf::FloatRect getGlobalBounds() const { return shape.getGlobalBounds(); }
	inline unsigned getDamage() const { return damage; }

private:
	sf::Vector2f positionCurrent;
	sf::Vector2f positionPrevious;

	sf::CircleShape shape;
	float collisionRadius;

	sf::Vector2f velocity;
	bool isMarkedForDeletion;

	int damage;
};