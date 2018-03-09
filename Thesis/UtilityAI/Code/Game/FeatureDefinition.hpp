#pragma once
#include "Engine/Core/XMLUtils.hpp"
#include <map>
#include <vector>

class FeatureDefinition
{
public:
    FeatureDefinition();
    FeatureDefinition(const tinyxml2::XMLElement& xmlElement);
    ~FeatureDefinition();

    static FeatureDefinition* GetFeatureDefinition(const std::string& lootName);
    static void PushFeatureDefinition(FeatureDefinition* featureDefinition);
    static void ParseFromXML(const std::string& fileName);

    std::string m_type;
    std::string m_subType;
    bool m_isSolid;
    Rgba m_gylphColor;
    Rgba m_fillColor;
    char m_glyph;
};

static std::map<std::string, FeatureDefinition*> s_featureDefinitions;