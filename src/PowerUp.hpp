#pragma once

#include <SFML/Graphics.hpp>

class PowerUp
{
public:
	enum class Type
	{
		DAMAGE,
		FIRE_RATE,
		SPEED,
		SHRINK,
		HEALTH
	};

	PowerUp(const sf::Vector2u& windowSize);

	void activate();
	void update(float deltaTime);
	void render(float alpha, sf::RenderWindow& window, bool isDebugModeOn);

	inline bool getNeedsDeleting() const { return needsDeleting; }
	inline Type getType() const { return type; }
	inline sf::Time getTimeLeft() const { return buffDuration - timer; }

private:
	sf::RectangleShape shape;
	float shapeSize;
	float collisionRadius;

	sf::Angle angleCurrent;
	sf::Angle anglePrevious;
	float rotationSpeed;

	float scaleCurrent;
	float scalePrevious;
	float scaleMax;
	float scaleMin;
	float scaleSpeed;

	bool isPickedUp;
	bool needsDeleting;

	Type type;
	sf::Time buffDuration;
	sf::Time timeout;
	sf::Time timer;
};