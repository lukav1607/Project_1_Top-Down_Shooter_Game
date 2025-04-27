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

	inline bool getIsActivated() const { return isActivated; }

	inline bool getNeedsDeleting() const { return needsDeleting; }
	inline Type getType() const { return type; }
	inline sf::Color getColor() const { return color; }
	inline const std::string& getName() const { return name; }
	inline sf::Time getTimeLeft() const { return buffDuration - timer; }
	inline sf::Vector2f getPosition() const { return shape.getPosition(); }
	inline float getCollisionRadius() const { return collisionRadius; }

private:
	sf::RectangleShape shape;
	sf::Color color;
	float shapeSize;
	float collisionRadius;

	sf::Angle angleCurrent;
	sf::Angle anglePrevious;
	float rotationSpeed;

	sf::Clock pulseClock;
	float scaleCurrent;
	float scalePrevious;
	float scaleAmplitude;
	float scaleSpeed;

	bool isActivated;
	bool needsDeleting;

	Type type;
	std::string name;
	sf::Time buffDuration;
	sf::Time despawnTime;
	sf::Time timer;
};