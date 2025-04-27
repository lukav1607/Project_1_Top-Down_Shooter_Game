#pragma once

#include "Effect.hpp"

struct Fragment
{
	sf::Vector2f positionCurrent;
	sf::Vector2f positionPrevious;
	sf::Vector2f velocity;
	sf::Angle angleCurrent;
	sf::Angle anglePrevious;
	float rotationSpeed = 0.f;
	sf::RectangleShape shape;
	sf::Color targetColor;
	sf::Color startColor;
	sf::Time lifetime;
};

class EnemyFragmentsEffect : public Effect
{
public:
	EnemyFragmentsEffect(sf::Vector2f center, sf::Color color);

	void update(float deltaTime, sf::Vector2f targetPosition) override;
	void render(float alpha, sf::RenderWindow& window) override;
	bool isFinished() const override;

	void clear();

private:
	std::vector<Fragment> fragments;
	sf::Time elapsedTime;
	const sf::Time LIFETIME_MAX = sf::seconds(1.f);
};