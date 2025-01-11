#pragma once

#include <string>

// Encapsulates the file path of the audio asset
class AudioAsset
{
public:
	explicit AudioAsset(std::string filePath)
		: m_fileName(std::move(filePath))
	{}

	[[nodiscard]] const std::string& GetFileName() const
	{
		return m_fileName;
	}

private:
	std::string m_fileName;
};