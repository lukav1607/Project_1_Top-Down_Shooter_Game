#include "EnemyFragmentsEffect.hpp"
#include "Utility.hpp"

using namespace Utility;

EnemyFragmentsEffect::EnemyFragmentsEffect(sf::Vector2f center, sf::Color color)
{
	const int FRAGMENT_COUNT = 8;
	for (int i = 0; i < FRAGMENT_COUNT; ++i)
	{
		Fragment f;

		sf::Angle angle = sf::degrees(getRandomNumber(0.f, 360.f));
		float speed = getRandomNumber(25.f, 100.f);

		f.positionPrevious = center;
		f.positionCurrent = center;
		f.velocity = { std::cos(angle.asRadians()) * speed, std::sin(angle.asRadians()) * speed };

		f.angleCurrent = sf::degrees(getRandomNumber(0.f, 360.f));
		f.anglePrevious = f.angleCurrent;
		f.rotationSpeed = getRandomNumber(-90.f, 90.f);

		float size = getRandomNumber(16.f, 24.f);
		f.shape.setSize({ size, size });
		f.shape.setOrigin({ size / 2.f, size / 2.f });
		f.shape.setPosition(center);

		f.targetColor = color;
		f.startColor = sf::Color::White;
		f.shape.setFillColor(f.startColor);

		f.lifetime = sf::seconds(getRandomNumber(0.5f, 1.f));

		fragments.push_back(f);
	}
}

void EnemyFragmentsEffect::update(float deltaTime, sf::Vector2f targetPosition)
{
	elapsedTime += sf::seconds(deltaTime);

	for (auto& f : fragments)
	{
		f.positionPrevious = f.positionCurrent;
		f.positionCurrent += f.velocity * deltaTime;
		f.anglePrevious = f.angleCurrent;
		f.angleCurrent += sf::degrees(f.rotationSpeed * deltaTime);

		float lifetimeProgress = 1.f - (elapsedTime.asSeconds() / f.lifetime.asSeconds());
		lifetimeProgress = std::clamp(lifetimeProgress, 0.f, 1.f);

		std::uint8_t r = static_cast<std::uint8_t>(f.targetColor.r + (f.startColor.r - f.targetColor.r) * lifetimeProgress);
		std::uint8_t g = static_cast<std::uint8_t>(f.targetColor.g + (f.startColor.g - f.targetColor.g) * lifetimeProgress);
		std::uint8_t b = static_cast<std::uint8_t>(f.targetColor.b + (f.startColor.b - f.targetColor.b) * lifetimeProgress);
		std::uint8_t a = static_cast<std::uint8_t>(255 * std::max(0.f, lifetimeProgress));

		f.shape.setFillColor(sf::Color(r, g, b, a));
	}
}

void EnemyFragmentsEffect::render(float alpha, sf::RenderWindow& window)
{
	for (auto& f : fragments)
	{
		// Interpolate between the previous and current position for smooth rendering
		f.shape.setPosition(interpolate(f.positionPrevious, f.positionCurrent, alpha));

		// Interpolate between the previous and current angle for smooth rendering
		f.shape.setRotation(interpolate(f.anglePrevious, f.angleCurrent, alpha));

		window.draw(f.shape);
	}
}

bool EnemyFragmentsEffect::isFinished() const
{
	return elapsedTime > LIFETIME_MAX;
}