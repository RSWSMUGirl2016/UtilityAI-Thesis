#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Math2D.hpp"
#include "Engine/Core/Noise.hpp"
#include "Game/MapGenerator_FromFile.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Tile.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <typeinfo>

//*******************************************************************
MapGenerator_FromFile::MapGenerator_FromFile()
{
}

//*******************************************************************
MapGenerator_FromFile::MapGenerator_FromFile(const tinyxml2::XMLElement& xmlElement, float permanence)
{
    m_tilesPermanence = permanence;

    const tinyxml2::XMLElement* infoElement = xmlElement.FirstChildElement("Info");
    
    m_offsetX = ParseXmlAttribute(*infoElement, "offsetx", m_offsetX);
    ASSERT_OR_DIE(m_offsetX >= 0 && m_offsetX < (SCREEN_WIDTH - m_offsetX), "OffsetX has to be larger than 0, but less than Screen Width!");

    m_offsetY = ParseXmlAttribute(*infoElement, "offsety", m_offsetY);
    ASSERT_OR_DIE(m_offsetY >= 0 && m_offsetY < (SCREEN_HEIGHT - m_offsetY), "OffsetY has to be larger than 0, but less than Screen Height!");

    m_isMirrored = ParseXmlAttribute(*infoElement, "ismirrored", m_isMirrored);
    m_rotation = ParseXmlAttribute(*infoElement, "rotation", m_rotation);
    ASSERT_OR_DIE(m_rotation >= 0 && m_rotation <= 270, "Rotation has to be 0, 90, 180, or 270.");

    bool isPerlinMap = false;
    isPerlinMap = ParseXmlAttribute(*infoElement, "isperlin", isPerlinMap);

    const tinyxml2::XMLElement* legendElement = xmlElement.FirstChildElement("Legend");
    
    std::string wallTiles;
    wallTiles = ParseXmlAttribute(*legendElement, "walls", wallTiles);
    ParseWallTiles(wallTiles);

    std::string floorTiles;
    floorTiles = ParseXmlAttribute(*legendElement, "floors", floorTiles);
    ParseFloorTiles(floorTiles);

    std::string entities;
    entities = ParseXmlAttribute(*legendElement, "entities", entities);
    ParseEntities(entities);

    if (!isPerlinMap)
    {
        const tinyxml2::XMLElement* locationsElement = xmlElement.FirstChildElement("Locations");
        for (auto xml_location = locationsElement->FirstChildElement("Location"); xml_location != nullptr; xml_location = xml_location->NextSiblingElement())
        {
            std::string name;
            int locationKey = 0;
            IntVector2 worldLocation;
            IntVector2 size;
            float health = 0.0f;

            name = ParseXmlAttribute(*xml_location, "name", name);
            locationKey = ParseXmlAttribute(*xml_location, "locationkey", locationKey);
            worldLocation = ParseXmlAttribute(*xml_location, "worldlocation", worldLocation);
            size = ParseXmlAttribute(*xml_location, "size", size);
            health = ParseXmlAttribute(*xml_location, "health", health);

            LocationDataFromFile newLocationDataFromFile;
            newLocationDataFromFile.m_name = name;
            newLocationDataFromFile.m_locationTypeKey = locationKey;
            newLocationDataFromFile.m_worldLocation = worldLocation;
            newLocationDataFromFile.m_size = size;
            newLocationDataFromFile.m_health = health;

            m_locationsData.push_back(newLocationDataFromFile);
        }
    }    
    else
    {
        const tinyxml2::XMLElement* locationsElement = xmlElement.FirstChildElement("Locations");
        for (auto xml_location = locationsElement->FirstChildElement("Location"); xml_location != nullptr; xml_location = xml_location->NextSiblingElement())
        {
            std::string name;
            int locationKey = 0;
            IntVector2 size;
            float health = 0.0f;

            name = ParseXmlAttribute(*xml_location, "name", name);
            locationKey = ParseXmlAttribute(*xml_location, "locationkey", locationKey);
            size = ParseXmlAttribute(*xml_location, "size", size);
            health = ParseXmlAttribute(*xml_location, "health", health);

            LocationDataFromFile newLocationDataFromFile;
            newLocationDataFromFile.m_name = name;
            newLocationDataFromFile.m_locationTypeKey = locationKey;
            newLocationDataFromFile.m_size = size;
            newLocationDataFromFile.m_health = health;

            m_locationsData.push_back(newLocationDataFromFile);
        }

        const tinyxml2::XMLElement* perlineDataLayersElement = xmlElement.FirstChildElement("PerlinLayersData");
        for (auto xml_perlinDataLayer = perlineDataLayersElement->FirstChildElement("PerlinLayerData"); xml_perlinDataLayer != nullptr; xml_perlinDataLayer = xml_perlinDataLayer->NextSiblingElement())
        {
            std::string layerName;
            float scale = 1.0f;
            int numOctaves = 1;
            float octavePersistence = 0.5f;
            float octaveScale = 2.0f;
            bool renormalize = true;
            int seed = 0;
            float noiseThreshold = 0.0f;

            layerName = ParseXmlAttribute(*xml_perlinDataLayer, "layername", layerName);
            scale = ParseXmlAttribute(*xml_perlinDataLayer, "scale", scale);
            numOctaves = ParseXmlAttribute(*xml_perlinDataLayer, "numoctaves", numOctaves);
            octavePersistence = ParseXmlAttribute(*xml_perlinDataLayer, "octavepersistence", octavePersistence);
            octaveScale = ParseXmlAttribute(*xml_perlinDataLayer, "octavescale", octaveScale);
            renormalize = ParseXmlAttribute(*xml_perlinDataLayer, "renormalize", renormalize);
            seed = ParseXmlAttribute(*xml_perlinDataLayer, "seed", seed);
            noiseThreshold = ParseXmlAttribute(*xml_perlinDataLayer, "noisethreshold", noiseThreshold);

            PerlinData newPerlinLayerData;
            newPerlinLayerData.m_layerName = layerName;
            newPerlinLayerData.m_scale = scale;
            newPerlinLayerData.m_numOctaves = (unsigned int)numOctaves;
            newPerlinLayerData.m_octavePersistence = octavePersistence;
            newPerlinLayerData.m_octaveScale = octaveScale;
            newPerlinLayerData.m_renormalize = renormalize;
            newPerlinLayerData.m_seed = (unsigned int)seed;
            newPerlinLayerData.m_noiseThreshold = noiseThreshold;
            m_perlinLayerData.push_back(newPerlinLayerData);
        }
    }

    const tinyxml2::XMLElement* mapRowsElement = xmlElement.FirstChildElement("MapRows");
    int numRows = 0;
    int numColumns = 0;
    numRows = ParseXmlAttribute(*mapRowsElement, "numrows", numRows);
    numColumns = ParseXmlAttribute(*mapRowsElement, "numcolumns", numColumns);

    m_dimensions = IntVector2(numRows, numColumns);

    for (auto xml_mapRow = mapRowsElement->FirstChildElement("MapRow"); xml_mapRow != nullptr; xml_mapRow = xml_mapRow->NextSiblingElement())
    {
        int index;
        std::string row;
        index = xml_mapRow->IntAttribute("index");
        row = xml_mapRow->Attribute("row");

        if (m_isMirrored)
        {
            row = std::string(row.rbegin(), row.rend());
        }

        m_mapRows[index] = row;
    }

    SetAllTilesToWalls();
    CreateMapFromFile();
    if (isPerlinMap)
    {
        CreateMapWithPerlin();
        GeneratorLocations();
    }
}

//*******************************************************************
void MapGenerator_FromFile::ParseWallTiles(std::string wallTiles)
{
    for (int wallTilesIndex = 0; wallTilesIndex < (int)wallTiles.length(); wallTilesIndex++)
    {
        if (wallTiles.at(wallTilesIndex) != ',')
        {
            m_legendWallTiles.push_back(wallTiles.at(wallTilesIndex));
        }
    }
}

//*******************************************************************
void MapGenerator_FromFile::ParseFloorTiles(std::string floorTiles)
{
    for (int floorTilesIndex = 0; floorTilesIndex < (int)floorTiles.length(); floorTilesIndex++)
    {
        if (floorTiles.at(floorTilesIndex) != ',')
        {
            m_legendFloorTiles.push_back(floorTiles.at(floorTilesIndex));
        }
    }
}

//*******************************************************************
void MapGenerator_FromFile::ParseEntities(std::string entities)
{
    for (int entitiesIndex = 0; entitiesIndex < (int)entities.length(); entitiesIndex++)
    {
        if (entities.at(entitiesIndex) != ',')
        {
            m_legendEntities.push_back(entities.at(entitiesIndex));
        }
    }
}

//*******************************************************************
MapGenerator_FromFile::~MapGenerator_FromFile()
{
}

//*******************************************************************
void MapGenerator_FromFile::CreateMapFromFile()
{
    std::map<int, std::string>::iterator mapRowsIterator;
    for (mapRowsIterator = m_mapRows.begin(); mapRowsIterator != m_mapRows.end(); mapRowsIterator++)
    {
        std::string currentRow = mapRowsIterator->second;
        for (int currentRowIndex = 0; currentRowIndex < (int)currentRow.size(); currentRowIndex++)
        {
            char glyph = currentRow[currentRowIndex];

            IntVector2 newTileCoords = IntVector2(currentRowIndex, mapRowsIterator->first);
            
            if (std::find(m_legendWallTiles.begin(), m_legendWallTiles.end(), glyph) != m_legendWallTiles.end())
            {
                Tile* tile = GetTileAtTileCoords(newTileCoords);
                std::string type = TileDefinition::GetTileTypeFromGlyph(glyph);
                tile->ChangeType(type);
                tile->m_tileCoords = newTileCoords;
                tile->m_permanence =  m_tilesPermanence;
                int tileIndex = GetTileIndexForTile(*tile);
                m_tiles[tileIndex] = *tile;
            }
            if (std::find(m_legendFloorTiles.begin(), m_legendFloorTiles.end(), glyph) != m_legendFloorTiles.end())
            {
                Tile* tile = GetTileAtTileCoords(newTileCoords);
                std::string type = TileDefinition::GetTileTypeFromGlyph(glyph);
                tile->ChangeType(type);
                tile->m_tileCoords = newTileCoords;
                tile->m_permanence = m_tilesPermanence;
                int tileIndex = GetTileIndexForTile(*tile);
                m_tiles[tileIndex] = *tile;
            }
        }
    }

}

//*******************************************************************
void MapGenerator_FromFile::ChangeTileTypeBasedOnPerlinNoise(std::string tileType, IntVector2 tileCoords, PerlinData perlinData)
{
    float currentNoise = Compute2dPerlinNoise((float)tileCoords.x, (float)tileCoords.y, perlinData.m_scale, perlinData.m_numOctaves, perlinData.m_octavePersistence, perlinData.m_octaveScale, perlinData.m_renormalize, perlinData.m_seed);
    if (perlinData.m_layerName.compare(tileType) == 0)
    {
        if (currentNoise > perlinData.m_noiseThreshold)
        {
            Tile* tile = GetTileAtTileCoords(tileCoords);
            ChangeTileType(tile, tileType);
        }
    }
}

//*******************************************************************
void MapGenerator_FromFile::CreateMapWithPerlin()
{
    for (int dataIndex = 0; dataIndex < (int)m_perlinLayerData.size(); dataIndex++)
    {
        PerlinData currentPerlinData = m_perlinLayerData[dataIndex];

        int tilesSize = (int)m_tiles.size();
        for (int tileIndex = 0; tileIndex < tilesSize; tileIndex++)
        {
            IntVector2 tileCoords = GetTileCoordinatesForTileIndex(tileIndex);

            //Perlin Noise : -1.0f to 1.0f
            //Tile Types: grass, dirt, food, animals, wood, rock, metal, water

            ChangeTileTypeBasedOnPerlinNoise("grass", tileCoords, currentPerlinData);
            ChangeTileTypeBasedOnPerlinNoise("dirt", tileCoords, currentPerlinData);
            ChangeTileTypeBasedOnPerlinNoise("food", tileCoords, currentPerlinData);
            ChangeTileTypeBasedOnPerlinNoise("animal", tileCoords, currentPerlinData);
            ChangeTileTypeBasedOnPerlinNoise("wood", tileCoords, currentPerlinData);
            ChangeTileTypeBasedOnPerlinNoise("rock", tileCoords, currentPerlinData);
            ChangeTileTypeBasedOnPerlinNoise("metal", tileCoords, currentPerlinData);
            ChangeTileTypeBasedOnPerlinNoise("water", tileCoords, currentPerlinData);
        }
    }
}

//*******************************************************************
void MapGenerator_FromFile::ChangeTileType(Tile* tile, std::string typeToBeChangedTo)
{
    tile->ChangeType(typeToBeChangedTo);
    tile->m_permanence = m_tilesPermanence;
    int tileIndex = GetTileIndexForTile(*tile);
    m_tiles[tileIndex] = *tile;
}

//*******************************************************************
void MapGenerator_FromFile::SetAllTilesToWalls()
{
    for (int tileCoordinateY = 0; tileCoordinateY < m_dimensions.x; tileCoordinateY++)
    {
        for (int tileCoordinateX = 0; tileCoordinateX < m_dimensions.y; tileCoordinateX++)
        {
            Tile tile = Tile();
            std::string tileType = TileDefinition::GetTileTypeFromGlyph(m_legendWallTiles[0]);
            tile.ChangeType(tileType);
            tile.m_tileCoords = IntVector2(tileCoordinateX, tileCoordinateY);
            tile.m_permanence = 0.f;
            m_tiles.push_back(tile);
        }
    }
}

//*******************************************************************
Tile* MapGenerator_FromFile::GetTileAtTileCoords(const IntVector2& tileCoords)
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
int MapGenerator_FromFile::GetTileIndexForTile(const Tile& tile)
{
    IntVector2 tileCoords = tile.m_tileCoords;
    return (int)(tileCoords.y * m_dimensions.y) + tileCoords.x;
}

//*******************************************************************
IntVector2 MapGenerator_FromFile::GetTileCoordinatesForTileIndex(int tileIndex)
{
    int tileX = tileIndex % (int)m_dimensions.y;
    int tileY = tileIndex / (int)m_dimensions.y;
    return IntVector2(tileX, tileY);
}

//*******************************************************************
void MapGenerator_FromFile::GeneratorLocations()
{
    int locationsSize = (int)m_locationsData.size();
    for (int locationIndex = 0; locationIndex < locationsSize; locationIndex++)
    {
        LocationDataFromFile& locationData = m_locationsData[locationIndex];
        IntVector2 possibleWorldLocation = FindRandomDirtTile();
        bool isThisPlaceAvailable = CanLocationFit(possibleWorldLocation, locationData.m_size);
        while (!isThisPlaceAvailable)
        {
            possibleWorldLocation = FindRandomDirtTile();
            isThisPlaceAvailable = CanLocationFit(possibleWorldLocation, locationData.m_size);
        }
        locationData.m_worldLocation = possibleWorldLocation;
    }
}

//*******************************************************************
bool MapGenerator_FromFile::CanLocationFit(IntVector2 tileCoords, IntVector2 locationSize)
{
    //Building on top of each other
    int locationWidth = locationSize.x;
    int locationHeight = locationSize.y;

    for (int xIndex = 0; xIndex < locationWidth; xIndex++)
    {
        for (int yIndex = 0; yIndex < locationHeight; yIndex++)
        {
            int tileCoordX = tileCoords.x + xIndex;
            int tileCoordY = tileCoords.y + yIndex;
            Tile* tile = GetTileAtTileCoords(IntVector2(tileCoordX, tileCoordY));

            if (tile == nullptr)
                return false;

            if (tile->m_tileDefinition->m_isSolid)
                return false;
            if (IsALocationAlreadyThere(tileCoords, locationSize))
                return false;
        }
    }
    return true;
}

//*******************************************************************
bool MapGenerator_FromFile::IsALocationAlreadyThere(IntVector2 tileCoords, IntVector2 locationSize)
{
    Vector2 newMins = Vector2((float)tileCoords.x, (float)tileCoords.y);
    Vector2 newMaxs = Vector2((float)(tileCoords.x + locationSize.x), (float)(tileCoords.y + locationSize.y));
    AABB2D newLocation = AABB2D(newMins, newMaxs);
    
    int locationsSize = (int)m_locationsData.size();
    for (int locationIndex = 0; locationIndex < locationsSize; locationIndex++)
    {
        IntVector2 currentLocationTileCoords = m_locationsData[locationIndex].m_worldLocation;
        IntVector2 currentLocationSize = m_locationsData[locationIndex].m_size;

        Vector2 currentMins = Vector2((float)currentLocationTileCoords.x, (float)currentLocationTileCoords.y);;
        Vector2 currentMaxs = Vector2((float)(currentLocationTileCoords.x + currentLocationSize.x), (float)(currentLocationTileCoords.y + currentLocationSize.y));
        AABB2D currentLocation = AABB2D(currentMins, currentMaxs);

        bool doLocationsOverlap = DoAABB2DsOverlap(currentLocation, newLocation);
        if (doLocationsOverlap)
            return true;
    }
    return false;
}

//*******************************************************************
IntVector2 MapGenerator_FromFile::FindRandomDirtTile()
{
    int randomXCoordinate = GetRandomIntInRange(1, m_dimensions.y - 1);
    int randomYCoordinate = GetRandomIntInRange(1, m_dimensions.x - 1);
    Tile* randomTile = GetTileAtTileCoords(IntVector2(randomXCoordinate, randomYCoordinate));

    while (randomTile->m_tileDefinition->m_type.compare("dirt") != 0)
    {
        randomXCoordinate = GetRandomIntInRange(1,m_dimensions.y - 1);
        randomYCoordinate = GetRandomIntInRange(1,m_dimensions.x - 1);
        randomTile = GetTileAtTileCoords(IntVector2(randomXCoordinate, randomYCoordinate));
    }

    return IntVector2(randomXCoordinate, randomYCoordinate);
}