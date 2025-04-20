#include "Player.hpp"

#include <cmath>

Player::Player(const sf::RenderWindow& const window) {
	shape.setSize({ 50.f, 50.f });
	shape.setOrigin(shape.getSize() / 2.f);
	shape.setFillColor(sf::Color::Green);
	shape.setPosition({ window.getSize().x / 2.f, window.getSize().y / 2.f });
	velocity = { 0.f, 0.f };
	direction = { 0.f, 0.f };
	acceleration = 1000.f;
	maxSpeed = 300.f;
	dampingFactor = 0.9f;
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

	// Rotate to face mouse
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	sf::Vector2f worldMousePos = window.mapPixelToCoords(mousePos);
	sf::Vector2f direction = worldMousePos - shape.getPosition();
	sf::Angle angle = sf::radians(std::atan2(direction.y, direction.x));
	shape.setRotation(angle);
}

void Player::draw(sf::RenderWindow& window) {
	window.draw(shape);
}