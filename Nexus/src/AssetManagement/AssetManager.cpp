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
	for (const auto& sprite : m_sprites)
	{
		delete sprite.second;
	}
	m_sprites.clear();
}

void AssetManager::AddSprite(const std::string& assetId, const std::string& filePath, int columns, int rows)
{
	CSimpleSprite* sprite = App::CreateSprite(filePath.c_str(), columns, rows);

	m_sprites.emplace(assetId, sprite);
}

CSimpleSprite* AssetManager::GetSprite(const std::string& assetId)
{
	return m_sprites[assetId];
}
