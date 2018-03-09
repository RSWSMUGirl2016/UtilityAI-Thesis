#pragma once
#include "Engine/Core/XMLUtils.hpp"
#include "Engine/Core/XMLUtils.hpp"
#include "Game/MapGenerator.hpp"

class Tile;

class MapGenerator_FromFile : public MapGenerator
{
public:
    int m_offsetX;
    int m_offsetY;
    bool m_isMirrored;
    int m_rotation;
    std::vector<char> m_legendEntities;
    std::vector<char> m_legendWallTiles;
    std::vector<char> m_legendFloorTiles;
    std::map<int, std::string> m_mapRows;

    MapGenerator_FromFile();
    ~MapGenerator_FromFile();

    MapGenerator_FromFile(const tinyxml2::XMLElement& xmlElement, float permanence);

    void ParseWallTiles(std::string wallTiles);
    void ParseFloorTiles(std::string floorTiles);
    void ParseEntities(std::string entities);

    void CreateMapFromFile();
    void ChangeTileTypeBasedOnPerlinNoise(std::string tileType, IntVector2 tileCoords, PerlinData perlinData);
    void CreateMapWithPerlin();
    void ChangeTileType(Tile* tile, std::string typeToBeChangedTo);
    void SetAllTilesToWalls();

    Tile* GetTileAtTileCoords(const IntVector2& tileCoords);
    int GetTileIndexForTile(const Tile& tile);
    IntVector2 GetTileCoordinatesForTileIndex(int tileIndex);
    void GeneratorLocations();
    bool CanLocationFit(IntVector2 tileCoords, IntVector2 locationSize);
    bool IsALocationAlreadyThere(IntVector2 tileCoords, IntVector2 locationSize);
    IntVector2 FindRandomDirtTile();
};