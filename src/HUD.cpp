#include "HUD.hpp"

#include <sstream>
#include <iomanip>

HUD::HUD(const sf::Font& font, const sf::RenderWindow& window) :
	font(font),
	healthText(font),
	scoreText(font)
{
	healthText.setFillColor(sf::Color::White);
	healthText.setCharacterSize(32);
	healthText.setPosition({ 20.f, 20.f });

	scoreText.setFillColor(sf::Color::White);
	scoreText.setCharacterSize(32);

	updatePositions(window);
}

void HUD::setHealth(float healthCurrent, float healthMax)
{
	std::ostringstream ss;
	ss << "HP: " << std::fixed << std::setprecision(0) << healthCurrent << " / " << healthMax;
	healthText.setString(ss.str());
}

void HUD::setScore(int score)
{
	scoreText.setString("Score: " + std::to_string(score));
}

void HUD::update(const sf::RenderWindow& window, float healthCurrent, float healthMax, int score)
{
	setHealth(healthCurrent, healthMax);
	setScore(score);
	updatePositions(window);
}

void HUD::render(sf::RenderWindow& window) const
{
	window.draw(healthText);
	window.draw(scoreText);	
}

void HUD::updatePositions(const sf::RenderWindow& window)
{
	scoreText.setOrigin({ scoreText.getGlobalBounds().size.x, 0.f });
	scoreText.setPosition({ window.getSize().x - 20.f, 20.f });
}