#pragma once

#include <SFML/Graphics.hpp>

#include "PowerUp.hpp"

class HUD
{
public:
	HUD(const sf::Font& font, const sf::RenderWindow& window);

	void update(const sf::RenderWindow& window, int livesCurrent, unsigned livesMax, int score, const std::shared_ptr<PowerUp>& activePowerUp);
	void render(sf::RenderWindow& window) const;

private:
	void setLives(int livesCurrent, unsigned livesMax);
	void setScore(int score);
	void updatePowerUp(const std::shared_ptr<PowerUp>& activePowerUp);
	void updatePositions(const sf::RenderWindow& window);

	const sf::Font& font;

	sf::Text lives;
	sf::Text score;
	sf::Text powerUp;
};