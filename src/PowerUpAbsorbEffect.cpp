#include "PowerUpAbsorbEffect.hpp"
#include "Utility.hpp"

#include <iostream>

using namespace Utility;

PowerUpAbsorbEffect::PowerUpAbsorbEffect(sf::Vector2f pickupPosition, sf::Vector2f playerPosition, sf::Color color) :
	playerPosition(playerPosition)
{
	const int PARTICLE_COUNT = 6;
	for (int i = 0; i < PARTICLE_COUNT; ++i)
	{
		PowerUpParticle p;

		p.shape.setSize({ 6.f, 6.f });
		p.shape.setFillColor(color);
		p.shape.setOrigin(p.shape.getSize() / 2.f);
		p.shape.setPosition(pickupPosition);
		p.positionCurrent = pickupPosition;
		p.positionPrevious = pickupPosition;

		sf::Angle angle = sf::degrees(getRandomNumber(0.f, 360.f));
		float speed = getRandomNumber(150.f, 250.f);
		p.velocity = { std::cos(angle.asRadians()) * speed, std::sin(angle.asRadians()) * speed };

		p.rotationSpeed = getRandomNumber(-300.f, 300.f);
		p.angleCurrent = sf::degrees(0.f);
		p.anglePrevious = p.angleCurrent;

		p.scaleCurrent = 1.f;
		p.scalePrevious = p.scaleCurrent;

		particles.push_back(p);
	}
}

void PowerUpAbsorbEffect::update(float deltaTime, sf::Vector2f targetPosition)
{
	elapsedTime += sf::seconds(deltaTime);
	playerPosition = targetPosition;

	if (phase == PowerUpAbsorbPhase::OUTWARD_BURST)
	{
		for (auto& p : particles)
		{
			p.positionPrevious = p.positionCurrent;
			p.positionCurrent += p.velocity * deltaTime;
			p.anglePrevious = p.angleCurrent;
			p.angleCurrent += sf::degrees(p.rotationSpeed * deltaTime);
		}
		if (elapsedTime >= sf::seconds(0.3f))
		{
			phase = PowerUpAbsorbPhase::DELAY;
			elapsedTime = sf::Time::Zero;

			for (auto& p : particles)
				p.spiraling = true;
		}
	}
	else if (phase == PowerUpAbsorbPhase::DELAY)
	{
		if (elapsedTime > sf::seconds(0.01f))
		{
			phase = PowerUpAbsorbPhase::INWARD_SPIRAL;
			elapsedTime = sf::Time::Zero;
		}
	}
	else if (phase == PowerUpAbsorbPhase::INWARD_SPIRAL)
	{
		for (auto& p : particles)
		{
			if (p.spiraling)
			{
				sf::Vector2f direction = playerPosition - p.positionCurrent;
				float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
				if (distance > 1.f)
				{
					direction /= distance;
					p.velocity += direction * 400.f * deltaTime;
					p.positionPrevious = p.positionCurrent;
					p.positionCurrent += p.velocity * deltaTime;
				}

				p.anglePrevious = p.angleCurrent;
				p.angleCurrent += sf::degrees(p.rotationSpeed * deltaTime);

				p.scalePrevious = p.scaleCurrent;
				p.scaleCurrent *= 0.999f;

				sf::Color c = p.shape.getFillColor();
				c.a = static_cast<std::uint8_t>(c.a * 0.98f);
				p.shape.setFillColor(c);
			}
		}
	}
}

void PowerUpAbsorbEffect::render(float alpha, sf::RenderWindow& window)
{
	for (auto& p : particles)
	{
		p.shape.setPosition(interpolate(p.positionPrevious, p.positionCurrent, alpha));
		p.shape.setRotation(interpolate(p.anglePrevious, p.angleCurrent, alpha));
		float scale = interpolate(p.scalePrevious, p.scaleCurrent, alpha);
		p.shape.setScale({ scale, scale });

		window.draw(p.shape);
	}
}

bool PowerUpAbsorbEffect::isFinished() const
{
	if (phase != PowerUpAbsorbPhase::INWARD_SPIRAL)
		return false;

	// Check if all particles are small and almost invisible
	for (const auto& p : particles)
	{
		if (p.scaleCurrent > 0.1f || p.shape.getFillColor().a > 10/* || playerPosition != p.positionCurrent*/)
		{
			std::cout << "Particle not finished" << std::endl;
			return false;
		}
	}
	std::cout << "Particle finished" << std::endl;
	return true;
}
