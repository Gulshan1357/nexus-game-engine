#pragma once

#include <map>
#include <string>

class AudioAsset;

class AudioManager
{
public:
	AudioManager();
	~AudioManager();

	void ClearAudioMap();

	void AddAudio(const std::string& soundId, const std::string& fileName);
	void PlayAudio(const std::string& soundId, bool looping = false);
	void StopAudio(const std::string& soundId);
	bool IsAudioPlaying(const std::string& soundId);

private:
	std::map<std::string, AudioAsset> m_sounds;

	static bool IsAudioFileNameValid(const std::string& fileName);
};
