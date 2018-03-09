#include "Game/ResourceDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//*******************************************************************
ResourceDefinition::ResourceDefinition()
    : m_name("none")
{}

//*******************************************************************
ResourceDefinition::ResourceDefinition(const tinyxml2::XMLElement& xmlElement)
    : m_name("none")
{
    m_name = ParseXmlAttribute(xmlElement, "name", m_name);
    m_weight = ParseXmlAttribute(xmlElement, "weight", m_weight);
    m_type = ParseXmlAttribute(xmlElement, "type", m_type);
    m_numberOfUses = ParseXmlAttribute(xmlElement, "uses", m_numberOfUses);
}

//*******************************************************************
ResourceDefinition::~ResourceDefinition()
{}

//*******************************************************************
ResourceDefinition* ResourceDefinition::GetResourceDefinition(const std::string& resourceName)
{
    return s_resourceDefinitions[resourceName];
}

//*******************************************************************
void ResourceDefinition::PushResourceDefinition(ResourceDefinition* resourceDefinition)
{
    if (s_resourceDefinitions.find(resourceDefinition->m_name) == s_resourceDefinitions.end())
    {
        s_resourceDefinitions[resourceDefinition->m_name] = resourceDefinition;
    }
    else
    {
        ASSERT_OR_DIE(true, "Multiple resource definitions!");
    }
}

//*******************************************************************
void ResourceDefinition::ParseFromXML(const std::string& fileName)
{
    tinyxml2::XMLDocument xmlDoc;
    //tinyxml2::XMLError eResult = xmlDoc.LoadFile("Data/Gameplay/Tiles.xml");
    tinyxml2::XMLError eResult = xmlDoc.LoadFile(fileName.c_str());
    ASSERT_OR_DIE(eResult == tinyxml2::XML_SUCCESS, "Error in loading file from file, could not find Resources.xml file\n");

    auto root = xmlDoc.RootElement();
    ASSERT_OR_DIE(root != nullptr, "Error in loading features from file, could not find root\n");

    for (auto xml_resourceDefintion = root->FirstChildElement("ResourceDefinition"); xml_resourceDefintion != nullptr; xml_resourceDefintion = xml_resourceDefintion->NextSiblingElement())
    {
        ResourceDefinition* resourceDefinition = new ResourceDefinition(*xml_resourceDefintion);
        ResourceDefinition::PushResourceDefinition(resourceDefinition);
    }
}
