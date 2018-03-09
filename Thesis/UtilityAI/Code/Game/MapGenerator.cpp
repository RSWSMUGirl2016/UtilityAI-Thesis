#include "Game/MapGenerator.hpp"
#include "Engine/Math/MathUtilities.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Tile.hpp"

//*******************************************************************
MapGenerator::MapGenerator()
{
}

//*******************************************************************
MapGenerator::~MapGenerator()
{
}

//*******************************************************************
Tile* MapGenerator::GetTileAtTileCoords(const IntVector2& tileCoords)
{
    int tileIndex = tileCoords.x + (tileCoords.y * (int)SCREEN_WIDTH);
    if ((tileIndex >= (int)m_tiles.size()) || (tileIndex < 0))
        return nullptr;
    Tile* tile = &m_tiles[tileIndex];
    if (tile != nullptr && (tileCoords.x < SCREEN_WIDTH) && (tileCoords.y < SCREEN_HEIGHT))
        return tile;
    return nullptr;
}

//*******************************************************************
void MapGenerator::AddOccupantAtTileCoords(const IntVector2& tileCoords, Actor* actor)
{
    int tileIndex = tileCoords.x + (tileCoords.y * (int)SCREEN_WIDTH);
    if ((tileIndex >= (int)m_tiles.size()) || (tileIndex < 0))
        return;
    Tile* tile = &m_tiles[tileIndex];
    actor->m_tileCoords = tile->m_tileCoords;
    tile->m_occupant = actor;
    actor->m_currentTile = tile;
}

//*******************************************************************
void MapGenerator::AddFeatureAtTileCoords(const IntVector2& tileCoords, Feature* feature)
{
    int tileIndex = tileCoords.x + (tileCoords.y * (int)SCREEN_WIDTH);
    if ((tileIndex >= (int)m_tiles.size()) || (tileIndex < 0))
        return;
    Tile* tile = &m_tiles[tileIndex];
    feature->m_tileCoords = tile->m_tileCoords;
    tile->m_feature = feature;
}
