#include "stdafx.h"
#include "AudioManager.h"

#include <stdexcept>

#include "AudioAsset.h"
#include "App/app.h"
#include "src/Utils/Logger.h"

AudioManager::AudioManager()
{
	Logger::Log("AudioManager constructor called!");
}

AudioManager::~AudioManager()
{
	// ClearSounds();
	Logger::Log("AudioManager destructor called!");
}

void AudioManager::ClearAudioMap()
{
	m_sounds.clear();
}

void AudioManager::AddAudio(const std::string& soundId, const std::string& fileName)
{
	if (!IsAudioFileNameValid(fileName))
	{
		Logger::Err("Invalid audio file: " + std::string(fileName));
	}
	m_sounds.emplace(soundId, AudioAsset(fileName));
}

void AudioManager::PlayAudio(const std::string& soundId, bool looping)
{
	const auto it = m_sounds.find(soundId);
	if (it != m_sounds.end())
	{
		App::PlaySound(it->second.GetFileName().c_str(), looping);
	}
	else
	{
		Logger::Log("PlayAudio(): Sound ID not found: " + soundId);
	}
}

void AudioManager::StopAudio(const std::string& soundId)
{
	auto it = m_sounds.find(soundId);
	if (it != m_sounds.end())
	{
		App::StopSound(it->second.GetFileName().c_str());
	}
	else
	{
		Logger::Log("StopAudio(): Sound ID not found: " + soundId);
	}
}

bool AudioManager::IsAudioPlaying(const std::string& soundId)
{
	auto it = m_sounds.find(soundId);
	if (it != m_sounds.end())
	{
		return App::IsSoundPlaying(it->second.GetFileName().c_str());
	}
	else
	{
		Logger::Log("IsAudioPlaying(): Sound ID not found: " + soundId);
		return false;
	}
}

bool AudioManager::IsAudioFileNameValid(const std::string& fileName)
{
	static const  std::string audio_suffix = ".wav";

	if (fileName.size() <= audio_suffix.size())
	{
		return false;
	}
	return fileName.substr(fileName.size() - audio_suffix.size()) == audio_suffix;
}
