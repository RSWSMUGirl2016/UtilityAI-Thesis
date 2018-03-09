#pragma once
#include "Engine/Core/XMLUtils.hpp"
#include "Game/MapGenerator_FromFile.hpp"
#include "Game/MapGenerator_RoomsAndPaths.hpp"
#include <map>
#include <vector>

class MapDefinition
{
public:
    MapDefinition();
    MapDefinition(const tinyxml2::XMLElement& xmlElement);
    ~MapDefinition();

    static MapDefinition* GetMapDefinition(const std::string& mapName);
    static void PushMapDefinition(MapDefinition* mapDefinition);
    static void ParseFromXML(const std::string& fileName);

    void PushGenerators(const tinyxml2::XMLElement& xmlElement);
    void SetTilesBasedOnGenerator();
    
    std::string m_name;
    std::vector<MapGenerator*> m_generators;
    std::vector<Tile> m_tiles;
};

static std::map<std::string, MapDefinition*> s_mapDefinitions;