#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Enemy.hpp"
#include "PowerUp.hpp"
#include "HUD.hpp"
#include "Utility.hpp"

#include <iostream>

using Utility::isKeyReleased;

int main() {
    auto settings = sf::ContextSettings();
    settings.antiAliasingLevel = 8;
    auto window = sf::RenderWindow(sf::VideoMode({ 1200u, 1200u }), "Top Down Shooter", sf::State::Windowed, settings);
    window.setVerticalSyncEnabled(true);

	bool isDebugModeOn = false;

	const float UPS = 60.f; // Updates per second
	const float TIMESTEP = 1.f / UPS;
	float accumulator = 0.f; // Time accumulator for fixed timestep
    sf::Clock clock;
	clock.stop();
	sf::Clock gameClock;
	clock.stop();

    Player player(window);
	std::vector<Enemy> enemies;
	std::vector<PowerUp> powerups;

    unsigned maxEnemies = 50;
	sf::Time spawnIntervalMin = sf::seconds(0.5f);
	sf::Time spawnIntervalMax = sf::seconds(1.25f);
	sf::Time spawnIntervalRampUp = sf::seconds(120.f);
	sf::Time spawnInterval = spawnIntervalMax;
	sf::Time timeSinceLastSpawn = spawnInterval;

	sf::Time powerupSpawnIntervalMin = sf::seconds(15.f);
	sf::Time powerupSpawnIntervalMax = sf::seconds(30.f);
	sf::Time powerupSpawnIntervalRampUp = sf::seconds(120.f);
	sf::Time powerupSpawnInterval = powerupSpawnIntervalMax;
	sf::Time powerupTimeSinceLastSpawn = powerupSpawnInterval;

	sf::Font font("assets/fonts/unispace bd.ttf");
	HUD hud(font, window);

	sf::Text titleText(font, "TOP-DOWN SHOOTER", 60U);
	titleText.setFillColor(sf::Color::White);
	titleText.setOrigin({ titleText.getGlobalBounds().size.x / 2.f, titleText.getGlobalBounds().size.y / 2.f });
	titleText.setPosition({ window.getSize().x / 2.f, window.getSize().y / 2.f - 50.f });
	sf::Text startText(font, "Press Enter to start", 30U);
	startText.setFillColor(sf::Color::White);
	startText.setOrigin({ startText.getGlobalBounds().size.x / 2.f, startText.getGlobalBounds().size.y / 2.f });
	startText.setPosition({ window.getSize().x / 2.f, window.getSize().y / 2.f + 50.f });

	sf::Text gameOverText(font, "GAME OVER", 60U);
	gameOverText.setFillColor(sf::Color::White);
	gameOverText.setOrigin({ gameOverText.getGlobalBounds().size.x / 2.f, gameOverText.getGlobalBounds().size.y / 2.f });
	gameOverText.setPosition({ window.getSize().x / 2.f, window.getSize().y / 2.f - 80.f });
	sf::Text scoreText(font, "", 45U);
	scoreText.setFillColor(sf::Color::White);
	sf::Text restartText(font, "Press Enter to restart", 30U);
	restartText.setFillColor(sf::Color::White);
	restartText.setOrigin({ restartText.getGlobalBounds().size.x / 2.f, restartText.getGlobalBounds().size.y / 2.f });
	restartText.setPosition({ window.getSize().x / 2.f, window.getSize().y / 2.f + 60.f });

	bool isGameStarted = false;
	bool isGameOver = false;

	std::srand(static_cast<unsigned>(std::time(nullptr)));

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
		
		if (!isGameStarted) {
			if (isKeyReleased(sf::Keyboard::Key::Enter)) {
				isGameStarted = true;
				clock.start();
				gameClock.start();
			}
			window.clear();
			window.draw(titleText);
			window.draw(startText);
			window.display();
		}
		else if (isGameOver)
		{
			if (isKeyReleased(sf::Keyboard::Key::Enter)) {
				isGameOver = false;
				isGameStarted = false;
				clock.reset();
				gameClock.reset();
				player.reset(window);
				enemies.clear();
			}

			scoreText.setString("Score: " + std::to_string(player.score));
			scoreText.setOrigin({ scoreText.getGlobalBounds().size.x / 2.f, scoreText.getGlobalBounds().size.y / 2.f });
			scoreText.setPosition({ window.getSize().x / 2.f, window.getSize().y / 2.f - 20.f });

			window.clear();
			window.draw(gameOverText);
			window.draw(scoreText);
			window.draw(restartText);
			window.display();
		}
		else {
			float frameTime = clock.restart().asSeconds();
			accumulator += frameTime;

			player.handleInput();

			if (isKeyReleased(sf::Keyboard::Key::F3))
				isDebugModeOn = !isDebugModeOn;
			else if (isKeyReleased(sf::Keyboard::Key::Escape))
				window.close();

			int fixedUpdateCount = 0;

			while (accumulator >= TIMESTEP)
			{
				timeSinceLastSpawn += sf::seconds(TIMESTEP);
				if (timeSinceLastSpawn >= spawnInterval && enemies.size() < maxEnemies)
				{
					enemies.emplace_back();
					enemies.back().initTimeBasedModifiers(gameClock.getElapsedTime());
					timeSinceLastSpawn = sf::seconds(0.f);

					// Smoothly transition spawnInterval from max to min linearly
					// After spawnIntervalRampUpTime seconds, it will be at spawnIntervalMin
					float t = std::min(gameClock.getElapsedTime().asSeconds() / spawnIntervalRampUp.asSeconds(), 1.f);
					spawnInterval = spawnIntervalMax - (spawnIntervalMax - spawnIntervalMin) * t;
				}

				powerupTimeSinceLastSpawn += sf::seconds(TIMESTEP);
				if (powerupTimeSinceLastSpawn >= powerupSpawnInterval)
				{
					powerups.emplace_back(window.getSize());
					powerupTimeSinceLastSpawn = sf::seconds(0.f);

					// Smoothly transition powerupSpawnInterval from max to min linearly
					// After powerupSpawnIntervalRampUpTime seconds, it will be at powerupSpawnIntervalMin
					float t = std::min(gameClock.getElapsedTime().asSeconds() / powerupSpawnIntervalRampUp.asSeconds(), 1.f);
					powerupSpawnInterval = powerupSpawnIntervalMax - (powerupSpawnIntervalMax - powerupSpawnIntervalMin) * t;
				}

				for (auto& powerup : powerups)
					powerup.update(TIMESTEP);

				for (auto& enemy : enemies)
					player.score += enemy.update(TIMESTEP, window, player.getPosition());

				player.update(TIMESTEP, window, enemies);
				hud.update(window, player.getHealthCurrent(), player.getHealthMax(), player.score);

				// Remove dead enemies
				enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& e) { return e.getNeedsDeleting(); }), enemies.end());
				// Remove timed-out powerups
				powerups.erase(std::remove_if(powerups.begin(), powerups.end(), [](const PowerUp& p) { return p.getNeedsDeleting(); }), powerups.end());

				if (player.getHealthCurrent() <= 0.f)
				{
					isGameOver = true;
					break;
				}

				accumulator -= TIMESTEP;
				fixedUpdateCount++;
			}

			float alpha = accumulator / TIMESTEP;

			window.clear();

			player.render(alpha, window, isDebugModeOn);

			for (auto& powerup : powerups)
				powerup.render(alpha, window, isDebugModeOn);

			for (auto& enemy : enemies)
				enemy.render(alpha, window, isDebugModeOn);

			hud.render(window);

			window.display();
		}
    }
	return 0;
}