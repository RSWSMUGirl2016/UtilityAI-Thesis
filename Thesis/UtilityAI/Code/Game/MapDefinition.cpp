#include "Game/GameCommon.hpp"
#include "Game/MapDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <typeinfo>

//*******************************************************************
MapDefinition::MapDefinition()
    : m_name("none")
{
}

//*******************************************************************
MapDefinition::MapDefinition(const tinyxml2::XMLElement& xmlElement)
    : m_name("none")
{
    m_name = ParseXmlAttribute(xmlElement, "name", m_name);

    PushGenerators(xmlElement);
    SetTilesBasedOnGenerator();
}

//*******************************************************************
MapDefinition::~MapDefinition()
{
    for (int mapGeneratorIndex = 0; mapGeneratorIndex < (int)m_generators.size(); mapGeneratorIndex++)
    {
        delete m_generators[mapGeneratorIndex];
        m_generators[mapGeneratorIndex] = nullptr;
    }
    m_generators.clear();
}

//*******************************************************************
MapDefinition* MapDefinition::GetMapDefinition(const std::string& mapName)
{
    return s_mapDefinitions[mapName];
}

//*******************************************************************
void MapDefinition::PushMapDefinition(MapDefinition* mapDefinition)
{
    if (s_mapDefinitions.find(mapDefinition->m_name) == s_mapDefinitions.end())
    {
        s_mapDefinitions[mapDefinition->m_name] = mapDefinition;
    }
    else
    {
        ASSERT_OR_DIE(true, "Multiple map definitions!");
    }
}

//*******************************************************************
void MapDefinition::ParseFromXML(const std::string& fileName)
{
    tinyxml2::XMLDocument xmlDoc;
    //tinyxml2::XMLError eResult = xmlDoc.LoadFile("Data/Gameplay/Tiles.xml");
    tinyxml2::XMLError eResult = xmlDoc.LoadFile(fileName.c_str());
    ASSERT_OR_DIE(eResult == tinyxml2::XML_SUCCESS, "Error in loading file from file, could not find Maps.xml file\n");

    auto root = xmlDoc.RootElement();
    ASSERT_OR_DIE(root != nullptr, "Error in loading map from file, could not find root\n");

    for (auto xml_mapDefintion = root->FirstChildElement("MapDefinition"); xml_mapDefintion != nullptr; xml_mapDefintion = xml_mapDefintion->NextSiblingElement())
    {
        MapDefinition* mapDefinition = new MapDefinition(*xml_mapDefintion);
        MapDefinition::PushMapDefinition(mapDefinition);
    }
}

//*******************************************************************
void MapDefinition::PushGenerators(const tinyxml2::XMLElement& xmlElement)
{
    const tinyxml2::XMLElement* generatorsElement = xmlElement.FirstChildElement("Generators");
    for (auto xml_generator = generatorsElement->FirstChildElement("Generator"); xml_generator != nullptr; xml_generator = xml_generator->NextSiblingElement())
    {
        std::string generatorName = xml_generator->Attribute("genName");
        float permanence = xml_generator->FloatAttribute("permanence");

        if (generatorName == "roomsandpaths")
        {
            m_generators.push_back(new MapGenerator_RoomsAndPaths(permanence));
        }

        if (generatorName == "fromfile")
        {
            m_generators.push_back(new MapGenerator_FromFile(*xml_generator, permanence));
        }
    }
}

//*******************************************************************
void MapDefinition::SetTilesBasedOnGenerator()
{
    for (int generatorIndex = 0; generatorIndex < (int)m_generators.size(); generatorIndex++)
    {
        MapGenerator* currentMapGenerator = m_generators[generatorIndex];
        if (generatorIndex == 0)
        {
            for (int generatorTilesIndex = 0; generatorTilesIndex < (int)currentMapGenerator->m_tiles.size(); generatorTilesIndex++)
            {
                Tile& currentGeneratorTile = currentMapGenerator->m_tiles[generatorTilesIndex];
                m_tiles.push_back(currentGeneratorTile);
            }
        }
        else
        {
            for (int generatorTilesIndex = 0; generatorTilesIndex < (int)currentMapGenerator->m_tiles.size(); generatorTilesIndex++)
            {
                Tile& currentGeneratorTile = currentMapGenerator->m_tiles[generatorTilesIndex];
                Tile& currentTileDefinitionTile = m_tiles[generatorTilesIndex];

                if (currentTileDefinitionTile.m_permanence < currentGeneratorTile.m_permanence)
                {
                    std::string type = currentGeneratorTile.m_tileDefinition->m_type;
                    currentTileDefinitionTile.ChangeType(type);
                    currentTileDefinitionTile.m_occupant = currentGeneratorTile.m_occupant;
                    currentTileDefinitionTile.m_feature = currentGeneratorTile.m_feature;
                    currentTileDefinitionTile.m_tileCoords = currentGeneratorTile.m_tileCoords;
                    currentTileDefinitionTile.m_inventory = currentGeneratorTile.m_inventory;
                }                
            }
        }
    }
}
