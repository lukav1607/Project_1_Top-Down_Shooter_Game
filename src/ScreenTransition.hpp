#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class ScreenTransition
{
public:
	enum class Mode
	{
		OPENING,
		CLOSING
	};

	ScreenTransition(sf::Vector2u windowSize);

	void start(Mode mode, sf::Vector2f position);
	void update(float deltaTime);
	void render(float alpha, sf::RenderWindow& window);

	inline bool getIsFinished() const { return isFinished; }
	inline bool getIsActive() const { return isActive; }

private:
	struct Rectangle
	{
		sf::RectangleShape shape;
		sf::Vector2f positionCurrent;
		sf::Vector2f positionPrevious;
		sf::Vector2f direction;
	};
	std::vector<Rectangle> rectangles;

	Mode mode;

	sf::Vector2f center;
	sf::Vector2u windowSize;

	float speed;
	bool isFinished;
	bool isActive;
};