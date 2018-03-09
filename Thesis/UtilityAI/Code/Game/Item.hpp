#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Game/ItemDefinition.hpp"
#include "Game/Stats.hpp"
#include <string>

enum EquipSlot
{
    NONE = -1,
    HEAD,
    SECONDARY,
    PRIMARY,
    CHEST,
    WAIST,
    LEGS,
    FEET,
    NUM_EQUIP_SLOTS
};

class Item
{
public:
    Stats m_stats;
    ItemDefinition* m_itemDefinition;
    IntVector2 m_tileCoords;

    std::string m_name;
    Rgba m_fillcolor;
    Rgba m_glyphcolor;
    char m_glyph;
    EquipSlot m_equipSlot;
    bool m_instantUse;
    int m_health;
    int m_defense;
    int m_damage;
    int m_weight;

public:
    Item();
	~Item();
    
    void ChangeType(const std::string& itemName);
    void ChangeEquipSlot(std::string newEquipSlot);

    void Update();
    void Render() const;
};