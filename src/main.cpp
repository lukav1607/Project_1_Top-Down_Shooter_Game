#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Enemy.hpp"
#include "HUD.hpp"
#include "Utility.hpp"

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

    Player player(window);
	std::vector<Enemy> enemies;

    unsigned maxEnemies = 50;
	sf::Time spawnInterval = sf::seconds(0.5f);
	sf::Time timeSinceLastSpawn = spawnInterval;

	sf::Font font("assets/fonts/unispace bd.ttf");
	HUD hud(font, window);

	sf::Text titleText(font, "TOP DOWN SHOOTER", 60U);
	titleText.setFillColor(sf::Color::White);
	titleText.setOrigin({ titleText.getGlobalBounds().size.x / 2.f, titleText.getGlobalBounds().size.y / 2.f });
	titleText.setPosition({ window.getSize().x / 2.f, window.getSize().y / 2.f - 50.f });
	sf::Text startText(font, "Press Enter to start", 30U);
	startText.setFillColor(sf::Color::White);
	startText.setOrigin({ startText.getGlobalBounds().size.x / 2.f, startText.getGlobalBounds().size.y / 2.f });
	startText.setPosition({ window.getSize().x / 2.f, window.getSize().y / 2.f + 50.f });

	bool isGameStarted = false;

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
			}
			window.clear();
			window.draw(titleText);
			window.draw(startText);
			window.display();
		}
		else {
			float frameTime = clock.restart().asSeconds();
			accumulator += frameTime;

			player.handleInput();

			if (isKeyReleased(sf::Keyboard::Key::F3))
				isDebugModeOn = !isDebugModeOn;

			int fixedUpdateCount = 0;

			while (accumulator >= TIMESTEP)
			{
				timeSinceLastSpawn += sf::seconds(TIMESTEP);
				if (timeSinceLastSpawn >= spawnInterval && enemies.size() < maxEnemies)
				{
					enemies.emplace_back();
					timeSinceLastSpawn = sf::seconds(0.f);
				}

				for (auto& enemy : enemies)
					player.score += enemy.update(TIMESTEP, window, player.getPosition());
				player.update(TIMESTEP, window, enemies);
				hud.update(window, player.getHealthCurrent(), player.getHealthMax(), player.score);

				// Remove dead enemies
				enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& e) { return e.getNeedsDeleting(); }), enemies.end());

				accumulator -= TIMESTEP;
				fixedUpdateCount++;
			}

			float alpha = accumulator / TIMESTEP;

			window.clear();

			player.render(alpha, window, isDebugModeOn);
			for (auto& enemy : enemies)
				enemy.render(alpha, window, isDebugModeOn);
			hud.render(window);

			window.display();
		}
    }
	return 0;
}