#pragma once
#include "Engine/Core/XMLUtils.hpp"
#include <map>
#include <vector>

class Actor;
class Item;

class LootDefinition
{
public:
    LootDefinition();
    LootDefinition(const tinyxml2::XMLElement& xmlElement);
    ~LootDefinition();

    static LootDefinition* GetLootDefinition(const std::string& lootName);
    static void PushLootDefinition(LootDefinition* lootDefinition);

    static void ParseFromXML(const std::string& fileName);
    void ParseItems(const tinyxml2::XMLElement& xmlElement);
    void ParseActors(const tinyxml2::XMLElement& xmlElement);

    static std::vector<Item> GetItemsBasedOnActor(std::string actorName);
    static bool ActorExistsInLootDefinition(std::vector<Actor>& actors, Actor myTypeOfActor);

    std::string m_name;
    std::vector<Item> m_items;
    std::vector<Actor> m_actors;
};

static std::map<std::string, LootDefinition*> s_lootDefinitions;