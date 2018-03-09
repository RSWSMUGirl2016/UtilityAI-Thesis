#include "Game/FeatureDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//*******************************************************************
FeatureDefinition::FeatureDefinition()
    : m_type("none")
{
}

//*******************************************************************
FeatureDefinition::FeatureDefinition(const tinyxml2::XMLElement& xmlElement)
    : m_type("none")
{
    m_type = ParseXmlAttribute(xmlElement, "type", m_type);
    m_subType = ParseXmlAttribute(xmlElement, "subtype", m_subType);
    m_isSolid = ParseXmlAttribute(xmlElement, "issolid", m_isSolid);
    m_fillColor = ParseXmlAttribute(xmlElement, "fillcolor", m_fillColor);
    m_gylphColor = ParseXmlAttribute(xmlElement, "glyphcolor", m_gylphColor);
    m_glyph = ParseXmlAttribute(xmlElement, "glyph", m_glyph);
}

//*******************************************************************
FeatureDefinition::~FeatureDefinition()
{
}

//*******************************************************************
FeatureDefinition* FeatureDefinition::GetFeatureDefinition(const std::string& lootName)
{
    return s_featureDefinitions[lootName];
}

//*******************************************************************
void FeatureDefinition::PushFeatureDefinition(FeatureDefinition* featureDefinition)
{
    if (s_featureDefinitions.find(featureDefinition->m_type) == s_featureDefinitions.end())
    {
        s_featureDefinitions[featureDefinition->m_type] = featureDefinition;
    }
    else
    {
        ASSERT_OR_DIE(true, "Multiple feature definitions!");
    }
}

//*******************************************************************
void FeatureDefinition::ParseFromXML(const std::string& fileName)
{
    tinyxml2::XMLDocument xmlDoc;
    //tinyxml2::XMLError eResult = xmlDoc.LoadFile("Data/Gameplay/Tiles.xml");
    tinyxml2::XMLError eResult = xmlDoc.LoadFile(fileName.c_str());
    ASSERT_OR_DIE(eResult == tinyxml2::XML_SUCCESS, "Error in loading file from file, could not find Items.xml file\n");

    auto root = xmlDoc.RootElement();
    ASSERT_OR_DIE(root != nullptr, "Error in loading features from file, could not find root\n");

    for (auto xml_featureDefintion = root->FirstChildElement("FeatureDefinition"); xml_featureDefintion != nullptr; xml_featureDefintion = xml_featureDefintion->NextSiblingElement())
    {
        FeatureDefinition* featureDefinition = new FeatureDefinition(*xml_featureDefintion);
        FeatureDefinition::PushFeatureDefinition(featureDefinition);
    }
}