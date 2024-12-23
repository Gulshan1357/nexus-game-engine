# AssetManager

The **AssetManager** stores all the sprites.

The **Render System** utilizes the `AssetManager` by:  
1. Retrieving the appropriate `CSimpleSprite` for an entity using `m_sprites['sprite-name']`.  
2. Setting the sprite's location using `CSimpleSprite->SetLocation(entity.transform)`.  
3. Drawing the sprite with `CSimpleSprite->Draw()`.