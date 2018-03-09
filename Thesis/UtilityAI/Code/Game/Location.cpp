#include "Game/Location.hpp"
#include "Game/Map.hpp"

#include <algorithm>

//*******************************************************************
Location::Location()
{}

//*******************************************************************
Location::Location(LocationType newLocationType, IntVector2 newWorldLocation, IntVector2 newSize)
    : m_locationType(newLocationType)
    , m_worldLocation(newWorldLocation)
    , m_size(newSize)
{}

//*******************************************************************
Location::~Location()
{
    for (Resource* currentResource : m_resources)
    {
        if (currentResource != nullptr)
        {
            delete currentResource;
            currentResource = nullptr;
        }
    }
    m_resources.clear();
}

//*******************************************************************
void Location::AddResourceToBuilding(Resource* newResource)
{
    m_resources.push_back(newResource);
}

//*******************************************************************
void Location::RemoveResourceFromBuilding(Resource* usedResource)
{
    for (Resource* currentResource : m_resources)
    {
        if (currentResource == usedResource)
        {
            m_resources.erase(std::remove(m_resources.begin(), m_resources.end(), currentResource), m_resources.end());
            return;
        }
    }
}

//*******************************************************************
LocationType Location::GetLocationType()
{
    return m_locationType;
}

//*******************************************************************
IntVector2 Location::GetWorldLocation()
{
    return m_worldLocation;
}

//*******************************************************************
IntVector2 Location::GetLocationSize()
{
    return m_size;
}

//*******************************************************************
void Location::SetHealth(float newHealth)
{
    m_health = newHealth;
}

//*******************************************************************
float Location::GetHealth()
{
    return m_health;
}

//*******************************************************************
void Location::Update()
{
    m_health -= 1.0f;
}

//*******************************************************************
void Location::ReplinishTiles(Map* map)
{
    int locationWidth = m_size.x;
    int locationHeight = m_size.y;

    for (int xIndex = 0; xIndex < locationWidth; xIndex++)
    {
        for (int yIndex = 0; yIndex < locationHeight; yIndex++)
        {
            Tile* tile = map->GetTileAtTileCoords(IntVector2(m_worldLocation.x + xIndex, m_worldLocation.y + yIndex));
            if (tile->m_tileDefinition->m_type.compare("dirt") == 0)
            {
                std::string tileResource = "";
                switch (m_locationType)
                {
                case LOCATION_WELL: tileResource = "water"; break;
                case LOCATION_FOOD_STORAGE: tileResource = "food"; break;
                case LOCATION_FARM: tileResource = "animal"; break;
                case LOCATION_ROCK_MINE: tileResource = "rock"; break;
                case LOCATION_METAL_MINE: tileResource = "metal"; break;
                default: tileResource = "dirt";
                }
                tile->ChangeType(tileResource);
                tile->m_tileCoords = IntVector2(tile->m_tileCoords.x, tile->m_tileCoords.y);
                tile->m_permanence = 1.0f;
                int tileIndex = map->GetTileIndexForTile(*tile);
                map->m_tiles[tileIndex] = *tile;
            }
        }
    }
}
