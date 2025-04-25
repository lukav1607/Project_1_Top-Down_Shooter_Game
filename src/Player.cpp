#include "Player.hpp"
#include "Utility.hpp"

#include <cmath>
#include <iostream>

using namespace Utility;

Player::Player(sf::Vector2u windowSize) :
	score(0),
	// Shape
	shapeSize(80.f),
	shapeColor(sf::Color(120, 255, 255)),
	offset(shapeSize / std::sqrt(3.f)),
	collisionRadius(shapeSize / 2.25f),
	// Movement
	positionCurrent(windowSize.x / 2.f, windowSize.y / 2.f),
	positionPrevious(positionCurrent),
	velocity(0.f, 0.f),
	direction(0.f, 0.f),
	dampingFactor(0.95f),
	// Rotation
	angleCurrent(sf::degrees(0.f)),
	anglePrevious(angleCurrent),
	rotationSpeed(5.f),
	// Health
	livesMax(5U),
	livesCurrent(livesMax),
	iFramesTimer(sf::seconds(0.f)),
	// Shooting
	isShooting(false),
	timeSinceLastShot(sf::seconds(0.f))
{
	// Initialize the shape
	shape.setPointCount(3);
	float height = std::sqrt(3.f) / 2.f * shapeSize;
	shape.setPoint(0, { height / 3.f * 2.f, 0.f });			// Right point (facing mouse cursor)
	shape.setPoint(1, { -height / 3.f, -shapeSize / 2.f }); // Bottom left point
	shape.setPoint(2, { -height / 3.f, shapeSize / 2.f });  // Bottom right point
	shape.setOrigin((shape.getPoint(0) + shape.getPoint(1) + shape.getPoint(2)) / 3.f);
	shape.setPosition(positionCurrent);
	shape.setFillColor(shapeColor);

	buffedStats =
	{
		sf::seconds(0.095f), // Fire rate
		2,				     // Bullet damage
		0.8f,			     // Bullet speed multiplier
		1.25f,			     // Bullet size multiplier
		750.f,			     // Max speed
		1250.f 			     // Acceleration
	};
}

void Player::reset(sf::Vector2u windowSize)
{
	// Reset player state
	positionCurrent = { windowSize.x / 2.f, windowSize.y / 2.f };
	positionPrevious = positionCurrent;
	velocity = { 0.f, 0.f };
	direction = { 0.f, 0.f };
	angleCurrent = sf::degrees(0.f);
	anglePrevious = angleCurrent;
	livesCurrent = livesMax;
	iFramesTimer = sf::seconds(0.f);
	bullets.clear();
	score = 0;
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

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		isShooting = true;
	else
		isShooting = false;
}

void Player::update(float deltaTime, const sf::RenderWindow& window, std::vector<Enemy>& enemies)
{
	if (iFramesTimer > sf::seconds(0.f))
		iFramesTimer -= sf::seconds(deltaTime);

	updateMovement(deltaTime);
	updateRotation(deltaTime, window);
	updateShooting(deltaTime, window, enemies);
	keepWithinWindow(window);
	managePowerUpExpiration();

	if (livesCurrent <= 0)
		livesCurrent = 0;
}

void Player::render(float alpha, sf::RenderWindow& window, bool isDebugModeOn)
{
	for (auto& bullet : bullets)
		bullet.render(alpha, window, isDebugModeOn);

	// Interpolate between the previous and current position for smooth rendering
	shape.setPosition(interpolate(positionPrevious, positionCurrent, alpha));

	// Interpolate between the previous and current angle for smooth rendering
	shape.setRotation(interpolate(anglePrevious, angleCurrent, alpha));

	// Set the color of the shape based on the invincibility state
	if (isInvincible())
	{
		sf::Color color = shape.getFillColor();
		color.a = static_cast<std::uint8_t>(std::abs(std::sin(iFramesTimer.asSeconds() * 10.f)) * 255);
		shape.setFillColor(color);
	}
	else
	{
		shape.setFillColor(shapeColor);
	}

	window.draw(shape);

	if (isDebugModeOn)
	{
		sf::CircleShape debug(collisionRadius);
		debug.setFillColor(sf::Color::Transparent);
		debug.setOutlineColor(sf::Color::Magenta);
		debug.setOutlineThickness(2.f);
		debug.setOrigin({ collisionRadius, collisionRadius });
		debug.setPosition({ positionCurrent.x, positionCurrent.y });
		window.draw(debug);
	}
}

void Player::launchBullet()
{
	// Get the tip of the shape (the point facing the mouse cursor) in global coordinates
	sf::Vector2f shapeTip = shape.getTransform().transformPoint(shape.getPoint(0));

	// Create a new bullet and add it to the bullets vector
	bullets.emplace_back(shapeTip, angleCurrent, currentStats.bulletDamage, bulletColor, currentStats.bulletSpeedMultiplier, currentStats.bulletSizeMultiplier);

	timeSinceLastShot = sf::seconds(0.f); // Reset the shot timer
}

void Player::managePowerUpExpiration()
{
	//for (auto it = activePowerUps.begin(); it != activePowerUps.end(); ++it)
	//{
		// Remove expired power-ups
		if (activePowerUp != nullptr && /*(*it)->getNeedsDeleting()*/activePowerUp->getNeedsDeleting())
		{
			// Reset the player's stats based on the power-up type
			switch (/*(*it)->getType()*/activePowerUp->getType())
			{
			case PowerUp::Type::DAMAGE:
				currentStats.bulletDamage = DEFAULT_STATS.bulletDamage;
				currentStats.bulletSizeMultiplier = DEFAULT_STATS.bulletSizeMultiplier;
				currentStats.bulletSpeedMultiplier = DEFAULT_STATS.bulletSpeedMultiplier;
				std::cout << "Damage power-up expired!" << std::endl;
				break;
			case PowerUp::Type::FIRE_RATE:
				currentStats.fireRate = DEFAULT_STATS.fireRate;
				std::cout << "Fire rate power-up expired!" << std::endl;
				break;
			case PowerUp::Type::SPEED:
				currentStats.maxSpeed = DEFAULT_STATS.maxSpeed;
				currentStats.acceleration = DEFAULT_STATS.acceleration;
				std::cout << "Speed power-up expired!" << std::endl;
				break;
			case PowerUp::Type::LIFE:
				break; // Life power-ups don't expire
			}

			activePowerUp = nullptr;
			// Remove the power-up from the activePowerUps vector
			//it = activePowerUps.erase(it);
			//if (it == activePowerUps.end()) 
			//	break;
		//}
	}
}

void Player::loseLife(unsigned amount)
{
	if (iFramesTimer <= sf::seconds(0.f))
	{
		livesCurrent -= amount;
		iFramesTimer = IFRAMES; // Reset the invincibility frames timer
	}
}

void Player::applyPowerUp(std::shared_ptr<PowerUp> powerUp)
{
	activePowerUp = powerUp;

	switch (activePowerUp->getType())
	{
	case PowerUp::Type::DAMAGE:
		currentStats.bulletDamage = buffedStats.bulletDamage;
		currentStats.bulletSizeMultiplier = buffedStats.bulletSizeMultiplier;
		currentStats.bulletSpeedMultiplier = buffedStats.bulletSpeedMultiplier;
		std::cout << "Damage power-up applied!" << std::endl;
		break;

	case PowerUp::Type::FIRE_RATE:
		currentStats.fireRate = buffedStats.fireRate;
		std::cout << "Fire rate power-up applied!" << std::endl;
		break;

	case PowerUp::Type::SPEED:
		currentStats.maxSpeed = buffedStats.maxSpeed;
		currentStats.acceleration = buffedStats.acceleration;
		std::cout << "Speed power-up applied!" << std::endl;
		break;

	case PowerUp::Type::LIFE:
		if (livesCurrent < livesMax)
			livesCurrent += 1;
		std::cout << "Life power-up applied!" << std::endl;
		break;
	}
}

void Player::updateMovement(float deltaTime)
{
	// Update direction and velocity
	if (direction.x != 0.f || direction.y != 0.f)
	{
		direction = normalize(direction);
		velocity += direction * currentStats.acceleration * deltaTime;
	}

	// Clamp the velocity to the max speed
	float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
	if (speed > currentStats.maxSpeed)
		velocity = (velocity / speed) * currentStats.maxSpeed;

	// Apply damping (simple friction)
	velocity *= std::pow(dampingFactor, deltaTime * 60.f); // Assuming 60 FPS

	// Move shape
	positionPrevious = positionCurrent;
	positionCurrent += velocity * deltaTime;
}

void Player::updateRotation(float deltaTime, const sf::RenderWindow& window)
{
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
}

void Player::updateShooting(float deltaTime, const sf::RenderWindow& window, std::vector<Enemy>& enemies)
{
	// Shooting Logic
	timeSinceLastShot += sf::seconds(deltaTime);
	if (isShooting && timeSinceLastShot >= currentStats.fireRate)
		launchBullet();

	// Remove off screen bullets
	bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return b.getIsMarkedForDeletion(); }), bullets.end());

	// Update bullets and detect collisions with enemies
	//for (auto& bullet : bullets)
	for (auto it = bullets.begin(); it != bullets.end(); ++it)
	{
		Bullet& bullet = *it;

		// Check if the bullet is marked for deletion and remove it
		if (bullet.getIsMarkedForDeletion())
		{
			it = bullets.erase(it);
			if (it == bullets.end())
				break;
			continue; // Skip the rest of the loop if the bullet is marked for deletion
		}

		// Update bullet position and check for collisions with enemies
		bullet.update(deltaTime, window);
		for (auto& enemy : enemies)
		{
			if (doesCircleIntersectCircle(bullet.getPosition(), bullet.getCollisionRadius(), enemy.getPosition(), enemy.getCollisionRadius()))
			{
				enemy.decreaseHealthBy(bullet.getDamage());
				bullet.markForDeletion();
				break; // Exit the loop after a hit to avoid multiple hits with the same bullet
			}
		}
	}
}

void Player::keepWithinWindow(const sf::RenderWindow& window)
{
	// Keep the player within the window bounds
	if (positionCurrent.x + collisionRadius > window.getSize().x)
		positionCurrent.x = window.getSize().x - collisionRadius;
	else if (positionCurrent.x - collisionRadius < 0.f)
		positionCurrent.x = collisionRadius;
	if (positionCurrent.y + collisionRadius > window.getSize().y)
		positionCurrent.y = window.getSize().y - collisionRadius;
	else if (positionCurrent.y - collisionRadius < 0.f)
		positionCurrent.y = collisionRadius;
}