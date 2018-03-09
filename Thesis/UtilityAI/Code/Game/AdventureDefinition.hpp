#pragma once
#include "Engine/Core/XMLUtils.hpp"
#include "Game/Feature.hpp"
#include "Game/PuzzleFeature.hpp"
#include "Game/Map.hpp"
#include <vector>
#include <string>
#include <map>

class AdventureDefinition
{
public:
    AdventureDefinition();
    AdventureDefinition(const tinyxml2::XMLElement& xmlElement);
    ~AdventureDefinition();

    static AdventureDefinition* GetAdventureDefinition(const std::string& adventureName);
    static void PushAdventureDefinition(AdventureDefinition* adventureDefinition);
    void PushMaps(const tinyxml2::XMLElement& xmlElement);
    void PushPuzzle(const tinyxml2::XMLElement& xmlElement, Map* map);
    static void ParseFromXML(const std::string& fileName);

    std::string m_name;
    std::string m_title;
    std::string m_nameOfStartMap;
    std::string m_nameOfStartTileType;
    std::map<int, std::string> m_orderOfMaps;
    std::map<std::string, Map*> m_maps;
    Feature* m_exitFeature;
    Feature* m_entranceFeature;
    std::vector<PuzzleFeature*> m_puzzle;
    std::vector<int> m_solvedOrderForPuzzle;
};

static std::map<std::string, AdventureDefinition*> s_adventureDefinitions;