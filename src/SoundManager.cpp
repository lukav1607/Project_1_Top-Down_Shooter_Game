#include "SoundManager.hpp"
#include "Utility.hpp"

#include <iostream>

using namespace Utility;

void SoundManager::loadSounds()
{
	auto load = [&](SoundID soundID, const std::string& filename) 
		{
		soundBuffers[soundID] = std::make_shared<sf::SoundBuffer>(filename);
		};

	load(SoundID::PLAYER_SHOOT, "assets/sounds/player-shoot.wav");
	load(SoundID::PLAYER_HIT, "assets/sounds/player-hit.wav");
	load(SoundID::ENEMY_HIT, "assets/sounds/enemy-hit.wav");
	load(SoundID::ENEMY_DEATH, "assets/sounds/enemy-death.wav");
	load(SoundID::POWERUP_SPAWN, "assets/sounds/powerup-spawn.wav");
	load(SoundID::POWERUP_ACTIVATE, "assets/sounds/powerup-activate.wav");
	load(SoundID::POWERUP_EXPIRE, "assets/sounds/powerup-expire.wav");
	load(SoundID::GAME_OVER, "assets/sounds/game-over.wav");
}

void SoundManager::playSound(SoundID soundID, float volumeMultiplier, float pitchVariancePercentage)
{
	auto it = soundBuffers.find(soundID);
	if (it != soundBuffers.end()) {
		auto sound = std::make_shared<sf::Sound>(*it->second);

		if (pitchVariancePercentage < 0.f || pitchVariancePercentage > 1.f) {
			std::cerr << "Warning: Pitch variance percentage must be between 0 and 1! Pitch variance set to default (0%)" << std::endl;
			pitchVariancePercentage = 0.f;
		}
		if (pitchVariancePercentage != 0.f)
			sound->setPitch(randomPitch(pitchVariancePercentage));

		sound->setVolume(volume * volumeMultiplier);
		sound->play();

		activeSounds.push_back(sound);

		std::cout << "Number of active sounds: " << activeSounds.size() << std::endl;
	}
	else {
		std::cerr << "Error: Sound ID not found!" << std::endl;
	}
}

void SoundManager::cleanupSounds() {
	activeSounds.erase(
		std::remove_if(activeSounds.begin(), activeSounds.end(),
			[](const std::shared_ptr<sf::Sound>& sound) {
				return sound->getStatus() == sf::Sound::Status::Stopped;
			}),
		activeSounds.end());
}