#include "PowerUp.hpp"
#include "Utility.hpp"

using namespace Utility;

PowerUp::PowerUp(const sf::Vector2u& windowSize) :
	rotationSpeed(10.f),
	scaleCurrent(1.f),
	scalePrevious(1.f),
	scaleMax(1.25f),
	scaleMin(0.75f),
	scaleSpeed(1.f),
	shapeSize(33.f),
	collisionRadius(shapeSize / 2.f),
	isPickedUp(false),
	needsDeleting(false),
	buffDuration(sf::seconds(7.5f)),
	timeout(sf::seconds(7.5f)),
	timer(sf::seconds(0.f)),
	type(Type::DAMAGE)
{
	shape.setSize({ shapeSize, shapeSize });
	shape.setOrigin({ shapeSize / 2.f, shapeSize / 2.f });
	//shape.setRotation(sf::degrees(getRandomNumber(0.f, 360.f)));
	shape.setRotation(sf::degrees(45.f));
	shape.setPosition
	( 
		{ getRandomNumber(shapeSize, windowSize.x - shapeSize),
		getRandomNumber(shapeSize, windowSize.y - shapeSize) }
	);

	int randomType = getRandomNumber(0, 4);
	switch (randomType)
	{
	case 0:
		type = Type::DAMAGE;
		shape.setFillColor(sf::Color(242, 107, 76));
		break;
	case 1:
		type = Type::FIRE_RATE;
		shape.setFillColor(sf::Color(92, 229, 138));
		break;
	case 2:
		type = Type::SPEED;
		shape.setFillColor(sf::Color(122, 204, 242));
		break;
	case 3:
		type = Type::HEALTH;
		shape.setFillColor(sf::Color(242, 138, 154));
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
		//anglePrevious = angleCurrent;
		//angleCurrent += sf::degrees(rotationSpeed * deltaTime);

		if (timer >= timeout)
			needsDeleting = true;
	}
	else
	{
		if (timer >= buffDuration || type == Type::HEALTH)
			needsDeleting = true;
	}
}

void PowerUp::render(float alpha, sf::RenderWindow& window, bool isDebugModeOn)
{
	if (!isPickedUp)
	{
		//shape.setRotation(interpolate(anglePrevious, angleCurrent, alpha));

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