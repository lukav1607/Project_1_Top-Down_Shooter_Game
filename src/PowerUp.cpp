#include "PowerUp.hpp"
#include "Utility.hpp"

using namespace Utility;

PowerUp::PowerUp(const sf::Vector2u& windowSize, int playerLivesCurrent, unsigned playerLivesMax) :
	rotationSpeed(10.f),
	scaleCurrent(1.f),
	scalePrevious(1.f),
	scaleAmplitude(0.15f),
	scaleSpeed(3.f),
	shapeSize(40.f),
	collisionRadius(shapeSize / 2.f),
	isPickedUp(false),
	needsDeleting(false),
	buffDuration(sf::seconds(8.f)),
	despawnTime(sf::seconds(8.f)),
	timer(sf::seconds(0.f)),
	type(Type::DAMAGE)
{
	shape.setSize({ shapeSize, shapeSize });
	shape.setOrigin({ shapeSize / 2.f, shapeSize / 2.f });
	shape.setRotation(sf::degrees(45.f));
	shape.setPosition
	( 
		{ getRandomNumber(shapeSize, windowSize.x - shapeSize),
		getRandomNumber(shapeSize, windowSize.y - shapeSize) }
	);

	int randomType = getRandomNumber(0, 3);

	// Ensure that the life power-up does not spawn if the player has max lives
	if (playerLivesCurrent >= playerLivesMax)
	{
		while (randomType == 3)
			randomType = getRandomNumber(0, 3);
	}

	switch (randomType)
	{
	case 0:
		type = Type::DAMAGE;
		color = sf::Color(242, 76, 107);
		shape.setFillColor(color);
		break;
	case 1:
		type = Type::FIRE_RATE;
		color = sf::Color(92, 229, 138);
		shape.setFillColor(color);
		break;
	case 2:
		type = Type::SPEED;
		color = sf::Color(122, 204, 242);
		shape.setFillColor(color);
		break;
	case 3:
		type = Type::LIFE;
		color = sf::Color(242, 138, 154);
		shape.setFillColor(color);
		break;
	}
}

void PowerUp::activate()
{
	isPickedUp = true;
	timer = sf::seconds(0.f);
}

void PowerUp::update(float deltaTime)
{
	timer += sf::seconds(deltaTime);

	if (!isPickedUp)
	{
		scalePrevious = scaleCurrent;
		scaleCurrent = std::sin(pulseClock.getElapsedTime().asSeconds() * scaleSpeed) * scaleAmplitude + 1.f;

		sf::Time flashStartTime = sf::seconds(3.f);
		sf::Time timeLeft = despawnTime - timer;
		if (timeLeft <= flashStartTime)
		{
			// Flashing effect
			float progress = 1.f - (timeLeft.asSeconds() / flashStartTime.asSeconds());
			float flashFrequency = 0.25f + progress * 1.5f;
			float flash = std::abs(std::sin(timer.asSeconds() * flashFrequency * 3.14159f));
			sf::Color flashingColor = color;
			flashingColor.a = static_cast<std::uint8_t>(flash * 255);
			shape.setFillColor(flashingColor);
		}
		else
		{
			shape.setFillColor(color);
		}

		if (timer >= despawnTime)
			needsDeleting = true;
	}
	else
	{
		if (timer >= buffDuration || type == Type::LIFE)
			needsDeleting = true;
	}
}

void PowerUp::render(float alpha, sf::RenderWindow& window, bool isDebugModeOn)
{
	if (!isPickedUp)
	{
		shape.setScale({ interpolate(scalePrevious, scaleCurrent, alpha), interpolate(scalePrevious, scaleCurrent, alpha) });

		window.draw(shape);

		if (isDebugModeOn)
		{
			sf::CircleShape debug(collisionRadius);
			debug.setFillColor(sf::Color::Transparent);
			debug.setOutlineColor(sf::Color::Magenta);
			debug.setOutlineThickness(2.f);
			debug.setOrigin({ collisionRadius, collisionRadius });
			debug.setPosition(shape.getPosition());
			window.draw(debug);
		}
	}
}