#include "stdafx.h"
#include "AssetManager.h"

#include "src/Utils/Logger.h"

AssetManager::AssetManager()
{
	Logger::Log("AssetManager constructor called!");
}

AssetManager::~AssetManager()
{
	ClearAssets();
	Logger::Log("AssetManager destructor called!");
}

void AssetManager::ClearAssets()
{
	for (const auto& [spriteName, sprite] : m_sprites)
	{
		delete sprite;
	}
	m_sprites.clear();
}

void AssetManager::AddSprite(const std::string& spriteId, const std::string& filePath, const int columns, const int rows)
{
	CSimpleSprite* sprite = App::CreateSprite(filePath.c_str(), columns, rows);

	m_sprites.emplace(spriteId, sprite);
}

CSimpleSprite* AssetManager::GetSprite(const std::string& spriteId)
{
	return m_sprites[spriteId];
}

float AssetManager::GetSpriteWidth(const std::string& spriteId)
{
	return GetSprite(spriteId)->GetWidth();
}

float AssetManager::GetSpriteHeight(const std::string& spriteId)
{
	return GetSprite(spriteId)->GetHeight();
}

void AssetManager::CreateAnimation(const std::string& spriteId, const unsigned int id, const float speed, const std::vector<int>& frames)
{
	GetSprite(spriteId)->CreateAnimation(id, speed, frames);
}
