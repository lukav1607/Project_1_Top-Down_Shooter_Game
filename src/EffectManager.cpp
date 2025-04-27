#include "EffectManager.hpp"

void EffectManager::update(float deltaTime, sf::Vector2f targetPosition)
{
	for (auto& effect : effects)
		effect->update(deltaTime, targetPosition);

	effects.erase(
		std::remove_if(effects.begin(), effects.end(),
			[](const std::shared_ptr<Effect>& e) { return e->isFinished(); }),
		effects.end());
}

void EffectManager::render(float alpha, sf::RenderWindow& window)
{
	for (const auto& effect : effects)
		effect->render(alpha, window);
}