#pragma once

#include <SFML/Graphics.hpp>

class HUD
{
public:
	HUD(const sf::Font& font, const sf::RenderWindow& window);

	void update(const sf::RenderWindow& window, float healthCurrent, float healthMax, int score);
	void render(sf::RenderWindow& window) const;

private:
	void setHealth(float healthCurrent, float healthMax);
	void setScore(int score);
	void updatePositions(const sf::RenderWindow& window);

	const sf::Font& font;

	sf::Text healthText;
	sf::Text scoreText;
};