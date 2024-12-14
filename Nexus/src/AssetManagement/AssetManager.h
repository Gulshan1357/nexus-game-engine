#pragma once

#include <string>
#include <map>

#include "../App/app.h"

class AssetManager
{
public:
	AssetManager();
	~AssetManager();

	void ClearAssets();

	void AddSprite(const std::string& spriteId, const std::string& filePath, int columns, int rows);
	CSimpleSprite* GetSprite(const std::string& spriteId);

	float GetSpriteWidth(const std::string& spriteId);
	float GetSpriteHeight(const std::string& spriteId);

	void CreateAnimation(const std::string& spriteId, const unsigned int id, const float speed, const std::vector<int>& frames);

private:
	std::map<std::string, CSimpleSprite*> m_sprites;
};