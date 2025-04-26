#include "ParticleSystem.hpp"
#include "Utility.hpp"

#include <random>

using namespace Utility;

ParticleSystem::ParticleSystem()
{
	particles.reserve(100);
}

void ParticleSystem::spawnNew(sf::Vector2f position, unsigned count)
{
	static std::mt19937 gen(std::random_device{}());
	static std::uniform_real_distribution<float> angleDist(0.f, 2.f * 3.14159f);
	static std::uniform_real_distribution<float> speedDist(50.f, 150.f);

	for (unsigned i = 0; i < count; ++i)
	{
		float angle = angleDist(gen);
		float speed = speedDist(gen);

		Particle particle;
		particle.positionCurrent = position;
		particle.positionPrevious = position;
		particle.velocity = { speed * std::cos(angle), speed * std::sin(angle) };
		particle.lifetime = LIFETIME_MAX;
		particle.shape.setSize({ 4.f, 4.f });
		particle.shape.setFillColor(sf::Color::White);
		particle.shape.setOrigin(particle.shape.getSize() / 2.f);
		particle.shape.setRotation(sf::radians(angle));
		particle.shape.setPosition(position);

		particles.push_back(particle);
	}
}

void ParticleSystem::update(float deltaTime)
{
	for (auto& particle : particles)
	{
		particle.lifetime -= sf::seconds(deltaTime);
		particle.positionPrevious = particle.positionCurrent;
		particle.positionCurrent += particle.velocity * deltaTime;
		sf::Color color = particle.shape.getFillColor();
		color.a = static_cast<std::uint8_t>(255 * (particle.lifetime.asSeconds() / LIFETIME_MAX.asSeconds()));
		particle.shape.setFillColor(color);

		// Remove dead particles
		particles.erase(std::remove_if(particles.begin(), particles.end(),
			[](const Particle& p) { return p.lifetime <= sf::seconds(0.f); }),
			particles.end());
	}
}

void ParticleSystem::render(float alpha, sf::RenderWindow& window, bool isDebugModeOn)
{
	for (auto& particle : particles)
	{
		// Interpolate between the previous and current position for smooth rendering
		particle.shape.setPosition(interpolate(particle.positionPrevious, particle.positionCurrent, alpha));

		window.draw(particle.shape);

		/*if (isDebugModeOn)
		{
			sf::CircleShape debug(2.f);
			debug.setFillColor(sf::Color::Transparent);
			debug.setOutlineColor(sf::Color::Magenta);
			debug.setOutlineThickness(2.f);
			debug.setOrigin({ 2.f, 2.f });
			debug.setPosition(particle.shape.getPosition());
			window.draw(debug);
		}*/
	}
}