# AssetManager

Rather than creating a new CSimpleSprite for each entity, I decided to create a CSimpleSprite for each sprite and then the render system can get CSimpleSprite for an entity based on m_sprites['sprite-name'], set the CSimpleSprite->Setlocation(entity.transform) and CSimpleSprite->Draw();
