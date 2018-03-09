#pragma once
#include "Engine/Core/XMLUtils.hpp"
#include <map>
#include <vector>

class ResourceDefinition
{
public:
    std::string m_name;
    float m_weight;
    std::string m_type;
    int m_numberOfUses;

public:
    ResourceDefinition();
    ResourceDefinition(const tinyxml2::XMLElement& xmlElement);
    ~ResourceDefinition();

public:
    static ResourceDefinition* GetResourceDefinition(const std::string& resourceName);
    static void PushResourceDefinition(ResourceDefinition* resourceDefinition);
    static void ParseFromXML(const std::string& fileName);
};

static std::map<std::string, ResourceDefinition*> s_resourceDefinitions;