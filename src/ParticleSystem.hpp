#pragma once

#include <vector>

#include "Particle.hpp"

class ParticleSystem
{
public:
	ParticleSystem();

	void spawnNew(sf::Vector2f position, unsigned count);

	void update(float deltaTime);
	void render(float alpha, sf::RenderWindow& window, bool isDebugModeOn);

private:
	std::vector<Particle> particles;

	const sf::Time LIFETIME_MAX = sf::seconds(0.75f);
};