#include "Game/Item.hpp"

//*******************************************************************
Item::Item()
{
}

//*******************************************************************
Item::~Item()
{
}

//*******************************************************************
void Item::ChangeType(const std::string& itemName)
{
    m_itemDefinition = ItemDefinition::GetItemDefinition(itemName);

    m_name = m_itemDefinition->m_name;
    ChangeEquipSlot(m_itemDefinition->m_equipSlot);
    m_instantUse = m_itemDefinition->m_instantUse;
    m_health = m_itemDefinition->m_health;
    m_damage = m_itemDefinition->m_damage;
    m_defense = m_itemDefinition->m_defense;
    m_glyphcolor = m_itemDefinition->m_glyphColors[0];
    m_fillcolor = m_itemDefinition->m_fillColors[0];
    m_glyph = m_itemDefinition->m_glyphs[0];
    m_weight = m_itemDefinition->m_weight;
}

//*******************************************************************
void Item::ChangeEquipSlot(std::string newEquipSlot)
{
    if (newEquipSlot == "none")
    {
        m_equipSlot = NONE;
        return;
    }
    if (newEquipSlot == "head")
    {
        m_equipSlot = HEAD;
        return;
    }if (newEquipSlot == "secondary")
    {
        m_equipSlot = SECONDARY;
        return;
    }if (newEquipSlot == "primary")
    {
        m_equipSlot = PRIMARY;
        return;
    }if (newEquipSlot == "chest")
    {
        m_equipSlot = CHEST;
        return;
    }if (newEquipSlot == "waist")
    {
        m_equipSlot = WAIST;
        return;
    }if (newEquipSlot == "legs")
    {
        m_equipSlot = LEGS;
        return;
    }if (newEquipSlot == "feet")
    {
        m_equipSlot = FEET;
        return;
    }
}

//*******************************************************************
void Item::Update()
{
}

//*******************************************************************
void Item::Render() const
{
}