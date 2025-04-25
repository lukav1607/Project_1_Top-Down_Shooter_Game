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
		LIFE
	};

	PowerUp(const sf::Vector2u& windowSize, int playerLivesCurrent, unsigned playerLivesMax);

	void activate();
	void update(float deltaTime);
	void render(float alpha, sf::RenderWindow& window, bool isDebugModeOn);

	inline bool getIsPickedUp() const { return isPickedUp; }
	inline bool getNeedsDeleting() const { return needsDeleting; }
	inline Type getType() const { return type; }
	inline sf::Time getTimeLeft() const { return buffDuration - timer; }
	inline sf::Vector2f getPosition() const { return shape.getPosition(); }
	inline float getCollisionRadius() const { return collisionRadius; }

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