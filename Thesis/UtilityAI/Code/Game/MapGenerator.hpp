#pragma once
#include "Engine/Math/IntVector2.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "Game/Feature.hpp"
#include <vector>

class Tile;

struct LocationDataFromFile
{
    std::string m_name;
    int m_locationTypeKey;
    IntVector2 m_worldLocation;
    IntVector2 m_size;
    float m_health;
};

struct PerlinData
{
    std::string m_layerName; //whatever tile you want to populate
    float m_scale; //1.0f
    unsigned int m_numOctaves; //1 
    float m_octavePersistence; //0.5f
    float m_octaveScale; //2.0f
    bool m_renormalize; //true, do no change this!
    unsigned int m_seed; //change for each layer 
    float m_noiseThreshold; //change for how much you want it to show on the map
};

class MapGenerator
{
public:
    std::vector<Entity*> m_entities;
    std::vector<Tile> m_tiles;
    IntVector2 m_dimensions;
    Player* m_player;
    std::string m_name;
    float m_tilesPermanence;
    std::vector<LocationDataFromFile> m_locationsData;
    std::vector<PerlinData> m_perlinLayerData;

    MapGenerator();
    ~MapGenerator();

    Tile* GetTileAtTileCoords(const IntVector2& tileCoords);
    void AddOccupantAtTileCoords(const IntVector2& tileCoords, Actor* actor);
    void AddFeatureAtTileCoords(const IntVector2& tileCoords, Feature* feature);
};