#pragma once

#include <unordered_map>
#include <SFML/Audio.hpp>

class SoundManager
{
public:
	enum class SoundID
	{
		PLAYER_SHOOT,
		PLAYER_HIT,
		ENEMY_HIT,
		ENEMY_DEATH,
		POWERUP_SPAWN,
		POWERUP_PICKUP,
		GAME_OVER
	};

	void loadSounds();

	// Remove any sounds that are no longer playing
	void cleanupSounds();

	// Play a sound with a random pitch variation. Note: pitchVariancePercentage 0.15f == +/- 15% variation
	void playSound(SoundID soundID, float pitchVariancePercentage = 0.f);

	float volume = 100.f;

private:
	std::unordered_map<SoundID, std::shared_ptr<sf::SoundBuffer>> soundBuffers;
	std::vector<std::shared_ptr<sf::Sound>> activeSounds;
};