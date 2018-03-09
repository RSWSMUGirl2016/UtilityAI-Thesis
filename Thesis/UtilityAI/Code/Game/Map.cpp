#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtilities.hpp"
#include "Game/Map.hpp"
#include "Game/GameCommon.hpp"

//*******************************************************************
Map::Map()
    : m_mapDefinition(nullptr)
    , m_player(nullptr)
    , m_currentPath(nullptr)
{}

//*******************************************************************
Map::~Map()
{
    delete m_vbos[0];
    m_vbos[0] = nullptr;

    delete m_vbos[1];
    m_vbos[1] = nullptr;

    delete m_vbos[2];
    m_vbos[2] = nullptr;

    delete m_vbos[3];
    m_vbos[3] = nullptr;
}

//*******************************************************************
Map::Map(std::string newMapName)
    : m_mapDefinition(nullptr)
    , m_player(nullptr)
    , m_currentPath(nullptr)
{
    m_mapDefinition = MapDefinition::GetMapDefinition(newMapName);
    m_dimensions = m_mapDefinition->m_generators[0]->m_dimensions;
 

    IntVector2 endingPoint;
    std::vector<Tile>& mapDefinitionTiles = m_mapDefinition->m_tiles;
    for (int tilesIndex = 0; tilesIndex < (int)mapDefinitionTiles.size(); tilesIndex++)
    {
        Tile& currentTile = mapDefinitionTiles[tilesIndex];
        int fillColorIndex = GetRandomIntInRange(0, (int)currentTile.m_tileDefinition->m_fillColors.size() - 1);
        currentTile.m_fillColor = currentTile.m_tileDefinition->m_fillColors[fillColorIndex];
        m_tiles.push_back(currentTile);

        /*if (currentTile.m_occupant != nullptr)
        {
            Actor* currentActor = currentTile.m_occupant;
            if (currentActor->m_name == "player")
            {
                m_player = new Player();
                m_player->ChangeType("player");
                m_player->m_currentMap = this;
                m_player->m_currentTile = &currentTile;
                m_player->m_tileCoords = currentTile.m_tileCoords;
            }
            else
            {
                Actor* actor = new Actor();
                actor->ChangeType(currentActor->m_name);
                actor->m_currentMap = this;
                actor->m_currentTile = &currentTile;
                actor->m_tileCoords = currentTile.m_tileCoords;
                m_entities.push_back(actor);
            }
        }

        if (currentTile.m_feature != nullptr)
        {
            Feature* currentFeature = currentTile.m_feature;
            if (currentFeature->m_name == "front door")
            {
                Feature* feature = new Feature();
                feature->ChangeType("front door");
                feature->m_currentMap = this;
                feature->m_currentTile = &currentTile;
                feature->m_tileCoords = currentTile.m_tileCoords;
                endingPoint = feature->m_tileCoords;
                m_entities.push_back(feature);
            }   
            if (currentFeature->m_name == "pressure plate" && currentFeature->m_subType == "puzzle")
            {
                PuzzleFeature* puzzleFeature = new PuzzleFeature();
                puzzleFeature->ChangeType("pressure plate");
                puzzleFeature->m_currentMap = this;
                puzzleFeature->m_currentTile = &currentTile;
                puzzleFeature->m_tileCoords = currentTile.m_tileCoords;
                endingPoint = puzzleFeature->m_tileCoords;
                m_puzzle.push_back(puzzleFeature);
            }
        }*/
    }

    //IntVector2 startingPoint = m_player->m_tileCoords;
    //m_testPath = GeneratePath(startingPoint, endingPoint, nullptr);
}

//*******************************************************************
void Map::ChangeType(const std::string& mapName)
{
    m_mapDefinition = MapDefinition::GetMapDefinition(mapName);
    m_dimensions = m_mapDefinition->m_generators[0]->m_dimensions;

    IntVector2 endingPoint;
    std::vector<Tile>& mapDefinitionTiles = m_mapDefinition->m_tiles;
    for (int tilesIndex = 0; tilesIndex < (int)mapDefinitionTiles.size(); tilesIndex++)
    {
        Tile& currentTile = mapDefinitionTiles[tilesIndex];
        m_tiles.push_back(currentTile);

        if (currentTile.m_occupant != nullptr)
        {
            Actor* currentActor = currentTile.m_occupant;
            if (currentActor->m_name == "player")
            {
                m_player = new Player();
                m_player->ChangeType("player");
                m_player->m_currentMap = this;
                m_player->m_currentTile = &currentTile;
                m_player->m_tileCoords = currentTile.m_tileCoords;
            }
            else
            {
                Actor* actor = new Actor();
                actor->ChangeType(currentActor->m_name);
                actor->m_currentMap = this;
                actor->m_currentTile = &currentTile;
                actor->m_tileCoords = currentTile.m_tileCoords;
                m_entities.push_back(actor);
            }
        }

        if (currentTile.m_feature != nullptr)
        {
            Feature* currentFeature = currentTile.m_feature;
            if (currentFeature->m_name == "front door")
            {
                Feature* feature = new Feature();
                feature->ChangeType("front door");
                feature->m_currentMap = this;
                feature->m_currentTile = &currentTile;
                feature->m_tileCoords = currentTile.m_tileCoords;
                endingPoint = feature->m_tileCoords;
                m_entities.push_back(feature);
            }
            if (currentFeature->m_name == "pressure plate" && currentFeature->m_subType == "puzzle")
            {
                PuzzleFeature* puzzleFeature = new PuzzleFeature();
                puzzleFeature->ChangeType("pressure plate");
                puzzleFeature->m_currentMap = this;
                puzzleFeature->m_currentTile = &currentTile;
                puzzleFeature->m_tileCoords = currentTile.m_tileCoords;
                endingPoint = puzzleFeature->m_tileCoords;
                m_puzzle.push_back(puzzleFeature);
            }
        }

    }
}

//*******************************************************************
int Map::GetTileIndexForTile(const Tile& tile)
{
    IntVector2 tileCoords = tile.m_tileCoords;
    return (int)(tileCoords.y * m_dimensions.y) + tileCoords.x;
}

//*******************************************************************
Tile* Map::GetTileAtTileCoords(const IntVector2& tileCoords)
{
    int tileIndex = tileCoords.x + (tileCoords.y * m_dimensions.y);
    if ((tileIndex >= (int)m_tiles.size()) || (tileIndex < 0))
        return nullptr;
    Tile* tile = &m_tiles[tileIndex];
    if (tile != nullptr && (tileCoords.x < m_dimensions.y) && (tileCoords.y < m_dimensions.x))
        return tile;
    return nullptr;
}

//*******************************************************************
IntVector2 Map::GetTileCoordinatesForTileIndex(int tileIndex)
{
    int tileX = tileIndex % (int)m_dimensions.y;
    int tileY = tileIndex / (int)m_dimensions.y;
    return IntVector2(tileX, tileY);
}

//*******************************************************************
Tile* Map::GetTileForTileIndex(int tileIndex)
{
    Tile* tile = &m_tiles[tileIndex];
    IntVector2 tileCoords = tile->m_tileCoords;
    if (tile != nullptr && (tileCoords.x < m_dimensions.y) && (tileCoords.y < m_dimensions.x))
        return tile;
    return nullptr;
}

//*******************************************************************
void Map::AddOccupantAtTileCoords(const IntVector2& tileCoords, Actor* actor)
{
    int tileIndex = tileCoords.x + (tileCoords.y * (int)m_dimensions.y);
    if ((tileIndex >= (int)m_tiles.size()) || (tileIndex < 0))
        return;
    Tile* tile = &m_tiles[tileIndex];
    actor->m_tileCoords = tile->m_tileCoords;    
    tile->m_occupant = actor;
    actor->m_currentTile = tile;
}

//*******************************************************************
void Map::RemoveOccupantAtTileCoords(const IntVector2& tileCoords)
{
    int tileIndex = tileCoords.x + (tileCoords.y * (int)m_dimensions.y);
    if ((tileIndex >= (int)m_tiles.size()) || (tileIndex < 0))
        return;
    Tile* tile = &m_tiles[tileIndex];
    tile->m_occupant = nullptr;
}

//*******************************************************************
void Map::RemoveOccupantPermanentlyAtTileCoords(const IntVector2& tileCoords)
{
    int tileIndex = tileCoords.x + (tileCoords.y * (int)m_dimensions.y);
    if ((tileIndex >= (int)m_tiles.size()) || (tileIndex < 0))
        return;
    Tile* tile = &m_tiles[tileIndex];

    if (tile->m_occupant->m_name != "player")
    {
        AddInventoryToTileAndRemoveFromActor(tileCoords, tile->m_occupant->m_inventory);
        m_entities.erase(std::find(m_entities.begin(), m_entities.end(), tile->m_occupant));
    }    
    delete tile->m_occupant;
    tile->m_occupant = nullptr;
}

//*******************************************************************
void Map::AddAIOccupantAtTileCoords(const IntVector2& tileCoords, Agent* actor)
{
    int tileIndex = tileCoords.x + (tileCoords.y * (int)m_dimensions.y);
    if ((tileIndex >= (int)m_tiles.size()) || (tileIndex < 0))
        return;
    Tile* tile = &m_tiles[tileIndex];
    actor->m_tileCoords = tile->m_tileCoords;
    tile->m_aiOccupant = actor;
    actor->m_currentTile = tile;
    actor->m_worldLocation = tile->m_tileCoords;
}

//*******************************************************************
void Map::RemoveAIOccupantAtTileCoords(const IntVector2& tileCoords)
{
    int tileIndex = tileCoords.x + (tileCoords.y * (int)m_dimensions.y);
    if ((tileIndex >= (int)m_tiles.size()) || (tileIndex < 0))
        return;
    Tile* tile = &m_tiles[tileIndex];
    tile->m_aiOccupant = nullptr;
}

//*******************************************************************
void Map::RemoveAIOccupantPermanentlyAtTileCoords(const IntVector2& tileCoords)
{
    int tileIndex = tileCoords.x + (tileCoords.y * (int)m_dimensions.y);
    if ((tileIndex >= (int)m_tiles.size()) || (tileIndex < 0))
        return;
    Tile* tile = &m_tiles[tileIndex];

    if (tile->m_occupant->m_name != "player")
    {
        AddInventoryToTileAndRemoveFromActor(tileCoords, tile->m_occupant->m_inventory);
        m_entities.erase(std::find(m_entities.begin(), m_entities.end(), tile->m_occupant));
    }
    delete tile->m_aiOccupant;
    tile->m_aiOccupant = nullptr;
}

//*******************************************************************
void Map::AddInventoryToTileAndRemoveFromActor(const IntVector2& tileCoords, Inventory& inventory)
{
    int tileIndex = tileCoords.x + (tileCoords.y * (int)m_dimensions.y);
    if ((tileIndex >= (int)m_tiles.size()) || (tileIndex < 0))
        return;
    Tile* tile = &m_tiles[tileIndex];

    tile->m_inventory = inventory;
    for (int inventoryIndex = 0; inventoryIndex < (int)inventory.m_items.size(); inventoryIndex++)
    {
        inventory.m_items[inventoryIndex] = nullptr;
    }
    inventory.m_items.clear();
}

//*******************************************************************
void Map::AddInventoryToActorAndRemoveFromTile(const IntVector2& tileCoords, Actor* actor)
{
    int tileIndex = tileCoords.x + (tileCoords.y * (int)m_dimensions.y);
    if ((tileIndex >= (int)m_tiles.size()) || (tileIndex < 0))
        return;
    Tile* tile = &m_tiles[tileIndex];

    actor->AddLootedItemstoInventory(tile->m_inventory.m_items);

    for (int inventoryIndex = 0; inventoryIndex < (int)tile->m_inventory.m_items.size(); inventoryIndex++)
    {
        tile->m_inventory.m_items[inventoryIndex] = nullptr;
    }
    tile->m_inventory.m_items.clear();
}

//*******************************************************************
void Map::AddInventoryToActorAndRemoveFromTile(const IntVector2& tileCoords, Player* player)
{
    int tileIndex = tileCoords.x + (tileCoords.y * (int)m_dimensions.y);
    if ((tileIndex >= (int)m_tiles.size()) || (tileIndex < 0))
        return;
    Tile* tile = &m_tiles[tileIndex];

    player->AddLootedItemstoInventory(tile->m_inventory.m_items);

    for (int inventoryIndex = 0; inventoryIndex < (int)tile->m_inventory.m_items.size(); inventoryIndex++)
    {
        tile->m_inventory.m_items[inventoryIndex] = nullptr;
    }
    tile->m_inventory.m_items.clear();
}

//*******************************************************************
IntVector2 Map::GetRandomAvailableFloorTile()
{
    int randomX = GetRandomIntInRange(1, (int)m_dimensions.y - 2);
    int randomY = GetRandomIntInRange(1, (int)m_dimensions.x - 2);
    Tile* chosenTile = this->GetTileAtTileCoords(IntVector2(randomX, randomY));
    while (chosenTile->m_tileDefinition->m_type != "grass" || chosenTile->m_occupant != nullptr || chosenTile->m_feature != nullptr)
    {
        randomX = GetRandomIntInRange(1, (int)m_dimensions.y - 2);
        randomY = GetRandomIntInRange(1, (int)m_dimensions.x - 2);
        chosenTile = this->GetTileAtTileCoords(IntVector2(randomX, randomY));
    }
    return IntVector2(randomX, randomY);
}

//*******************************************************************
void Map::AddFeatureAtTileCoords(const IntVector2& tileCoords, Feature* feature)
{
    int tileIndex = tileCoords.x + (tileCoords.y * (int)m_dimensions.y);
    if ((tileIndex >= (int)m_tiles.size()) || (tileIndex < 0))
        return;
    Tile* tile = &m_tiles[tileIndex];
    feature->m_tileCoords = tile->m_tileCoords;
    feature->m_currentMap = this;
    tile->m_feature = feature;
}

//*******************************************************************
void Map::RemoveFeatureAtTileCoords(const IntVector2& tileCoords)
{
    int tileIndex = tileCoords.x + (tileCoords.y * (int)m_dimensions.y);
    if ((tileIndex >= (int)m_tiles.size()) || (tileIndex < 0))
        return;
    Tile* tile = &m_tiles[tileIndex];

    m_entities.erase(std::find(m_entities.begin(), m_entities.end(), tile->m_feature));   
    delete tile->m_occupant;
    tile->m_feature = nullptr;
}

//*******************************************************************
bool Map::IsInBounds(const IntVector2& tileCoords)
{
    if (((tileCoords.x < m_dimensions.y) && (tileCoords.x >= 0)) && ((tileCoords.y < m_dimensions.x) && (tileCoords.y >= 0)))
    {
        return true;
    }
    return false;
}

//*******************************************************************
Vector2 Map::GetWorldCenterForTileCoordinates(const IntVector2 tileCoordinates)
{
    Vector2 centerWorldPosition;
    centerWorldPosition.x = 0.5f + (float)tileCoordinates.x;
    centerWorldPosition.y = 0.5f + (float)tileCoordinates.y;
    return centerWorldPosition;
}

//*******************************************************************
IntVector2 Map::GetTileCoordinatesForWorldPosition(const Vector2& worldPosition) const
{
    IntVector2 tileCoordinates;
    tileCoordinates.x = (int)floor(worldPosition.x);
    tileCoordinates.y = (int)floor(worldPosition.y);
    return tileCoordinates;
}

//*******************************************************************
SIGHT Map::HasLineOfSight(const Vector2& startPosition, const Vector2& endPosition, Actor* usesActorsSight)
{
    Vector2 displacement = endPosition - startPosition;
    displacement *= 0.001f;
    Vector2 currentPosition = startPosition;
    for (int step = 0; step < 1000; step)
    {
        currentPosition += displacement;
        IntVector2 mapTile = GetTileCoordinatesForWorldPosition(currentPosition);
        Tile* tile = GetTileAtTileCoords(mapTile);
        if ((tile->m_tileDefinition != nullptr) && tile->IsSolid())
            return NOT_IN_SIGHT;
        
        if (tile->m_occupant != nullptr && usesActorsSight != nullptr)
        {
            if (tile->m_occupant->m_faction != usesActorsSight->m_faction)
            {
                return ENEMY_IN_SIGHT;
            }
        }
       
    }

    return IN_SIGHT;
}

//*******************************************************************
Vector2 Map::GetLineOfSight(const Vector2& startPosition, const Vector2& endPosition, Actor* usesActorsSight)
{
    Vector2 displacement = endPosition - startPosition;
    displacement *= 0.001f;
    Vector2 currentPosition = startPosition;
    for (int step = 0; step < 1000; step)
    {
        currentPosition += displacement;
        IntVector2 mapTile = GetTileCoordinatesForWorldPosition(currentPosition);
        Tile* tile = GetTileAtTileCoords(mapTile);
        if ((tile->m_tileDefinition != nullptr) && tile->IsSolid())
        {
            return currentPosition;
        }
        if (tile->m_occupant != nullptr && usesActorsSight != nullptr)
        {
            if (tile->m_occupant->m_faction != usesActorsSight->m_faction)
            {
                return currentPosition;
            }
        }
    }

    return endPosition;
}

//*******************************************************************
RayCastResult2D Map::AmanatidesWooRaycast(Vector2 startPosition, Vector2 endPosition)
{
    int tileX = (int)floor(startPosition.x);
    int tileY = (int)floor(startPosition.y);
    IntVector2 tileCoords = IntVector2(tileX, tileY);
    Tile* tile = GetTileAtTileCoords(tileCoords);

    if (tile->IsSolid())
    {
        RayCastResult2D rayCastResult2D = RayCastResult2D();
        rayCastResult2D.m_didImpact = true;
        rayCastResult2D.m_impactPosition = startPosition;
        rayCastResult2D.m_impactTileCoords = tileCoords;
        return rayCastResult2D;
    }

    Vector2 rayDisplacement = endPosition - startPosition;

    float tDeltaX = 1 / abs(rayDisplacement.x);
    int tileStepX = 0;
    if (rayDisplacement.x > 0)
        tileStepX = 1;
    else
        tileStepX = -1;
    int offsetToLeadingEdgeX = (tileStepX + 1) / 2;
    float firstVerticalIntersectionX = (float)(tileX + offsetToLeadingEdgeX);
    float tOfNextXCrossing = abs(firstVerticalIntersectionX - startPosition.x) * tDeltaX;

    float tDeltaY = 1 / abs(rayDisplacement.y);
    int tileStepY = 0;
    if (rayDisplacement.y > 0)
        tileStepY = 1;
    else
        tileStepY = -1;
    int offsetToLeadingEdgeY = (tileStepY + 1) / 2;
    float firstHorizontalIntersectionY= (float)(tileY + offsetToLeadingEdgeY);
    float tOfNextYCrossing = abs(firstHorizontalIntersectionY - startPosition.y) * tDeltaY;

    while (true)
    {
        if (tOfNextXCrossing < tOfNextYCrossing)
        {
            if (tOfNextXCrossing > 1)
            {
                //NO IMPACT
                return RayCastResult2D();
            }
            tileX += tileStepX;
            Tile* currTile = GetTileAtTileCoords(IntVector2(tileX, tileY));
            if (currTile->IsSolid())
            {
                RayCastResult2D rayCastResult2D = RayCastResult2D();
                rayCastResult2D.m_didImpact = true;
                rayCastResult2D.m_impactPosition = startPosition + Vector2(rayDisplacement.x * tOfNextXCrossing, rayDisplacement.y * tOfNextYCrossing);
                rayCastResult2D.m_impactTileCoords = IntVector2(tileX, tileY);
                rayCastResult2D.m_impactFraction = tOfNextXCrossing;
                return rayCastResult2D;
            }
            tOfNextXCrossing += tDeltaX;
        }
        else
        {
            if (tOfNextYCrossing > 1)
            {
                //NO IMPACT
                return RayCastResult2D();
            }
            tileY += tileStepY;
            Tile* currTile = GetTileAtTileCoords(IntVector2(tileX, tileY));
            if (currTile->IsSolid())
            {
                RayCastResult2D rayCastResult2D = RayCastResult2D();
                rayCastResult2D.m_didImpact = true;
                rayCastResult2D.m_impactPosition = startPosition + Vector2(rayDisplacement.x * tOfNextXCrossing, rayDisplacement.y * tOfNextYCrossing);
                rayCastResult2D.m_impactTileCoords = IntVector2(tileX, tileY);
                rayCastResult2D.m_impactFraction = tOfNextYCrossing;
                return rayCastResult2D;
            }
            tOfNextYCrossing += tDeltaY;
        }
    }

    return RayCastResult2D();
}

//*******************************************************************
Path Map::GeneratePath(const IntVector2& start, const IntVector2& destination, Actor* useGCostFor /*= nullptr*/)
{
    StartSteppedPath(start, destination, useGCostFor);
    Path generatedPath;
    bool isComplete = false;
    while (!isComplete)
    {
        isComplete = ContinueSteppedPath(generatedPath);
    }
    return generatedPath;
}

//*******************************************************************
void Map::StartSteppedPath(const IntVector2& start, const IntVector2& end, Actor* useGCostFor /*= nullptr*/)
{
    if (m_currentPath != nullptr)
    {
        delete m_currentPath;
        m_currentPath = nullptr;
    }
    m_currentPath = new PathGenerator(this, start, end, useGCostFor);
}

//*******************************************************************
bool Map::ContinueSteppedPath(Path& out_pathWhenComplete)
{
    GUARANTEE_OR_DIE(m_currentPath != nullptr, "The current A* path is nullptr.");
    bool isComplete = m_currentPath->Step();
    if (isComplete)
    {
        out_pathWhenComplete = m_currentPath->m_finalPath;
        return true;
    }
    return false;
}

//*******************************************************************
int Map::CalculateManhattanDistance(const IntVector2& start, const IntVector2& end) const
{
    return abs(end.x - start.x) + abs(end.y - start.y);
}

//*******************************************************************
void Map::Update()
{
}

//*******************************************************************
void Map::Render()
{
    if ((int)m_vbos.size() == 0)
    {
        VertexBuffer* vboFill = new VertexBuffer(g_theSimpleRenderer->m_device, nullptr, 32, BUFFERUSAGE_DYNAMIC);
        VertexBuffer* vboGlyph = new VertexBuffer(g_theSimpleRenderer->m_device, nullptr, 32, BUFFERUSAGE_DYNAMIC);
        VertexBuffer* vboDebug = new VertexBuffer(g_theSimpleRenderer->m_device, nullptr, 32, BUFFERUSAGE_DYNAMIC);
        VertexBuffer* vboTint = new VertexBuffer(g_theSimpleRenderer->m_device, nullptr, 32, BUFFERUSAGE_DYNAMIC);
        m_vbos.push_back(vboFill);
        m_vbos.push_back(vboGlyph);
        m_vbos.push_back(vboDebug);
        m_vbos.push_back(vboTint);
    }

    RenderTiles();
}

//*******************************************************************
void Map::RenderTiles()
{
    ClearVertexes();

    for (int tileIndex = 0; tileIndex < (int)m_tiles.size(); ++tileIndex)
    {
        Tile tile = m_tiles[tileIndex];
        tile.Render(m_fillVertexes, m_glyphVertexes, m_tintVertexes);
    }

    if ((int)m_fillVertexes.size() > 0)
    {
        g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_whiteSample);
        g_theSimpleRenderer->DrawVertexArrays(&m_fillVertexes[0], (int)m_fillVertexes.size(), m_vbos[0]);
    }    

    if ((int)m_glyphVertexes.size() > 0)
    {
        g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_fontTexture);
        g_theSimpleRenderer->EnableBlend(BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA);
        g_theSimpleRenderer->DrawVertexArrays(&m_glyphVertexes[0], (int)m_glyphVertexes.size(), m_vbos[1]);
        g_theSimpleRenderer->DisableBlend();
    }    
    
    if ((int)m_tintVertexes.size() > 0)
    {
        g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_whiteSample);
        g_theSimpleRenderer->DrawVertexArrays(&m_tintVertexes[0], (int)m_tintVertexes.size(), m_vbos[3]);
    }    
}

//*******************************************************************
void Map::RenderTestAStarPath()
{
    char glyphCstring[] = { 'x', '\0'};
    Rgba glyphColor = Rgba(255, 0, 255, 255);

    for (int pathTileIndex = 0; pathTileIndex < (int)m_testPath.size(); ++pathTileIndex)
    {
        Tile* tile = m_testPath[pathTileIndex];
        float centerX = (tile->m_tileCoords.x * TILE_WIDTH) + (TILE_WIDTH / 2.f);
        float centerY = (tile->m_tileCoords.y * TILE_HEIGHT) + (TILE_HEIGHT / 2.f);

        g_theSimpleRenderer->DrawTextCenteredAndScaledToFitAABB2D(g_theSimpleRenderer->m_font, AABB2D(Vector2(centerX, centerY), TILE_WIDTH, TILE_HEIGHT), glyphCstring, glyphColor, m_debugVertexes);
    }    

    g_theSimpleRenderer->DrawVertexArrays(&m_debugVertexes[0], (int)m_debugVertexes.size(), m_vbos[2]);
}

//*******************************************************************
void Map::ClearVertexes()
{
    m_fillVertexes.clear();
    m_glyphVertexes.clear();
    m_debugVertexes.clear();
    m_tintVertexes.clear();
}

//*******************************************************************
void Map::RandomlyPlacedExit(std::string exitType)
{
    Feature* feature = new Feature();
    feature->ChangeType(exitType);

    int randomX = GetRandomIntInRange(1, (int)SCREEN_WIDTH - 2);
    int randomY = GetRandomIntInRange(1, (int)SCREEN_HEIGHT - 2);

    Tile* chosenTile = GetTileAtTileCoords(IntVector2(randomX, randomY));
    while (chosenTile->m_tileDefinition->m_type != "grass" || chosenTile->m_occupant != nullptr || chosenTile->m_feature != nullptr || (int)chosenTile->m_inventory.m_items.size() > 0)
    {
        randomX = GetRandomIntInRange(1, (int)SCREEN_WIDTH - 2);
        randomY = GetRandomIntInRange(1, (int)SCREEN_HEIGHT - 2);
        chosenTile = this->GetTileAtTileCoords(IntVector2(randomX, randomY));
    }

    feature->m_currentTile = chosenTile;
    AddFeatureAtTileCoords(feature->m_currentTile->m_tileCoords, feature);
    m_entities.push_back(feature);
}

//*******************************************************************
void Map::RandomlyPlacePuzzleFeatures()
{
    for (int puzzleIndex = 0; puzzleIndex < (int)m_puzzle.size(); puzzleIndex++)
    {
        PuzzleFeature* currentPuzzleFeature = m_puzzle[puzzleIndex];

        int randomX = GetRandomIntInRange(1, (int)SCREEN_WIDTH - 2);
        int randomY = GetRandomIntInRange(1, (int)SCREEN_HEIGHT - 2);

        Tile* chosenTile = GetTileAtTileCoords(IntVector2(randomX, randomY));
        while (chosenTile->m_tileDefinition->m_type != "grass" || chosenTile->m_occupant != nullptr || chosenTile->m_feature != nullptr || (int)chosenTile->m_inventory.m_items.size() > 0)
        {
            randomX = GetRandomIntInRange(1, (int)SCREEN_WIDTH - 2);
            randomY = GetRandomIntInRange(1, (int)SCREEN_HEIGHT - 2);
            chosenTile = this->GetTileAtTileCoords(IntVector2(randomX, randomY));
        }

        currentPuzzleFeature->m_currentTile = chosenTile;
        AddFeatureAtTileCoords(currentPuzzleFeature->m_currentTile->m_tileCoords, currentPuzzleFeature);
    }
}