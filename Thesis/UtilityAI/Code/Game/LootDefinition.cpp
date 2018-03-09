#include "Game/Actor.hpp"
#include "Game/Item.hpp"
#include "Game/LootDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <typeinfo>

//*******************************************************************
LootDefinition::LootDefinition()
    : m_name("none")
{
}

//*******************************************************************
LootDefinition::LootDefinition(const tinyxml2::XMLElement& xmlElement)
    : m_name("none")
{
    m_name = ParseXmlAttribute(xmlElement, "name", m_name);    
    ParseItems(xmlElement);
    ParseActors(xmlElement);
}

//*******************************************************************
LootDefinition::~LootDefinition()
{
}

//*******************************************************************
LootDefinition* LootDefinition::GetLootDefinition(const std::string& lootName)
{
    return s_lootDefinitions[lootName];
}

//*******************************************************************
void LootDefinition::PushLootDefinition(LootDefinition* lootDefinition)
{
    if (s_lootDefinitions.find(lootDefinition->m_name) == s_lootDefinitions.end())
    {
        s_lootDefinitions[lootDefinition->m_name] = lootDefinition;
    }
    else
    {
        ASSERT_OR_DIE(true, "Multiple loot definitions!");
    }
}

//*******************************************************************
void LootDefinition::ParseFromXML(const std::string& fileName)
{
    tinyxml2::XMLDocument xmlDoc;
    //tinyxml2::XMLError eResult = xmlDoc.LoadFile("Data/Gameplay/Tiles.xml");
    tinyxml2::XMLError eResult = xmlDoc.LoadFile(fileName.c_str());
    ASSERT_OR_DIE(eResult == tinyxml2::XML_SUCCESS, "Error in loading file from file, could not find Items.xml file\n");

    auto root = xmlDoc.RootElement();
    ASSERT_OR_DIE(root != nullptr, "Error in loading loot from file, could not find root\n");

    for (auto xml_lootDefintion = root->FirstChildElement("LootDefinition"); xml_lootDefintion != nullptr; xml_lootDefintion = xml_lootDefintion->NextSiblingElement())
    {
        LootDefinition* lootDefinition = new LootDefinition(*xml_lootDefintion);
        LootDefinition::PushLootDefinition(lootDefinition);
    }
}

//*******************************************************************
void LootDefinition::ParseItems(const tinyxml2::XMLElement& xmlElement)
{
    char const *itemNames = xmlElement.Attribute("items");

    ASSERT_OR_DIE(itemNames != nullptr, "LootDefinition::ParseItems : items attribute does not exist");

    std::istringstream iStringStream(itemNames);
    std::string stringToken;

    while (std::getline(iStringStream, stringToken, ','))
    {
        Item item = Item();
        item.ChangeType(stringToken);
        m_items.push_back(item);
    }
}

//*******************************************************************
void LootDefinition::ParseActors(const tinyxml2::XMLElement& xmlElement)
{
    char const *actorNames = xmlElement.Attribute("actors");

    ASSERT_OR_DIE(actorNames != nullptr, "LootDefinition::ParseActors : actors attribute does not exist");

    std::istringstream iStringStream(actorNames);
    std::string stringToken;

    while (std::getline(iStringStream, stringToken, ','))
    {
        Actor actor = Actor();
        actor.ChangeType(stringToken);
        m_actors.push_back(actor);
    }
}


//*******************************************************************
std::vector<Item> LootDefinition::GetItemsBasedOnActor(std::string actorName)
{
    std::vector<Item> possibleItems;
    std::map<std::string, LootDefinition*>::iterator lootDefinitionMapIterator;
    for (lootDefinitionMapIterator = s_lootDefinitions.begin(); lootDefinitionMapIterator != s_lootDefinitions.end(); lootDefinitionMapIterator++)
    {
        std::vector<Actor>& actors = lootDefinitionMapIterator->second->m_actors;
        std::vector<Actor>::iterator actorIterator;

        Actor myTypeOfActor = Actor();
        myTypeOfActor.ChangeType(actorName);

        bool wasActorFound = ActorExistsInLootDefinition(actors, myTypeOfActor);
        if (wasActorFound)
        {
            std::vector<Item>& lootDefinitionItems = s_lootDefinitions[lootDefinitionMapIterator->first]->m_items;
            for (int lootDefinitionItemsIndex = 0; lootDefinitionItemsIndex < (int)lootDefinitionItems.size(); lootDefinitionItemsIndex++)
            {
                possibleItems.push_back(lootDefinitionItems[lootDefinitionItemsIndex]);
            }            
        }
    }
    return possibleItems;
}

//*******************************************************************
bool LootDefinition::ActorExistsInLootDefinition(std::vector<Actor>& actors, Actor myTypeOfActor)
{
    for (int actorsIndex = 0; actorsIndex < (int)actors.size(); actorsIndex++)
    {
        if (myTypeOfActor.m_name == actors[actorsIndex].m_name)
        {
            return true;
        }
    }
    return false;
}