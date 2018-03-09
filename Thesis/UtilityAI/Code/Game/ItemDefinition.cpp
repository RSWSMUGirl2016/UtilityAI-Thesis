#include "Game/ItemDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//*******************************************************************
ItemDefinition::ItemDefinition()
    : m_name("none")
{
}

//*******************************************************************
ItemDefinition::ItemDefinition(const tinyxml2::XMLElement& xmlElement)
    : m_name("none")
{

    m_name = ParseXmlAttribute(xmlElement, "name", m_name);
    m_equipSlot = ParseXmlAttribute(xmlElement, "equipslot", m_equipSlot);
    m_instantUse = ParseXmlAttribute(xmlElement, "instantuse", m_instantUse);
    m_health = ParseXmlAttribute(xmlElement, "health", m_health);
    m_defense = ParseXmlAttribute(xmlElement, "defense", m_defense);
    m_damage = ParseXmlAttribute(xmlElement, "damage", m_damage);
    m_weight = ParseXmlAttribute(xmlElement, "weight", m_weight);

    Rgba fillColor;
    fillColor = ParseXmlAttribute(xmlElement, "fillcolor", fillColor);
    m_fillColors.push_back(fillColor);

    Rgba glyphColor;
    glyphColor = ParseXmlAttribute(xmlElement, "glyphcolor", glyphColor);
    m_glyphColors.push_back(glyphColor);

    char glyph = ' ';
    glyph = ParseXmlAttribute(xmlElement, "glyph", glyph);
    m_glyphs.push_back(glyph);
}

//*******************************************************************
ItemDefinition::~ItemDefinition()
{
}

//*******************************************************************
ItemDefinition* ItemDefinition::GetItemDefinition(const std::string& actorName)
{
    return s_itemDefinitions[actorName];
}

//*******************************************************************
void ItemDefinition::PushItemDefinition(ItemDefinition* ItemDefinition)
{
    if (s_itemDefinitions.find(ItemDefinition->m_name) == s_itemDefinitions.end())
    {
        s_itemDefinitions[ItemDefinition->m_name] = ItemDefinition;
    }
    else
    {
        ASSERT_OR_DIE(true, "Multiple item definitions!");
    }
}

//*******************************************************************
void ItemDefinition::ParseFromXML(const std::string& fileName)
{
    tinyxml2::XMLDocument xmlDoc;
    //tinyxml2::XMLError eResult = xmlDoc.LoadFile("Data/Gameplay/Tiles.xml");
    tinyxml2::XMLError eResult = xmlDoc.LoadFile(fileName.c_str());
    ASSERT_OR_DIE(eResult == tinyxml2::XML_SUCCESS, "Error in loading file from file, could not find Items.xml file\n");

    auto root = xmlDoc.RootElement();
    ASSERT_OR_DIE(root != nullptr, "Error in loading item from file, could not find root\n");

    for (auto xml_itemDefintion = root->FirstChildElement("ItemDefinition"); xml_itemDefintion != nullptr; xml_itemDefintion = xml_itemDefintion->NextSiblingElement())
    {
        ItemDefinition* itemDefinition = new ItemDefinition(*xml_itemDefintion);
        ItemDefinition::PushItemDefinition(itemDefinition);
    }
}