#include "ParticleSystem.hpp"
#include "Utility.hpp"

#include <random>

using namespace Utility;

ParticleSystem::ParticleSystem() :
	spreadAngle(sf::degrees(60.f))
{
	particles.reserve(100);
}

void ParticleSystem::spawnNew(sf::Vector2f position, sf::Vector2f direction, sf::Color color, unsigned count)
{
	static std::mt19937 gen(std::random_device{}());
	//static std::uniform_real_distribution<float> angleDist(0.f, 2.f * 3.14159f);
	static std::uniform_real_distribution<float> offsetDist(-0.5f, 0.5f);
	static std::uniform_real_distribution<float> speedDist(150.f, 300.f);

	for (unsigned i = 0; i < count; ++i)
	{
		float speed = speedDist(gen);
		float size = getRandomNumber(sizeMin, sizeMax);

		//float angle = angleDist(gen);
		float angleOffset = offsetDist(gen) * spreadAngle.asRadians();
		float cosA = std::cos(angleOffset);
		float sinA = std::sin(angleOffset);

		sf::Vector2f finalDirection(
			direction.x * cosA - direction.y * sinA,
			direction.x * sinA + direction.y * cosA);

		Particle particle;
		particle.positionCurrent = position;
		particle.positionPrevious = position;
		particle.velocity = { speed * finalDirection.x, speed * finalDirection.y };
		particle.lifetime = LIFETIME_MAX;
		particle.startColor = sf::Color::White;
		particle.endColor = color;
		particle.shape.setFillColor(particle.startColor);
		particle.shape.setSize({ size, size });
		particle.shape.setOrigin(particle.shape.getSize() / 2.f);
		particle.shape.setRotation(sf::radians(getRandomNumber(0.f, 360.f)));
		particle.shape.setPosition(position);

		particles.push_back(particle);
	}
}

void ParticleSystem::update(float deltaTime)
{
	for (auto& particle : particles)
	{
		particle.lifetime -= sf::seconds(deltaTime);

		particle.velocity *= 0.95f; // Damping effect
		particle.positionPrevious = particle.positionCurrent;
		particle.positionCurrent += particle.velocity * deltaTime;

		float lifetimeProgress = 1.f - (particle.lifetime.asSeconds() / LIFETIME_MAX.asSeconds());
		lifetimeProgress = std::clamp(lifetimeProgress, 0.f, 1.f);

		std::uint8_t r = static_cast<std::uint8_t>(particle.startColor.r + (particle.endColor.r - particle.startColor.r) * lifetimeProgress);
		std::uint8_t g = static_cast<std::uint8_t>(particle.startColor.g + (particle.endColor.g - particle.startColor.g) * lifetimeProgress);
		std::uint8_t b = static_cast<std::uint8_t>(particle.startColor.b + (particle.endColor.b - particle.startColor.b) * lifetimeProgress);
		std::uint8_t a = static_cast<std::uint8_t>(255 * (particle.lifetime.asSeconds() / LIFETIME_MAX.asSeconds()));

		particle.shape.setFillColor(sf::Color(r, g, b, a));
	}

	// Remove dead particles
	particles.erase(std::remove_if(particles.begin(), particles.end(),
		[](const Particle& p) { return p.lifetime <= sf::seconds(0.f); }),
		particles.end());
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