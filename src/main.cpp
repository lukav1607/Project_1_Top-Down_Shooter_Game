#include <SFML/Graphics.hpp>
#include "Player.hpp"

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
			// Update the game state with a fixed timestep
			player.update(TIMESTEP, window);
			accumulator -= TIMESTEP;
            fixedUpdateCount++;
		}
        
		float alpha = accumulator / TIMESTEP;

        window.clear();
        player.draw(alpha, window);
        window.display();
    }
	return 0;
}