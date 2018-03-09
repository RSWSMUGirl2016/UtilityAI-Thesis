#pragma once
#include "Engine/Core/XMLUtils.hpp"
#include <map>
#include <vector>

class ItemDefinition
{
public:
    ItemDefinition();
    ItemDefinition(const tinyxml2::XMLElement& xmlElement);
    ~ItemDefinition();

    static ItemDefinition* GetItemDefinition(const std::string& actorName);
    static void PushItemDefinition(ItemDefinition* itemDefinition);
    static void ParseFromXML(const std::string& fileName);

    std::string m_name;
    std::vector<Rgba> m_fillColors;
    std::vector<Rgba> m_glyphColors;
    std::vector<char> m_glyphs;
    std::string m_equipSlot;
    bool m_instantUse;
    int m_health;
    int m_defense;
    int m_damage;
    int m_weight;
};

static std::map<std::string, ItemDefinition*> s_itemDefinitions;