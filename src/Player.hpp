#pragma once

#include <memory>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "Bullet.hpp"
#include "Enemy.hpp"
#include "PowerUp.hpp"

class Player
{
public:
	Player(sf::Vector2u windowSize);

	void reset(sf::Vector2u windowSize);
	void handleInput();
	void update(float deltaTime, const sf::RenderWindow& window, std::vector<Enemy>& enemies);
	void render(float alpha, sf::RenderWindow& window, bool isDebugModeOn);

	void loseLife(unsigned amount = 1);
	void applyPowerUp(std::shared_ptr<PowerUp> powerUp);

	inline int getLivesCurrent() const { return livesCurrent; }
	inline unsigned getLivesMax() const { return livesMax; }
	inline sf::Vector2f getPosition() const { return positionCurrent; }
	inline float getCollisionRadius() const { return collisionRadius; }

	inline bool isInvincible() const { return iFramesTimer > sf::seconds(0.f); }

	int score;

private:
	void launchBullet();
	void managePowerUpExpiration();

	void updateMovement(float deltaTime);
	void updateRotation(float deltaTime, const sf::RenderWindow& window);
	void updateShooting(float deltaTime, const sf::RenderWindow& window, std::vector<Enemy>& enemies);
	void keepWithinWindow(const sf::RenderWindow& window);

	sf::ConvexShape shape;
	sf::Color shapeColor;
	float shapeSize;
	float offset; // Offset from center to the tip of the player shape
	float collisionRadius;

	sf::Vector2f positionCurrent;
	sf::Vector2f positionPrevious;
	sf::Vector2f velocity;
	sf::Vector2f direction;
	float dampingFactor; // A factor to reduce velocity over time (simulates friction)

	sf::Angle angleCurrent;
	sf::Angle anglePrevious;
	float rotationSpeed;

	unsigned livesMax;
	int livesCurrent;
	const sf::Time IFRAMES = sf::seconds(0.75f);
	sf::Time iFramesTimer;

	std::vector<Bullet> bullets;
	sf::Color bulletColor = sf::Color(255, 230, 100);
	bool isShooting;
	sf::Time timeSinceLastShot;

	struct BuffableStats
	{
		sf::Time fireRate = sf::seconds(0.15f);
		int bulletDamage = 1;
		float bulletSpeedMultiplier = 1.f;
		float bulletSizeMultiplier = 1.f;
		float maxSpeed = 500.f;
		float acceleration = 1000.f;
	};
	const BuffableStats DEFAULT_STATS;
	BuffableStats currentStats;
	BuffableStats buffedStats;

	std::vector<std::shared_ptr<PowerUp>> activePowerUps;
};