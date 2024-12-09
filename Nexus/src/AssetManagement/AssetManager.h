#pragma once

#include <string>
#include <map>

#include "../App/app.h"

// Rather than creating a new CSimpleSprite for each entity, I decided to create a CSimpleSprite for each sprite and the render system can get CSimpleSprite for an entity based on m_sprites['sprite-name'], set the CSimpleSprite->Setlocation(entity.transform) and CSimpleSprite->Draw();

class AssetManager
{
public:
	AssetManager();
	~AssetManager();

	void ClearAssets();
	void AddSprite(const std::string& assetId, const std::string& filePath, int columns, int rows);

	CSimpleSprite* GetSprite(const std::string& assetId);

private:
	std::map<std::string, CSimpleSprite*> m_sprites;
};