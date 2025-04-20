#include "Player.hpp"

#include <cmath>

Player::Player(const sf::RenderWindow& const window) {
	shape.setPointCount(3);
	float width = 50.f;
	float height = std::sqrt(3.f) / 2.f * width;
	//shape.setSize({ 50.f, 50.f });
	shape.setPoint(0, { height / 3.f * 2.f, 0.f }); // Right point (facing mouse cursor)
	shape.setPoint(1, { -height / 3.f, -width / 2.f }); // Bottom left point
	shape.setPoint(2, { -height / 3.f, width / 2.f }); // Bottom right point
	//shape.setOrigin(shape.getSize() / 2.f);
	shape.setOrigin({ 0.f, 0.f });
	//shape.setOrigin({ shape.getLocalBounds().size.x / 2.f, shape.getLocalBounds().size.y / 2.f });
	shape.setFillColor(sf::Color::Green);
	shape.setPosition({ window.getSize().x / 2.f, window.getSize().y / 2.f });
	velocity = { 0.f, 0.f };
	direction = { 0.f, 0.f };
	acceleration = 1000.f;
	maxSpeed = 500.f;
	dampingFactor = 0.95f;
}

sf::Vector2f Player::normalize(const sf::Vector2f& vector) {
	float length = std::sqrt(vector.x * vector.x + vector.y * vector.y);

	if (length != 0.f)
		return vector / length;
	else
		return { 0.f, 0.f };
}

void Player::handleInput() {
	direction = { 0.f, 0.f }; // Reset direction each frame

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) direction.y -= 1.f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) direction.y += 1.f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) direction.x -= 1.f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) direction.x += 1.f;
}

void Player::update(float deltaTime, const sf::RenderWindow& const window) {
	// Update direction and velocity
	if (direction.x != 0.f || direction.y != 0.f) {
		direction = normalize(direction);
		velocity += direction * acceleration * deltaTime;
	}

	// Clamp the velocity to the max speed
	float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
	if (speed > maxSpeed) {
		velocity = (velocity / speed) * maxSpeed;
	}

	// Apply damping (simple friction)
	velocity *= std::pow(dampingFactor, deltaTime * 60.f); // Assuming 60 FPS

	// Move shape
	shape.move(velocity * deltaTime);

	// Calculate target angle towards the mouse
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	sf::Vector2f worldMousePos = window.mapPixelToCoords(mousePos);
	sf::Vector2f mouseDirection = worldMousePos - shape.getPosition();
	sf::Angle targetAngle = sf::radians(std::atan2(mouseDirection.y, mouseDirection.x));
	
	// Smoothly interpolate between the current angle and the target angle
	sf::Angle angleDifference = targetAngle - currentAngle;

	// Normalize the angle difference to the range [-180, 180] to avoid issues with wrapping
	if (currentAngle > sf::degrees(180.f))
		currentAngle -= sf::degrees(360.f);
	else if (currentAngle < sf::degrees(-180.f))
		currentAngle += sf::degrees(360.f);

	// Apply smoothing to the angle change
	currentAngle += angleDifference * rotationSpeed * deltaTime;

	// Apply the angle to the shape
	shape.setRotation(currentAngle);
}

void Player::draw(sf::RenderWindow& window) {
	window.draw(shape);
}