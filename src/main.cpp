#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Enemy.hpp"

int main() {
    auto settings = sf::ContextSettings();
    settings.antiAliasingLevel = 8;
    auto window = sf::RenderWindow(sf::VideoMode({ 1200u, 1200u }), "Top Down Shooter", sf::State::Windowed, settings);
    window.setVerticalSyncEnabled(true);

	const float UPS = 50.f; // Updates per second
	const float TIMESTEP = 1.f / UPS;
	float accumulator = 0.f; // Time accumulator for fixed timestep
    sf::Clock clock;

    Player player(window);
	std::vector<Enemy> enemies;

    unsigned maxEnemies = 5;
	sf::Time timeSinceLastSpawn = sf::seconds(0.f);
	sf::Time spawnInterval = sf::seconds(3.f);

    while (window.isOpen())
    {
		float frameTime = clock.restart().asSeconds();
        accumulator += frameTime;

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        player.handleInput();

        int fixedUpdateCount = 0;

		while (accumulator >= TIMESTEP)
		{
			timeSinceLastSpawn += sf::seconds(frameTime);
			if (timeSinceLastSpawn >= spawnInterval && enemies.size() < maxEnemies)
			{
				enemies.emplace_back();
				timeSinceLastSpawn = sf::seconds(0.f);
			}

			// Update the game state with a fixed timestep

			for (auto& enemy : enemies)
			    enemy.update(TIMESTEP, window, player.getPosition());

			player.update(TIMESTEP, window, enemies);

			// Remove dead enemies
			enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& e) { return e.getHealth() <= 0; }), enemies.end());

			accumulator -= TIMESTEP;
            fixedUpdateCount++;
		}
        
		float alpha = accumulator / TIMESTEP;

        window.clear();

        player.render(alpha, window);
		for (auto& enemy : enemies)
		    enemy.render(alpha, window);

        window.display();
    }
	return 0;
}