#include "Game.hpp"
#include "Utility.hpp"
#include "EnemyFragmentsEffect.hpp"
#include "PowerUpAbsorbEffect.hpp"

#include <iostream>
#include <cmath>

using namespace Utility;

Game::Text::Text(const sf::Font& font, sf::Vector2u windowSize) :
	title(font),
	start(font),
	gameOver(font),
	score(font),
	restart(font),
	textAlpha(0)
{
	title.setFillColor(sf::Color(255, 255, 255, textAlpha));
	title.setCharacterSize(60U);
	title.setString("TOP-DOWN SHOOTER");
	title.setOrigin({ title.getGlobalBounds().size.x / 2.f, title.getGlobalBounds().size.y / 2.f });
	title.setPosition({ windowSize.x / 2.f, windowSize.y / 2.f - 50.f });

	start.setFillColor(sf::Color(255, 255, 255, textAlpha));
	start.setCharacterSize(30U);
	start.setString("Press Enter to start");
	start.setOrigin({ start.getGlobalBounds().size.x / 2.f, start.getGlobalBounds().size.y / 2.f });
	start.setPosition({ windowSize.x / 2.f, windowSize.y / 2.f + 50.f });

	gameOver.setFillColor(sf::Color(255, 255, 255, textAlpha));
	gameOver.setCharacterSize(60U);
	gameOver.setString("GAME OVER");
	gameOver.setOrigin({ gameOver.getGlobalBounds().size.x / 2.f, gameOver.getGlobalBounds().size.y / 2.f });
	gameOver.setPosition({ windowSize.x / 2.f, windowSize.y / 2.f - 80.f });

	score.setFillColor(sf::Color(255, 255, 255, textAlpha));
	score.setCharacterSize(45U);

	restart.setFillColor(sf::Color(255, 255, 255, textAlpha));
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
	maxEnemies(25U),
	//
	screenTransition(WINDOW_SIZE),
	isScreenTransitionClosing(false)
{
	enemies.reserve(maxEnemies);

	// Set up the window
	auto settings = sf::ContextSettings();
	settings.antiAliasingLevel = 8;
	window = sf::RenderWindow(sf::VideoMode(WINDOW_SIZE), "Top-down Shooter", sf::State::Windowed, settings);
	window.setVerticalSyncEnabled(true);

	// Enemy spawn parameters
	enemySpawnParams.intervalMin = sf::seconds(0.75f);
	enemySpawnParams.intervalMax = sf::seconds(1.25f);
	enemySpawnParams.rampUpTime = sf::seconds(180.f);
	enemySpawnParams.intervalCurrent = enemySpawnParams.intervalMax;
	enemySpawnParams.timeSinceLastSpawn = sf::seconds(0.f);

	// Powerup spawn parameters
	powerupSpawnParams.intervalMin = sf::seconds(12.f);
	powerupSpawnParams.intervalMax = sf::seconds(18.f);
	powerupSpawnParams.rampUpTime = sf::seconds(120.f);
	powerupSpawnParams.intervalCurrent = powerupSpawnParams.intervalMax;
	powerupSpawnParams.timeSinceLastSpawn = sf::seconds(0.f);

	// Seed the random number generator
	std::srand(static_cast<unsigned>(std::time(nullptr)));

	soundManager.loadSounds();
	soundManager.volume = 50.f;
	
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
		if (text.textAlpha < 255)
		{
			text.textAlpha += TIMESTEP * 500;
			if (text.textAlpha >= 255)
				text.textAlpha = 255;

			text.title.setFillColor(sf::Color(255, 255, 255, text.textAlpha));
			text.start.setFillColor(sf::Color(255, 255, 255, text.textAlpha));
		}
		if (isKeyReleased(sf::Keyboard::Key::Enter))
		{
			text.textAlpha = 0;
			text.title.setFillColor(sf::Color(255, 255, 255, text.textAlpha));
			text.start.setFillColor(sf::Color(255, 255, 255, text.textAlpha));

			generateBackground();
			screenTransition.start(ScreenTransition::Mode::OPENING, { window.getSize().x / 2.f, window.getSize().y / 2.f });
			gameState = GameState::PLAYING;
			gameClock.start();
			logicClock.start();
		}
	}
	//  Game over state
	else if (gameState == GameState::GAME_OVER)
	{
		if (text.textAlpha < 255)
		{
			text.textAlpha += TIMESTEP * 500;
			if (text.textAlpha >= 255)
				text.textAlpha = 255;

			text.gameOver.setFillColor(sf::Color(255, 255, 255, text.textAlpha));
			text.score.setFillColor(sf::Color(255, 255, 255, text.textAlpha));
			text.restart.setFillColor(sf::Color(255, 255, 255, text.textAlpha));
		}
		if (isKeyReleased(sf::Keyboard::Key::Enter))
		{
			text.textAlpha = 0;
			text.gameOver.setFillColor(sf::Color(255, 255, 255, text.textAlpha));
			text.score.setFillColor(sf::Color(255, 255, 255, text.textAlpha));
			text.restart.setFillColor(sf::Color(255, 255, 255, text.textAlpha));

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
		static int updateCount = 0;
		float frameTime = logicClock.restart().asSeconds();
		accumulator += frameTime;

		// Fixed timestep update
		while (accumulator >= TIMESTEP)
		{
			if (!screenTransition.getIsFinished())
				screenTransition.update(TIMESTEP);

			if (!isScreenTransitionClosing)
			{
				processSpawns();

				// Update powerups
				for (auto it = powerups.begin(); it != powerups.end(); ++it)
				{
					// Update powerups and check for pickup by player
					powerups.at(it - powerups.begin())->update(TIMESTEP);
					processCollisionsWithPlayer(*it);


					// Remove expired powerups
					if ((*it)->getNeedsDeleting())
					{
						it = powerups.erase(it);
						if (it == powerups.end())
							break;
					}
				}

				// Update enemies
				for (auto& enemy : enemies) {
					// Update enemy and add score if it is killed
					player.score += enemy.update(TIMESTEP, window, player.getPosition());
					processCollisionsWithPlayer(enemy);

					if (enemy.getNeedsDeleting())
						effectManager.addEffect(std::make_shared<EnemyFragmentsEffect>(enemy.getPosition(), enemy.getColor()));
				}
				// Remove dead enemies
				unsigned enemyCount = enemies.size();
				enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& e) { return e.getNeedsDeleting(); }), enemies.end());
				if (enemies.size() < enemyCount)
					soundManager.playSound(SoundManager::SoundID::ENEMY_DEATH, 2.25f, 0.1f);

				// Update player
				player.update(TIMESTEP, window, enemies);
				if (player.wasBulletJustFired())
					soundManager.playSound(SoundManager::SoundID::PLAYER_SHOOT, 1.f, 0.15f);
				if (player.hasBulletJustHit())
					soundManager.playSound(SoundManager::SoundID::ENEMY_HIT, 0.5f, 0.1f);
				if (player.getHasPowerUpJustExpired())
					soundManager.playSound(SoundManager::SoundID::POWERUP_EXPIRE, 2.f);

				hud.update(window, player.getLivesCurrent(), player.getLivesMax(), player.score, player.getActivePowerUp());

				// Update effects
				effectManager.update(TIMESTEP, player.getPosition());
			}

			// On player death
			if (player.getLivesCurrent() <= 0)
			{
				if (!isScreenTransitionClosing)
				{
					screenTransition.start(ScreenTransition::Mode::CLOSING, player.getPosition());
					isScreenTransitionClosing = true;
				}
				else if (!screenTransition.getIsActive() && screenTransition.getIsFinished())
				{
					gameState = GameState::GAME_OVER;
					soundManager.playSound(SoundManager::SoundID::GAME_OVER);

					text.score.setString("Score: " + std::to_string(player.score));
					text.score.setOrigin({ text.score.getGlobalBounds().size.x / 2.f, text.score.getGlobalBounds().size.y / 2.f });
					text.score.setPosition({ window.getSize().x / 2.f, window.getSize().y / 2.f - 20.f });

					isScreenTransitionClosing = false;
				}
			}

			accumulator -= TIMESTEP;
		}
		float alpha = accumulator / TIMESTEP;

		if (updateCount % 60 == 0) {
			soundManager.cleanupSounds();
		}
		updateCount++;
	}
}

void Game::render()
{
	window.clear(sf::Color(10, 10, 10));

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
		for (const auto& star : backgroundStars)
			window.draw(star);

		effectManager.render(alpha, window);
		player.render(alpha, window, isDebugModeOn);

		for (auto& enemy : enemies)
			enemy.render(alpha, window, isDebugModeOn);

		for (auto& powerup : powerups)
			powerup->render(alpha, window, isDebugModeOn);

		hud.render(window);

		if (!screenTransition.getIsFinished())
			screenTransition.render(alpha, window);
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
		powerups.emplace_back(std::make_shared<PowerUp>(window.getSize(), player.getLivesCurrent(), player.getLivesMax()));
		powerupSpawnParams.timeSinceLastSpawn = sf::seconds(0.f);

		// Smoothly transition intervalCurrent from max to min linearly
		// After rampUpTime seconds, it will be at intervalMin
		float t = std::min(gameClock.getElapsedTime().asSeconds() / powerupSpawnParams.rampUpTime.asSeconds(), 1.f);
		powerupSpawnParams.intervalCurrent = powerupSpawnParams.intervalMax - (powerupSpawnParams.intervalMax - powerupSpawnParams.intervalMin) * t;

		soundManager.playSound(SoundManager::SoundID::POWERUP_SPAWN, 5.f, 0.05f);
	}
}

void Game::processCollisionsWithPlayer(Enemy& enemy)
{
	if (doesCircleIntersectCircle(player.getPosition(), player.getCollisionRadius(), enemy.getPosition(), enemy.getCollisionRadius())
		&& !player.isInvincible())
	{
		player.loseLife();
		enemy.decreaseHealthBy(enemy.getHealth());
		enemy.scoreValue = 0;
		soundManager.playSound(SoundManager::SoundID::PLAYER_HIT, 2.5f);
	}
}

void Game::processCollisionsWithPlayer(std::shared_ptr<PowerUp> powerUp)
{
	if (doesCircleIntersectCircle(player.getPosition(), player.getCollisionRadius(), powerUp->getPosition(), powerUp->getCollisionRadius())
		&& !powerUp->getIsActivated())
	{
		powerUp->activate();
		player.applyPowerUp(powerUp);
		effectManager.addEffect(std::make_shared<PowerUpAbsorbEffect>(powerUp->getPosition(), player.getPosition(), powerUp->getColor()));
		soundManager.playSound(SoundManager::SoundID::POWERUP_ACTIVATE, 2.f);
	}
}

void Game::generateBackground()
{
	for (unsigned i = 0; i < BACKGROUND_STARS_COUNT; ++i)
	{
		sf::RectangleShape star;

		float size = getRandomNumber(1.f, 5.f);
		star.setSize({ size, size });
		star.setOrigin({ size / 2.f, size / 2.f });

		std::uint8_t colorValue = getRandomNumber(175, 225);
		star.setFillColor(sf::Color(colorValue, colorValue, colorValue, 255U));

		star.setPosition({ getRandomNumber(0.f, static_cast<float>(WINDOW_SIZE.x)), getRandomNumber(0.f, static_cast<float>(WINDOW_SIZE.y)) });
		sf::Angle rotation = sf::degrees(getRandomNumber(0.f, 360.f));
		star.setRotation(rotation);

		backgroundStars.push_back(star);
	}
}