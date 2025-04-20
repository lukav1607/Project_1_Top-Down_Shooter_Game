#include <SFML/Graphics.hpp>
#include "Player.hpp"

int main() {
    auto window = sf::RenderWindow(sf::VideoMode({ 1200u, 1200u }), "Top Down Shooter");
    window.setFramerateLimit(60);

    sf::Clock clock;
    Player player(window);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
			}
        }

        auto deltaTime = clock.restart().asSeconds();

        player.handleInput();
        player.update(deltaTime, window);

        window.clear();
        player.draw(window);
        window.display();
    }
	return 0;
}