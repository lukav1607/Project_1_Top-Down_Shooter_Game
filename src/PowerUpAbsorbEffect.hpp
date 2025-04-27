#pragma once

#include "Effect.hpp"

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
	float rotationSpeed;
	sf::RectangleShape shape;
	bool spiraling = false;
};

class PowerUpAbsorbEffect : public Effect
{
public:
	PowerUpAbsorbEffect(sf::Vector2f pickupPosition, sf::Vector2f playerPosition, sf::Color color);

	void update(float deltaTime) override;
	void render(float alpha, sf::RenderWindow& window) override;
	bool isFinished() const override;

private:
	std::vector<PowerUpParticle> particles;
	sf::Vector2f playerPosition;
	PowerUpAbsorbPhase phase = PowerUpAbsorbPhase::OUTWARD_BURST;
	sf::Time elapsedTime;
};