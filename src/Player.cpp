#include "Player.hpp"

#include <cmath>
#include <iostream>

Player::Player(const sf::RenderWindow& window)
{
	shape.setPointCount(3);
	shapeSize = 80.f;
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

	isShooting = false;
}

sf::Vector2f Player::normalize(const sf::Vector2f& vector)
{
	// Calculate the length of the vector
	float length = std::sqrt(vector.x * vector.x + vector.y * vector.y);

	// Normalize the vector to a unit vector, avoid division by zero
	if (length != 0.f)
		return vector / length;
	else
		return { 0.f, 0.f };
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

	// Check for shooting
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		isShooting = true;
	else
		isShooting = false;
}

void Player::update(float deltaTime, const sf::RenderWindow& window)
{
	positionPrevious = positionCurrent;
	anglePrevious = angleCurrent;

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
	positionCurrent += velocity * deltaTime;

	// Calculate target angle to the mouse
	sf::Vector2f worldMousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
	sf::Vector2f mouseDirection = worldMousePos - shape.getPosition(); // Vector from player to mouse
	sf::Angle targetAngle = sf::radians(std::atan2(mouseDirection.y, mouseDirection.x)); // Angle of vector in radians
	
	// Calculate the difference in degrees between the target angle and the current angle
	sf::Angle angleDifference = sf::degrees(fmodf(targetAngle.asDegrees() - angleCurrent.asDegrees() + 180.f, 360.f) - 180.f);

	// Normalize the angle difference to the range [-180, 180] to avoid issues with wrapping
	if(angleDifference < sf::degrees(-180.f))
		angleDifference += sf::degrees(360.f);
	else if (angleDifference > sf::degrees(180.f))
		angleDifference -= sf::degrees(360.f);

	// Change the current angle towards the target angle whilst respecting the rotation speed
	angleCurrent += angleDifference * rotationSpeed * deltaTime;

	if (isShooting)
	{
		// Get the tip of the shape (the point facing the mouse cursor) in global coordinates
		sf::Vector2f shapeTip = shape.getTransform().transformPoint(shape.getPoint(0));
		// Create a new bullet and add it to the bullets vector
		bullets.emplace_back(shapeTip, angleCurrent);
	}

	for (auto& bullet : bullets)
		bullet.update(deltaTime, window);	

	// Remove off screen bullets
	bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return b.getIsOffScreen(); }), bullets.end());
}

void Player::draw(float alpha, sf::RenderWindow& window)
{
	// Interpolate between the previous and current position for smooth rendering
	sf::Vector2f positionInterpolated = positionCurrent * alpha + positionPrevious * (1.f - alpha);
	shape.setPosition(positionInterpolated);

	// Interpolate between the previous and current angle for smooth rendering
	sf::Angle angleInterpolated = angleCurrent * alpha + anglePrevious * (1.f - alpha);
	shape.setRotation(angleInterpolated);

	for (auto& bullet : bullets)
		bullet.draw(alpha, window);

	window.draw(shape);

	std::cout << bullets.size() << std::endl;
}