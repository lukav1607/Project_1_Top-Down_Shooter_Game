#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "Bullet.hpp"
#include "Enemy.hpp"

class Player
{
public:
	Player(const sf::RenderWindow& window);

	void handleInput();
	void update(float deltaTime, const sf::RenderWindow& window, std::vector<Enemy>& enemies);
	void render(float alpha, sf::RenderWindow& window);

	inline sf::Vector2f getPosition() const { return positionCurrent; }

	enum class FireMode
	{
		SemiAuto,
		FullAuto,
		Burst,
		Charge
	};

private:
	void launchBullet();
	void cycleFireModes();

	bool wasMousePressedLastFrame;

	sf::Vector2f positionCurrent;
	sf::Vector2f positionPrevious;

	sf::ConvexShape shape;
	float shapeSize;
	float offset; // Offset from center to the tip of the player shape

	sf::Vector2f velocity;
	sf::Vector2f direction;
	float acceleration;
	float maxSpeed;
	float dampingFactor; // A factor to reduce velocity over time (simulates friction)

	sf::Angle angleCurrent;
	sf::Angle anglePrevious;
	float rotationSpeed;

	unsigned healthMax;
	int health;

	float bulletSpeedMultiplier;
	float bulletSizeMultiplier;

	std::vector<Bullet> bullets;
	sf::Color bulletColor = sf::Color::Green;
	bool isShooting;
	FireMode fireMode;

	sf::Time fireRateAuto;
	sf::Time timeSinceLastShot;

	int shotsQueued; // Used for semi-auto mode and burst mode
	int burstSize;
	sf::Time fireRateBurst;
	sf::Time timeBetweenBursts;
	sf::Time timeSinceLastBurst;

	bool isCharging = false;
	sf::Time chargeTime;
	sf::Time chargeTimeMin;
	sf::Time chargeTimeMax;
	float bulletSpeedMultMinCharge;
	float bulletSpeedMultMaxCharge;
	float bulletSizeMultMinCharge;
	float bulletSizeMultMaxCharge;
};