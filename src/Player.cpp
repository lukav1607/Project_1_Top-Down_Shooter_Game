#include "Player.hpp"
#include "Utility.hpp"

#include <cmath>
#include <iostream>

using Utility::normalize;
using Utility::interpolate;
using Utility::isKeyReleased;

Player::Player(const sf::RenderWindow& window)
{
	wasMousePressedLastFrame = false;

	shape.setPointCount(3);
	shapeSize = 50.f;
	offset = shapeSize / std::sqrt(3.f);
	float height = std::sqrt(3.f) / 2.f * shapeSize;
	shape.setPoint(0, { height / 3.f * 2.f, 0.f }); // Right point (facing mouse cursor)
	shape.setPoint(1, { -height / 3.f, -shapeSize / 2.f }); // Bottom left point
	shape.setPoint(2, { -height / 3.f, shapeSize / 2.f }); // Bottom right point
	shape.setOrigin((shape.getPoint(0) + shape.getPoint(1) + shape.getPoint(2)) / 3.f);
	shape.setFillColor(sf::Color::Green);

	positionCurrent = { window.getSize().x / 2.f, window.getSize().y / 2.f };
	positionPrevious = positionCurrent;
	shape.setPosition(positionCurrent);

	velocity = { 0.f, 0.f };
	direction = { 0.f, 0.f };
	acceleration = 1000.f;
	maxSpeed = 500.f;
	dampingFactor = 0.95f;

	angleCurrent = sf::degrees(0.f);
	anglePrevious = angleCurrent;
	
	rotationSpeed = 5.f;

	healthMax = 1000;
	health = healthMax;

	fireMode = FireMode::Charge;
	bulletSpeedMultiplier = 1.f;
	bulletSizeMultiplier = 1.f;
	cycleFireModes();

	isShooting = false;
	shotsQueued = 0;
	fireRateAuto = sf::seconds(0.075f);
	timeSinceLastShot = sf::seconds(0.f);
	burstSize = 5;
	fireRateBurst = sf::seconds(0.033f);
	timeBetweenBursts = sf::seconds(0.33f);
	timeSinceLastBurst = timeBetweenBursts;
	isCharging = false;
	chargeTime = sf::seconds(0.f);
	chargeTimeMin = sf::seconds(0.5f);
	chargeTimeMax = sf::seconds(2.f);
	bulletSpeedMultMinCharge = 0.75f;
	bulletSpeedMultMaxCharge = 0.3f;
	bulletSizeMultMinCharge = 1.15f;
	bulletSizeMultMaxCharge = 5.f;
}

void Player::handleInput()
{
	direction = { 0.f, 0.f }; // Reset direction each frame

	// Check for key presses and set direction accordingly
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) 
		direction.y -= 1.f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) 
		direction.y += 1.f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) 
		direction.x -= 1.f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) 
		direction.x += 1.f;

	if (isKeyReleased(sf::Keyboard::Key::Space))
		cycleFireModes();

	bool isMousePressedNow = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
	isShooting = false;

	switch (fireMode)
	{
	case FireMode::SemiAuto:
		if (isMousePressedNow && !wasMousePressedLastFrame)
			shotsQueued++;
		break;

	case FireMode::FullAuto:
		if (isMousePressedNow)
			isShooting = true;
		break;

	case FireMode::Burst:
		if (isMousePressedNow && !wasMousePressedLastFrame && shotsQueued <= 0 && timeSinceLastBurst >= timeBetweenBursts)
			shotsQueued = burstSize;
		break;

	case FireMode::Charge:
		if (isMousePressedNow)
		{
			isCharging = true;
		}
		else if (isCharging)
		{
			isCharging = false;
			shotsQueued = 1;
		}
		break;
	}

	wasMousePressedLastFrame = isMousePressedNow; // Store the state of the mouse button for the next frame
}

void Player::update(float deltaTime, const sf::RenderWindow& window, std::vector<Enemy>& enemies)
{
	// Update direction and velocity
	if (direction.x != 0.f || direction.y != 0.f)
	{
		direction = normalize(direction);
		velocity += direction * acceleration * deltaTime;
	}

	// Clamp the velocity to the max speed
	float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
	if (speed > maxSpeed)
	{
		velocity = (velocity / speed) * maxSpeed;
	}

	// Apply damping (simple friction)
	velocity *= std::pow(dampingFactor, deltaTime * 60.f); // Assuming 60 FPS

	// Move shape
	positionPrevious = positionCurrent;
	positionCurrent += velocity * deltaTime;

	// Calculate target angle to the mouse
	sf::Vector2f worldMousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
	sf::Vector2f mouseDirection = worldMousePos - positionCurrent; // Vector from player to mouse
	sf::Angle targetAngle = sf::radians(std::atan2(mouseDirection.y, mouseDirection.x)); // Angle of vector in radians

	// Calculate the difference in degrees between the target angle and the current angle
	sf::Angle angleDifference = sf::degrees(fmodf(targetAngle.asDegrees() - angleCurrent.asDegrees() + 180.f, 360.f) - 180.f);

	// Normalize the angle difference to the range [-180, 180] to avoid issues with wrapping
	if (angleDifference < sf::degrees(-180.f))
		angleDifference += sf::degrees(360.f);
	else if (angleDifference > sf::degrees(180.f))
		angleDifference -= sf::degrees(360.f);

	// Change the current angle towards the target angle whilst respecting the rotation speed
	anglePrevious = angleCurrent;
	angleCurrent += angleDifference * rotationSpeed * deltaTime;

	//if (!isShooting)
	timeSinceLastShot += sf::seconds(deltaTime);

	// Firing Logic
	switch (fireMode)
	{
	case FireMode::SemiAuto:
		if (shotsQueued > 0)
		{
			launchBullet();
			shotsQueued--;
			if (shotsQueued < 0)
				shotsQueued = 0;
		}
		break;

	case FireMode::FullAuto:
		if (isShooting && timeSinceLastShot >= fireRateAuto)
			launchBullet();
		break;

	case FireMode::Burst:
		timeSinceLastBurst += sf::seconds(deltaTime);
		if (shotsQueued > 0)
		{
			if (timeSinceLastShot >= fireRateBurst)
			{
				launchBullet();
				shotsQueued--;

				if (shotsQueued <= 0)
				{
					timeSinceLastBurst = sf::seconds(0.f); // Reset the burst cooldown
					shotsQueued = 0;
				}
			}
		}
		break;

	case FireMode::Charge:
		if (chargeTime < chargeTimeMax)
		{
			chargeTime += sf::seconds(deltaTime); // Track how long the player has been charging
		}
		if (shotsQueued == 1 && chargeTime > chargeTimeMin)
		{
			// Calculate the charge time ratio
			float chargeTimeRatio = std::clamp(chargeTime.asSeconds() / chargeTimeMax.asSeconds(), 0.f, 1.f);
			// Ease the charge time ratio for smoother transitions
			chargeTimeRatio *= chargeTimeRatio;

			bulletSpeedMultiplier = bulletSpeedMultMinCharge + (bulletSpeedMultMaxCharge - bulletSpeedMultMinCharge) * chargeTimeRatio;
			bulletSizeMultiplier = bulletSizeMultMinCharge + (bulletSizeMultMaxCharge - bulletSizeMultMinCharge) * chargeTimeRatio;
			
			launchBullet();
			chargeTime = sf::seconds(0.f);
			shotsQueued = 0; // Reset the shots queued
		}
		break;
	}

	for (auto& bullet : bullets)
	{
		bullet.update(deltaTime, window);
	}
	for (auto& enemy : enemies)
	{
		for (auto& bullet : bullets)
		{
			if (enemy.getGlobalBounds().contains(bullet.getPosition()))
			{
				enemy.decreaseHealthBy(bullet.getDamage());
				bullet.markForDeletion();
				break;
			}
		}
	}

	// Remove off screen bullets
	bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return b.getIsMarkedForDeletion(); }), bullets.end());
}

void Player::render(float alpha, sf::RenderWindow& window)
{
	for (auto& bullet : bullets)
		bullet.render(alpha, window);

	// Interpolate between the previous and current position for smooth rendering
	shape.setPosition(interpolate(positionPrevious, positionCurrent, alpha));

	// Interpolate between the previous and current angle for smooth rendering
	shape.setRotation(interpolate(anglePrevious, angleCurrent, alpha));

	window.draw(shape);
}

void Player::launchBullet()
{
	// Get the tip of the shape (the point facing the mouse cursor) in global coordinates
	sf::Vector2f shapeTip = shape.getTransform().transformPoint(shape.getPoint(0));
	// Create a new bullet and add it to the bullets vector
	bullets.emplace_back(shapeTip, angleCurrent, bulletColor, bulletSpeedMultiplier, bulletSizeMultiplier);

	timeSinceLastShot = sf::seconds(0.f); // Reset the shot timer
}

void Player::cycleFireModes()
{
	// Cycle through the fire modes
	switch (fireMode)
	{
	case FireMode::SemiAuto:
		fireMode = FireMode::FullAuto;
		bulletSpeedMultiplier = 1.15f;
		bulletSizeMultiplier = 0.9f;
		break;
	case FireMode::FullAuto:
		fireMode = FireMode::Burst;
		bulletSpeedMultiplier = 0.9f;
		bulletSizeMultiplier = 1.33f;
		break;
	case FireMode::Burst:
		fireMode = FireMode::Charge;
		//bulletSpeedMultiplier = 0.5f;
		//bulletSizeMultiplier = 1.f;
		break;
	case FireMode::Charge:
		fireMode = FireMode::SemiAuto;
		bulletSpeedMultiplier = 1.1f;
		bulletSizeMultiplier = 1.1f;
		break;
	}
	std::cout << "Fire mode: " << static_cast<int>(fireMode) << std::endl;
}