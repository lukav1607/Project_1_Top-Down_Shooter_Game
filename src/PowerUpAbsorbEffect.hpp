#pragma once

#include "Effect.hpp"
#include <memory>

enum class PowerUpAbsorbPhase
{
	OUTWARD_BURST,
	DELAY,
	INWARD_SPIRAL
};

struct PowerUpParticle
{
	sf::Vector2f positionCurrent;
	sf::Vector2f positionPrevious;
	sf::Vector2f velocity;
	sf::Angle angleCurrent;
	sf::Angle anglePrevious;
	float rotationSpeed = 0.f;
	float scaleCurrent = 1.f;
	float scalePrevious = 1.f;
	sf::RectangleShape shape;
	bool spiraling = false;
};

class PowerUpAbsorbEffect : public Effect
{
public:
	PowerUpAbsorbEffect(sf::Vector2f pickupPosition, sf::Vector2f playerPosition, sf::Color color);

	void update(float deltaTime, sf::Vector2f targetPosition) override;
	void render(float alpha, sf::RenderWindow& window) override;
	bool isFinished() const override;
	
	void clear();

private:
	std::vector<PowerUpParticle> particles;
	sf::Vector2f playerPosition;
	PowerUpAbsorbPhase phase = PowerUpAbsorbPhase::OUTWARD_BURST;
	sf::Time elapsedTime;
};