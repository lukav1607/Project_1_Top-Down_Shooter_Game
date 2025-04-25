#pragma once

#include <SFML/Graphics.hpp>

class HUD
{
public:
	HUD(const sf::Font& font, const sf::RenderWindow& window);

	void update(const sf::RenderWindow& window, int livesCurrent, unsigned livesMax, int score);
	void render(sf::RenderWindow& window) const;

private:
	void setLives(int livesCurrent, unsigned livesMax);
	void setScore(int score);
	void updatePositions(const sf::RenderWindow& window);

	const sf::Font& font;

	sf::Text livesText;
	sf::Text scoreText;
};