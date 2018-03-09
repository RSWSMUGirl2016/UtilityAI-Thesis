#pragma once

#include "Game/Resource.hpp"
#include "Engine/Math/IntVector2.hpp"

#include <vector>

class Map;

enum LocationType
{
    LOCATION_WELL, //0
    LOCATION_FARM, //1
    LOCATION_HOUSE, //2
    LOCATION_PIPE, //3
    LOCATION_FOOD_STORAGE, //4
    LOCATION_HUT, //5
    LOCATION_ROCK_MINE, //6
    LOCATION_METAL_MINE, //7
};

class Location
{
private:
    std::vector<Resource*> m_resources;
    LocationType m_locationType;
    float m_health;
    IntVector2 m_worldLocation;
    IntVector2 m_size;

public:
    Location();
    Location(LocationType newLocationType, IntVector2 newWorldLocation, IntVector2 newSize);
    ~Location();

public:
    void AddResourceToBuilding(Resource* newResource);
    void RemoveResourceFromBuilding(Resource* usedResource);
    LocationType GetLocationType();
    IntVector2 GetWorldLocation();
    IntVector2 GetLocationSize();
    void SetHealth(float newHealth);
    float GetHealth();
    void Update();
    void ReplinishTiles(Map* map);
};