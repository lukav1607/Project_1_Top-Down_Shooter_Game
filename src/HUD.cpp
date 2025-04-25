#include "HUD.hpp"

#include <sstream>
#include <iomanip>

HUD::HUD(const sf::Font& font, const sf::RenderWindow& window) :
	font(font),
	lives(font),
	score(font),
	powerUp(font)
{
	lives.setFillColor(sf::Color::White);
	lives.setCharacterSize(32);
	lives.setPosition({ 20.f, 20.f });

	score.setFillColor(sf::Color::White);
	score.setCharacterSize(32);

	powerUp.setCharacterSize(32);

	updatePositions(window);
}

void HUD::setLives(int livesCurrent, unsigned livesMax)
{
	std::ostringstream ss;
	ss << "Lives: " /*<< std::fixed << std::setprecision(0)*/ << livesCurrent << " / " << livesMax;
	lives.setString(ss.str());
}

void HUD::setScore(int score)
{
	this->score.setString("Score: " + std::to_string(score));
}

void HUD::updatePowerUp(const std::shared_ptr<PowerUp>& activePowerUp)
{
	if (activePowerUp != nullptr && activePowerUp->getTimeLeft() > sf::Time::Zero && activePowerUp->getType() != PowerUp::Type::LIFE)
	{
		std::ostringstream ss;
		ss << std::fixed << std::setprecision(1) << "Power-up: " << activePowerUp->getName() << " (" << activePowerUp->getTimeLeft().asSeconds() << "s)";
		powerUp.setString(ss.str());
	}
	else
	{
		powerUp.setString("");
	}
}

void HUD::update(const sf::RenderWindow& window, int livesCurrent, unsigned livesMax, int score, const std::shared_ptr<PowerUp>& activePowerUp)
{
	setLives(livesCurrent, livesMax);
	setScore(score);
	updatePowerUp(activePowerUp);
	updatePositions(window);
}

void HUD::render(sf::RenderWindow& window) const
{
	window.draw(lives);
	window.draw(score);
	window.draw(powerUp);
}

void HUD::updatePositions(const sf::RenderWindow& window)
{
	score.setOrigin({ score.getGlobalBounds().size.x, 0.f });
	score.setPosition({ window.getSize().x - 20.f, 20.f });
	powerUp.setOrigin({ 0.f, powerUp.getGlobalBounds().size.y });
	powerUp.setPosition({ 20.f, window.getSize().y - 20.f});
}