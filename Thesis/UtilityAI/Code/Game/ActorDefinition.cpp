#include "Game/ActorDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//*******************************************************************
ActorDefinition::ActorDefinition()
    : m_name("none")
{
}

//*******************************************************************
ActorDefinition::ActorDefinition(const tinyxml2::XMLElement& xmlElement)
    : m_name("none")
{
    m_name = ParseXmlAttribute(xmlElement, "name", m_name);

    m_currentHealth = ParseXmlAttribute(xmlElement, "health", m_currentHealth);
    m_totalHealth = m_currentHealth;

    m_damage = ParseXmlAttribute(xmlElement, "damage", m_damage);
    m_defense = ParseXmlAttribute(xmlElement, "defense", m_defense);

    Rgba fillColor;
    fillColor = ParseXmlAttribute(xmlElement, "fillcolor", fillColor);
    m_fillColors.push_back(fillColor);

    Rgba glyphColor;
    glyphColor = ParseXmlAttribute(xmlElement, "glyphcolor", glyphColor);
    m_glyphColors.push_back(glyphColor);

    char glyph = ' ';
    glyph = ParseXmlAttribute(xmlElement, "glyph", glyph);
    m_glyphs.push_back(glyph);

    m_faction = ParseXmlAttribute(xmlElement, "faction", m_faction);

    m_totalWeight = ParseXmlAttribute(xmlElement, "totalweight", m_totalWeight);

    PushBehaviors(xmlElement);

    s_actorGlyphsAndTheirRespectiveNames[glyph] = m_name;
}

//*******************************************************************
ActorDefinition::~ActorDefinition()
{
}

//*******************************************************************
ActorDefinition* ActorDefinition::GetActorDefinition(const std::string& actorName)
{
    return s_ActorDefinitions[actorName];
}

//*******************************************************************
std::string ActorDefinition::GetActorTypeFromGlyph(char glyph)
{
    return s_actorGlyphsAndTheirRespectiveNames[glyph];
}

//*******************************************************************
void ActorDefinition::PushActorDefinition(ActorDefinition* ActorDefinition)
{
    if (s_ActorDefinitions.find(ActorDefinition->m_name) == s_ActorDefinitions.end())
    {
        s_ActorDefinitions[ActorDefinition->m_name] = ActorDefinition;
    }
    else
    {
        ASSERT_OR_DIE(true, "Multiple actor definitions!");
    }
}

//*******************************************************************
void ActorDefinition::PushBehaviors(const tinyxml2::XMLElement& xmlElement)
{
    const tinyxml2::XMLElement* behaviorsElement = xmlElement.FirstChildElement("Behaviors");

    for (auto xml_behavior = behaviorsElement->FirstChildElement("Behavior"); xml_behavior != nullptr; xml_behavior = xml_behavior->NextSiblingElement())
    {
        Behavior* currentBehavior = Behavior::Create(*xml_behavior);
        m_behaviors.push_back(currentBehavior);
    }

}

//*******************************************************************
void ActorDefinition::ParseFromXML(const std::string& fileName)
{
    tinyxml2::XMLDocument xmlDoc;
    //tinyxml2::XMLError eResult = xmlDoc.LoadFile("Data/Gameplay/Tiles.xml");
    tinyxml2::XMLError eResult = xmlDoc.LoadFile(fileName.c_str());
    ASSERT_OR_DIE(eResult == tinyxml2::XML_SUCCESS, "Error in loading file from file, could not find Actors.xml file\n");

    auto root = xmlDoc.RootElement();
    ASSERT_OR_DIE(root != nullptr, "Error in loading file from file, could not find root\n");

    for (auto xml_actorDefintion = root->FirstChildElement("ActorDefinition"); xml_actorDefintion != nullptr; xml_actorDefintion = xml_actorDefintion->NextSiblingElement())
    {
        ActorDefinition* actorDefinition = new ActorDefinition(*xml_actorDefintion);
        ActorDefinition::PushActorDefinition(actorDefinition);
    }
}