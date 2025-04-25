#include "HUD.hpp"

#include <sstream>
#include <iomanip>

HUD::HUD(const sf::Font& font, const sf::RenderWindow& window) :
	font(font),
	livesText(font),
	scoreText(font)
{
	livesText.setFillColor(sf::Color::White);
	livesText.setCharacterSize(32);
	livesText.setPosition({ 20.f, 20.f });

	scoreText.setFillColor(sf::Color::White);
	scoreText.setCharacterSize(32);

	updatePositions(window);
}

void HUD::setLives(int livesCurrent, unsigned livesMax)
{
	std::ostringstream ss;
	ss << "Lives: " /*<< std::fixed << std::setprecision(0)*/ << livesCurrent << " / " << livesMax;
	livesText.setString(ss.str());
}

void HUD::setScore(int score)
{
	scoreText.setString("Score: " + std::to_string(score));
}

void HUD::update(const sf::RenderWindow& window, int livesCurrent, unsigned livesMax, int score)
{
	setLives(livesCurrent, livesMax);
	setScore(score);
	updatePositions(window);
}

void HUD::render(sf::RenderWindow& window) const
{
	window.draw(livesText);
	window.draw(scoreText);	
}

void HUD::updatePositions(const sf::RenderWindow& window)
{
	scoreText.setOrigin({ scoreText.getGlobalBounds().size.x, 0.f });
	scoreText.setPosition({ window.getSize().x - 20.f, 20.f });
}