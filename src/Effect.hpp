#pragma once

#include <SFML/Graphics.hpp>

class Effect
{
public:
	virtual ~Effect() = default;
	virtual void update(float deltaTime) = 0;
	virtual void render(float alpha, sf::RenderWindow& window) = 0;
	virtual bool isFinished() const = 0;
};