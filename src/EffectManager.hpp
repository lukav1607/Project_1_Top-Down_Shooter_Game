#pragma once

#include "Effect.hpp"

class EffectManager
{
public:
	inline void addEffect(std::shared_ptr<Effect> effect) { effects.push_back(effect); }

	void update(float deltaTime, sf::Vector2f targetPosition);
	void render(float alpha, sf::RenderWindow& window);

	void clear();

private:
	std::vector<std::shared_ptr<Effect>> effects;
};