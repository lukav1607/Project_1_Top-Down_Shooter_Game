#include "Player.hpp"
#include "Utility.hpp"

#include <cmath>
//#include <iostream>

using Utility::normalize;
using Utility::interpolate;
using Utility::isKeyReleased;

Player::Player(const sf::RenderWindow& window) :
	// Shape
	shapeSize(50.f),
	offset(shapeSize / std::sqrt(3.f)),
	collisionRadius(shapeSize / 2.25f),
	// Movement
	positionCurrent(window.getSize().x / 2.f, window.getSize().y / 2.f),
	positionPrevious(positionCurrent),
	velocity(0.f, 0.f),
	direction(0.f, 0.f),
	acceleration(1000.f),
	maxSpeed(500.f),
	dampingFactor(0.95f),
	// Rotation
	angleCurrent(sf::degrees(0.f)),
	anglePrevious(angleCurrent),
	rotationSpeed(5.f),
	// Health
	healthMax(100.f),
	health(healthMax),
	// Shooting
	isShooting(false),
	timeSinceLastShot(sf::seconds(0.f)),
	fireRate(sf::seconds(0.1f)),
	bulletSpeedMultiplier(1.f),
	bulletSizeMultiplier(1.f)
{
	// Initialize the shape
	shape.setPointCount(3);
	float height = std::sqrt(3.f) / 2.f * shapeSize;
	shape.setPoint(0, { height / 3.f * 2.f, 0.f });			// Right point (facing mouse cursor)
	shape.setPoint(1, { -height / 3.f, -shapeSize / 2.f }); // Bottom left point
	shape.setPoint(2, { -height / 3.f, shapeSize / 2.f });  // Bottom right point
	shape.setOrigin((shape.getPoint(0) + shape.getPoint(1) + shape.getPoint(2)) / 3.f);
	shape.setPosition(positionCurrent);
	shape.setFillColor(sf::Color::Green);
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
	updateMovement(deltaTime);
	updateRotation(deltaTime, window);
	updateShooting(deltaTime, window, enemies);	
}

void Player::render(float alpha, sf::RenderWindow& window, bool isDebugModeOn)
{
	for (auto& bullet : bullets)
		bullet.render(alpha, window, isDebugModeOn);

	// Interpolate between the previous and current position for smooth rendering
	shape.setPosition(interpolate(positionPrevious, positionCurrent, alpha));

	// Interpolate between the previous and current angle for smooth rendering
	shape.setRotation(interpolate(anglePrevious, angleCurrent, alpha));

	window.draw(shape);

	if (isDebugModeOn)
	{
		sf::CircleShape debug(collisionRadius);
		debug.setFillColor(sf::Color::Transparent);
		debug.setOutlineColor(sf::Color::Magenta);
		debug.setOutlineThickness(1.f);
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
	bullets.emplace_back(shapeTip, angleCurrent, bulletColor, bulletSpeedMultiplier, bulletSizeMultiplier);

	timeSinceLastShot = sf::seconds(0.f); // Reset the shot timer
}

void Player::updateMovement(float deltaTime)
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
		velocity = (velocity / speed) * maxSpeed;

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
	if (isShooting && timeSinceLastShot >= fireRate)
		launchBullet();

	// Remove off screen bullets
	bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return b.getIsMarkedForDeletion(); }), bullets.end());

	// Update bullets and detect collisions with enemies
	for (auto& bullet : bullets)
	{
		bullet.update(deltaTime, window);
		for (auto& enemy : enemies)
		{
			if (enemy.getGlobalBounds().findIntersection(bullet.getGlobalBounds()))
			{
				enemy.decreaseHealthBy(bullet.getDamage());
				bullet.markForDeletion();
			}
		}
	}
}