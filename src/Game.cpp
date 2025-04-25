#include "Game.hpp"
#include "Utility.hpp"

using namespace Utility;

Game::Text::Text(const sf::Font& font, sf::Vector2u windowSize) :
	title(font),
	start(font),
	gameOver(font),
	score(font),
	restart(font)
{
	title.setFillColor(sf::Color::White);
	title.setCharacterSize(60U);
	title.setString("TOP-DOWN SHOOTER");
	title.setOrigin({ title.getGlobalBounds().size.x / 2.f, title.getGlobalBounds().size.y / 2.f });
	title.setPosition({ windowSize.x / 2.f, windowSize.y / 2.f - 50.f });

	start.setFillColor(sf::Color::White);
	start.setCharacterSize(30U);
	start.setString("Press Enter to start");
	start.setOrigin({ start.getGlobalBounds().size.x / 2.f, start.getGlobalBounds().size.y / 2.f });
	start.setPosition({ windowSize.x / 2.f, windowSize.y / 2.f + 50.f });

	gameOver.setFillColor(sf::Color::White);
	gameOver.setCharacterSize(60U);
	gameOver.setString("GAME OVER");
	gameOver.setOrigin({ gameOver.getGlobalBounds().size.x / 2.f, gameOver.getGlobalBounds().size.y / 2.f });
	gameOver.setPosition({ windowSize.x / 2.f, windowSize.y / 2.f - 80.f });

	score.setFillColor(sf::Color::White);
	score.setCharacterSize(45U);

	restart.setFillColor(sf::Color::White);
	restart.setCharacterSize(30U);
	restart.setString("Press Enter to restart");
	restart.setOrigin({ restart.getGlobalBounds().size.x / 2.f, restart.getGlobalBounds().size.y / 2.f });
	restart.setPosition({ windowSize.x / 2.f, windowSize.y / 2.f + 60.f });
}

Game::Game() :
	// Game loop
	accumulator(0.f),
	alpha(1.f),
	// Game state
	gameState(GameState::TITLE),
	isDebugModeOn(false),	
	// Text
	font("assets/fonts/unispace bd.ttf"),
	text(font, WINDOW_SIZE),
	// Entities
	player(WINDOW_SIZE),
	hud(font, window),
	maxEnemies(50U)
{
	// Set up the window
	auto settings = sf::ContextSettings();
	settings.antiAliasingLevel = 8;
	window = sf::RenderWindow(sf::VideoMode(WINDOW_SIZE), "Top-down Shooter", sf::State::Windowed, settings);
	window.setVerticalSyncEnabled(true);

	// Enemy spawn parameters
	enemySpawnParams.intervalMin = sf::seconds(0.5f);
	enemySpawnParams.intervalMax = sf::seconds(1.25f);
	enemySpawnParams.rampUpTime = sf::seconds(120.f);
	enemySpawnParams.intervalCurrent = enemySpawnParams.intervalMax;
	enemySpawnParams.timeSinceLastSpawn = enemySpawnParams.intervalCurrent;

	// Powerup spawn parameters
	powerupSpawnParams.intervalMin = sf::seconds(15.f);
	powerupSpawnParams.intervalMax = sf::seconds(30.f);
	powerupSpawnParams.rampUpTime = sf::seconds(120.f);
	powerupSpawnParams.intervalCurrent = powerupSpawnParams.intervalMax;
	powerupSpawnParams.timeSinceLastSpawn = powerupSpawnParams.intervalCurrent;

	// Seed the random number generator
	std::srand(static_cast<unsigned>(std::time(nullptr)));

	logicClock.stop();
	gameClock.stop();
}

int Game::run()
{
	while (window.isOpen())
	{
		processInput();
		update();
		render();
	}
	return 0;
}

void Game::processInput()
{
	while (const std::optional event = window.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
			window.close();
	}
	
	if (isKeyReleased(sf::Keyboard::Key::F3))
		isDebugModeOn = !isDebugModeOn;
	else if (isKeyReleased(sf::Keyboard::Key::Escape))
		window.close();

	player.handleInput();
}

void Game::update()
{
	// Title screen state
	if (gameState == GameState::TITLE)
	{
		if (isKeyReleased(sf::Keyboard::Key::Enter))
		{
			gameState = GameState::PLAYING;
			gameClock.start();
			logicClock.start();
		}
	}
	//  Game over state
	else if (gameState == GameState::GAME_OVER)
	{
		if (isKeyReleased(sf::Keyboard::Key::Enter))
		{
			gameState = GameState::TITLE;
			gameClock.reset();
			logicClock.reset();
			player.reset(WINDOW_SIZE);
			enemies.clear();
			powerups.clear();
		}
	}
	// Playing state
	else if (gameState == GameState::PLAYING)
	{
		float frameTime = logicClock.restart().asSeconds();
		accumulator += frameTime;

		// Fixed timestep update
		while (accumulator >= TIMESTEP)
		{
			processSpawns();

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

			// On player death
			if (player.getHealthCurrent() <= 0.f)
			{
				gameState = GameState::GAME_OVER;
				text.score.setString("Score: " + std::to_string(player.score));
				text.score.setOrigin({ text.score.getGlobalBounds().size.x / 2.f, text.score.getGlobalBounds().size.y / 2.f });
				text.score.setPosition({ window.getSize().x / 2.f, window.getSize().y / 2.f - 20.f });
				break;
			}

			accumulator -= TIMESTEP;
		}
		float alpha = accumulator / TIMESTEP;
	}
}

void Game::render()
{
	window.clear();

	// Title screen state
	if (gameState == GameState::TITLE)
	{
		window.draw(text.title);
		window.draw(text.start);
	}
	//  Game over state
	else if	(gameState == GameState::GAME_OVER)
	{
		window.draw(text.gameOver);
		window.draw(text.score);
		window.draw(text.restart);
	}
	//  Playing state
	else if (gameState == GameState::PLAYING)
	{
		player.render(alpha, window, isDebugModeOn);

		for (auto& powerup : powerups)
			powerup.render(alpha, window, isDebugModeOn);

		for (auto& enemy : enemies)
			enemy.render(alpha, window, isDebugModeOn);

		hud.render(window);
	}

	window.display();
}

void Game::processSpawns()
{
	// Enemy spawn logic
	enemySpawnParams.timeSinceLastSpawn += sf::seconds(TIMESTEP);
	if (enemySpawnParams.timeSinceLastSpawn >= enemySpawnParams.intervalCurrent && enemies.size() < maxEnemies)
	{
		enemies.emplace_back();
		enemies.back().initTimeBasedModifiers(gameClock.getElapsedTime());
		enemySpawnParams.timeSinceLastSpawn = sf::seconds(0.f);

		// Smoothly transition intervalCurrent from max to min linearly
		// After rampUpTime seconds, it will be at intervalMin
		float t = std::min(gameClock.getElapsedTime().asSeconds() / enemySpawnParams.rampUpTime.asSeconds(), 1.f);
		enemySpawnParams.intervalCurrent = enemySpawnParams.intervalMax - (enemySpawnParams.intervalMax - enemySpawnParams.intervalMin) * t;
	}

	// Powerup spawn logic
	powerupSpawnParams.timeSinceLastSpawn += sf::seconds(TIMESTEP);
	if (powerupSpawnParams.timeSinceLastSpawn >= powerupSpawnParams.intervalCurrent)
	{
		powerups.emplace_back(window.getSize());
		powerupSpawnParams.timeSinceLastSpawn = sf::seconds(0.f);

		// Smoothly transition intervalCurrent from max to min linearly
		// After rampUpTime seconds, it will be at intervalMin
		float t = std::min(gameClock.getElapsedTime().asSeconds() / powerupSpawnParams.rampUpTime.asSeconds(), 1.f);
		powerupSpawnParams.intervalCurrent = powerupSpawnParams.intervalMax - (powerupSpawnParams.intervalMax - powerupSpawnParams.intervalMin) * t;
	}
}