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

	float GetSpriteWidth(std::string spriteId);
	float GetSpriteHeight(std::string spriteId);

	void CreateAnimation(std::string spriteId, const unsigned int id, const float speed, const std::vector<int>& frames);

private:
	std::map<std::string, CSimpleSprite*> m_sprites;
};