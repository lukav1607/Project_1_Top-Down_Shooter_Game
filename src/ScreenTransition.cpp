#include "ScreenTransition.hpp"
#include "Utility.hpp"

#include <iostream>

using namespace Utility;

ScreenTransition::ScreenTransition(sf::Vector2u windowSize) :
	windowSize(windowSize),
	speed(1500.f),
	isFinished(false),
	isActive(false),
	mode(Mode::OPENING)
{
	rectangles.resize(4); 

	for (int i = 0; i < 4; i++)
	{
		rectangles.at(i).shape.setFillColor(sf::Color::Black);
		rectangles.at(i).shape.setSize({ static_cast<float>(windowSize.x), static_cast<float>(windowSize.y) });
		rectangles.at(i).shape.setOrigin({ 0.f, rectangles.at(i).shape.getSize().y / 2.f });
		rectangles.at(i).shape.setRotation(sf::degrees(45.f * (i + 1) + i * 45.f));
	}
	rectangles.at(0).direction = { sf::Vector2f(1.f, 1.f) };
	rectangles.at(1).direction = { sf::Vector2f(-1.f, 1.f) };
	rectangles.at(2).direction = { sf::Vector2f(-1.f, -1.f) };
	rectangles.at(3).direction = { sf::Vector2f(1.f, -1.f) };
}

void ScreenTransition::start(Mode mode, sf::Vector2f position)
{
	isActive = true;
	isFinished = false;
	this->mode = mode;
	center = position;

	for (auto& rectangle : rectangles)
	{
		if (mode == Mode::OPENING)
		{
			rectangle.positionCurrent = position;
			rectangle.positionPrevious = position;
		}
		else if (mode == Mode::CLOSING)
		{
			rectangle.positionCurrent.x = center.x + rectangle.direction.x * windowSize.x;
			rectangle.positionCurrent.y = center.y + rectangle.direction.y * windowSize.y;
			rectangle.positionPrevious = rectangle.positionCurrent;
		}
	}
}

void ScreenTransition::update(float deltaTime)
{
	if (mode == Mode::OPENING)
	{
		for (auto& rectangle : rectangles)
		{
			rectangle.positionPrevious = rectangle.positionCurrent;
			rectangle.positionCurrent += rectangle.direction * speed * deltaTime;

			if (rectangle.positionCurrent.x >= windowSize.x && rectangle.positionCurrent.y >= windowSize.y)
			{
				isFinished = true;
				isActive = false;
			}
		}
	}
	else if (mode == Mode::CLOSING)
	{
		for (auto& rectangle : rectangles)
		{
			rectangle.positionPrevious = rectangle.positionCurrent;
			rectangle.positionCurrent -= rectangle.direction * speed * deltaTime;
		}
		if ((rectangles.at(0).positionCurrent.x <= center.x && rectangles.at(0).positionCurrent.y <= center.y) ||
			(rectangles.at(1).positionCurrent.x >= center.x && rectangles.at(1).positionCurrent.y <= center.y) ||
			(rectangles.at(2).positionCurrent.x >= center.x && rectangles.at(2).positionCurrent.y >= center.y) ||
			(rectangles.at(3).positionCurrent.x <= center.x && rectangles.at(3).positionCurrent.y >= center.y))
		{
			isFinished = true;
			isActive = false;
		}
	}
}

void ScreenTransition::render(float alpha, sf::RenderWindow& window)
{
	if (!isFinished)
	{
		for (auto& rectangle : rectangles)
		{
			rectangle.shape.setPosition(interpolate(rectangle.positionPrevious, rectangle.positionCurrent, alpha));
			window.draw(rectangle.shape);
		}
	}
} 