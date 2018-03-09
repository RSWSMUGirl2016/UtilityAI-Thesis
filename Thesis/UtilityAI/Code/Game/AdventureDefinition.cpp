#include "Game/AdventureDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <stdlib.h>

//*******************************************************************
AdventureDefinition::AdventureDefinition()
    : m_name("none")
{
}

//*******************************************************************
AdventureDefinition::AdventureDefinition(const tinyxml2::XMLElement& xmlElement)
{
    m_name = ParseXmlAttribute(xmlElement, "name", m_name);
    m_title = ParseXmlAttribute(xmlElement, "title", m_title);

    const tinyxml2::XMLElement* startConditionsElement = xmlElement.FirstChildElement("StartConditions");
    m_nameOfStartMap = startConditionsElement->Attribute("startMap");
    m_nameOfStartTileType = startConditionsElement->Attribute("startTileType");

    PushMaps(xmlElement);
}

//*******************************************************************
AdventureDefinition::~AdventureDefinition()
{
}

//*******************************************************************
AdventureDefinition* AdventureDefinition::GetAdventureDefinition(const std::string& adventureName)
{
    return s_adventureDefinitions[adventureName];
}

//*******************************************************************
void AdventureDefinition::PushAdventureDefinition(AdventureDefinition* adventureDefinition)
{
    if (s_adventureDefinitions.find(adventureDefinition->m_name) == s_adventureDefinitions.end())
    {
        s_adventureDefinitions[adventureDefinition->m_name] = adventureDefinition;
    }
    else
    {
        ASSERT_OR_DIE(true, "Multiple adventure definitions!");
    }
}

//*******************************************************************
void AdventureDefinition::PushMaps(const tinyxml2::XMLElement& xmlElement)
{
    int currentMapIndex = 0;
    int mapIndexIncrementer = 1;
    const tinyxml2::XMLElement* mapsElement = xmlElement.FirstChildElement("Maps");
    for (auto xml_map = mapsElement->FirstChildElement("Map"); xml_map != nullptr; xml_map = xml_map->NextSiblingElement())
    {
        std::string mapName = xml_map->Attribute("mapName");
        std::string mapDefinitionName = xml_map->Attribute("mapDefinition");

        const tinyxml2::XMLElement* exitElement = xml_map->FirstChildElement("Exit");
        std::string featureType = exitElement->Attribute("type");
        std::string destinationMapName = exitElement->Attribute("destinationMap");
        std::string destinationFeatureType = exitElement->Attribute("createDestinationFeature");

        if ((m_maps.find(destinationMapName) == m_maps.end()) && (destinationMapName != "none")) {
            Map* destinationMap = new Map();
            destinationMap->ChangeType(destinationMapName);
            m_maps[destinationMapName] = destinationMap;
            m_orderOfMaps[currentMapIndex + 1] = destinationMapName;
            mapIndexIncrementer++;
        }
       
        if (m_maps.find(mapName) == m_maps.end()) {
            Map* map = new Map();
            map->ChangeType(mapName);
            if (destinationMapName != "none")
                map->m_nextMap = m_maps[destinationMapName];
            else
                map->m_nextMap = nullptr;

            if (featureType != "none")
            {
                Feature* exitFeature = new Feature();
                exitFeature->ChangeType(featureType);
                exitFeature->m_featureDefinition->m_subType = "exit";
                m_exitFeature = exitFeature;
                //map->RandomlyPlacedExit(featureType);
                map->m_entities.push_back(m_exitFeature);                
            }   
            PushPuzzle(*xml_map, map);

            m_maps[mapName] = map;
            m_orderOfMaps[currentMapIndex] = mapName;
        }
        else
        {
            if (featureType != "none")
            {
                Feature* exitFeature = new Feature();
                exitFeature->ChangeType(featureType);
                exitFeature->m_featureDefinition->m_subType = "exit";
                m_exitFeature = exitFeature;
                //m_maps[mapName]->RandomlyPlacedExit(featureType);
                m_maps[mapName]->m_entities.push_back(m_exitFeature);                
            }
            PushPuzzle(*xml_map, m_maps[mapName]);
        }

        currentMapIndex += mapIndexIncrementer;
        if (mapIndexIncrementer > 1)
        {
            mapIndexIncrementer = 1;
        }        
    }
}

//*******************************************************************
void AdventureDefinition::PushPuzzle(const tinyxml2::XMLElement& xmlElement, Map* map)
{
    const tinyxml2::XMLElement* puzzleElement = xmlElement.FirstChildElement("Puzzle");
    std::string puzzleType = puzzleElement->Attribute("puzzleType");
    std::string puzzleName = puzzleElement->Attribute("puzzleName");
    //int numberOfFeatures = puzzleElement->IntAttribute("numberOfFeatures");    
    int numberOfTurnsToSolvePuzzle = puzzleElement->IntAttribute("numTurnsToSolve");
    map->m_totalNumTurnsToSolve = numberOfTurnsToSolvePuzzle;

    std::string solvedOrderForPuzzle = puzzleElement->Attribute("solvedOrderForPuzzle");
    for (int solvedOrderForPuzzleStringIndex = 0; solvedOrderForPuzzleStringIndex < (int)solvedOrderForPuzzle.length(); solvedOrderForPuzzleStringIndex++)
    {
        char currentCharacterInString = solvedOrderForPuzzle.at(solvedOrderForPuzzleStringIndex);
        if (currentCharacterInString != ',')
        {
            map->m_solvedOrderForPuzzle.push_back(currentCharacterInString - 48);
            PuzzleFeature* puzzleFeature = new PuzzleFeature();
            puzzleFeature->ChangeType(puzzleType);
            puzzleFeature->m_indexInSolvedOrder = currentCharacterInString - 48;
            map->m_puzzle.push_back(puzzleFeature);
        }
    }

    map->RandomlyPlacePuzzleFeatures();
}

//*******************************************************************
void AdventureDefinition::ParseFromXML(const std::string& fileName)
{
    tinyxml2::XMLDocument xmlDoc;
    //tinyxml2::XMLError eResult = xmlDoc.LoadFile("Data/Gameplay/Tiles.xml");
    tinyxml2::XMLError eResult = xmlDoc.LoadFile(fileName.c_str());
    ASSERT_OR_DIE(eResult == tinyxml2::XML_SUCCESS, "Error in loading file from file, could not find Adventures.xml file\n");

    auto root = xmlDoc.RootElement();
    ASSERT_OR_DIE(root != nullptr, "Error in loading file from file, could not find root\n");

    for (auto xml_adventureDefintion = root->FirstChildElement("AdventureDefinition"); xml_adventureDefintion != nullptr; xml_adventureDefintion = xml_adventureDefintion->NextSiblingElement())
    {
        AdventureDefinition* adventureDefinition = new AdventureDefinition(*xml_adventureDefintion);
        AdventureDefinition::PushAdventureDefinition(adventureDefinition);
    }
}
