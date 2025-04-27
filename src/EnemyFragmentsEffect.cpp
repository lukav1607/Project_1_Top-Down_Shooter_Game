#include "EnemyFragmentsEffect.hpp"
#include "Utility.hpp"

using namespace Utility;

EnemyFragmentsEffect::EnemyFragmentsEffect(sf::Vector2f center, sf::Color color)
{
	const int FRAGMENT_COUNT = 8;
	for (int i = 0; i < FRAGMENT_COUNT; ++i)
	{
		Fragment fragment;

		sf::Angle angle = sf::degrees(getRandomNumber(0.f, 360.f));
		float speed = getRandomNumber(25.f, 100.f);

		fragment.positionPrevious = center;
		fragment.positionCurrent = center;
		fragment.velocity = { std::cos(angle.asRadians()) * speed, std::sin(angle.asRadians()) * speed };

		fragment.angleCurrent = sf::degrees(getRandomNumber(0.f, 360.f));
		fragment.anglePrevious = fragment.angleCurrent;
		fragment.rotationSpeed = getRandomNumber(-90.f, 90.f);

		float size = getRandomNumber(16.f, 24.f);
		fragment.shape.setSize({ size, size });
		fragment.shape.setOrigin({ size / 2.f, size / 2.f });
		fragment.shape.setPosition(center);
		fragment.shape.setFillColor(color);

		fragment.lifetime = sf::seconds(getRandomNumber(0.5f, 1.f));

		fragments.push_back(fragment);
	}
}

void EnemyFragmentsEffect::update(float deltaTime)
{
	elapsedTime += sf::seconds(deltaTime);

	for (auto& fragment : fragments)
	{
		fragment.positionPrevious = fragment.positionCurrent;
		fragment.positionCurrent += fragment.velocity * deltaTime;
		fragment.anglePrevious = fragment.angleCurrent;
		fragment.angleCurrent += sf::degrees(fragment.rotationSpeed * deltaTime);

		// Fade out over lifetime
		float ratio = 1.f - (elapsedTime.asSeconds() / fragment.lifetime.asSeconds());
		sf::Color color = fragment.shape.getFillColor();
		color.a = static_cast<std::uint8_t>(255 * std::max(0.f, ratio));
		fragment.shape.setFillColor(color);
	}
}

void EnemyFragmentsEffect::render(float alpha, sf::RenderWindow& window)
{
	for (auto& fragment : fragments)
	{
		// Interpolate between the previous and current position for smooth rendering
		fragment.shape.setPosition(interpolate(fragment.positionPrevious, fragment.positionCurrent, alpha));

		// Interpolate between the previous and current angle for smooth rendering
		fragment.shape.setRotation(interpolate(fragment.anglePrevious, fragment.angleCurrent, alpha));

		window.draw(fragment.shape);
	}
}

bool EnemyFragmentsEffect::isFinished() const
{
	return false;
}