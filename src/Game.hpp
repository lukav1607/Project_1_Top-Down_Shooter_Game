#pragma once

#include "SFML/Graphics.hpp"

#include "Player.hpp"
#include "HUD.hpp"
#include "Enemy.hpp"
#include "PowerUp.hpp"

class Game
{
public:
	enum class GameState
	{
		TITLE,
		PLAYING,
		GAME_OVER
	};

	Game();

	int run();

private:
	void processInput();
	void update();
	void render();

	void processSpawns();
	void processCollisionsWithPlayer(Enemy& enemy);
	void processCollisionsWithPlayer(std::shared_ptr<PowerUp> powerUp);
		
	sf::RenderWindow window;
	sf::ContextSettings settings;
	const sf::Vector2u WINDOW_SIZE = { 1200U, 1200U };

	sf::Clock logicClock;
	sf::Clock gameClock;

	const float UPS = 60.f; // Updates per second
	const float TIMESTEP = 1.f / UPS;
	float accumulator; // Time accumulator for fixed timestep
	float alpha; // Interpolation factor for rendering

	GameState gameState;
	bool isDebugModeOn;

	sf::Font font;
	struct Text
	{
		Text(const sf::Font& font, sf::Vector2u windowSize);
		sf::Text title;
		sf::Text start;
		sf::Text gameOver;
		sf::Text score;
		sf::Text restart;
	};
	Text text;

	Player player;
	HUD hud;

	struct SpawnParameters
	{
		sf::Time intervalMin;
		sf::Time intervalMax;
		sf::Time intervalCurrent;
		sf::Time rampUpTime;
		sf::Time timeSinceLastSpawn;
	};
	std::vector<Enemy> enemies;
	unsigned maxEnemies;
	SpawnParameters enemySpawnParams;

	std::vector<std::shared_ptr<PowerUp>> powerups;
	SpawnParameters powerupSpawnParams;
	
};